#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>

#include <string.h>

#include "rpc_db.h"

#include "utils.h"


struct data_t {
	u_int len;
	float min, max, avg, med;
	float *values;

	data_t() { }

	data_t(u_int l, float *v): len(l), min(INFINITY), max(-INFINITY), avg(0.f)
	{
		values = new float[l];
		for (u_int i = 0; i != l; ++i) {
			values[i] = v[i];

			min = min > v[i] ? v[i] : min;
			max = max < v[i] ? v[i] : max;
			avg += v[i];
		}

		avg /= l;

		std::nth_element(v, v + l / 2, v + l);
		if (l & 1)
			med = v[l / 2];
		else
			med = (v[l / 2] + *std::max_element(v, v + l / 2)) / 2;
	}

	data_t(data_t &&other)
	{
		len = other.len;
		other.len = 0;

		min = other.min;
		other.min = 0;

		max = other.max;
		other.max = 0;

		avg = other.avg;
		other.avg = 0;

		med = other.med;
		other.med = 0;

		values = other.values;
		other.values = nullptr;
	}

	~data_t()
	{
		if (values != nullptr) {
			delete[] values;
			values = nullptr;
		}
	}
};

struct client_t {
	u_int num_elem;
	float min, max, avg, med;
	std::string user;
	std::unordered_map<int, data_t> data;

	client_t() {}
	client_t(const std::string &user):
		user(user), min(INFINITY), max(-INFINITY), avg(0.f), num_elem(0) { }

	void update_stats(int id)
	{
		auto &data_entry = data[id];
		u_int new_num_elem = num_elem + data_entry.len;

		min = min > data_entry.min ? data_entry.min : min;
		max = max < data_entry.max ? data_entry.max : max;
		avg = (avg * num_elem + data_entry.avg * data_entry.len) / new_num_elem;
		num_elem = new_num_elem;

		update_median();
	}

private:
	void update_median()
	{
		std::vector<float> all_vals;
		size_t total_count;

		for (const auto &entry : data)
			all_vals.insert(
				all_vals.end(),
				entry.second.values,
				entry.second.values + entry.second.len
			);

		total_count = all_vals.size();
		std::nth_element(
			all_vals.begin(),
			all_vals.begin() + total_count / 2,
			all_vals.end()
		);

		if (total_count & 1)
			med = all_vals[total_count / 2];
		else
			med = (
				all_vals[total_count / 2]
				+ *std::max_element(
					all_vals.begin(),
					all_vals.begin() + total_count / 2
				)
			) / 2;
	}
};

static u_long key;
static std::unordered_set<std::string> active_users;
static std::unordered_map<u_long, client_t> db;


u_long *login_1_svc(char **user, struct svc_req *cl)
{
	std::string user_str;
	static u_long crt_key;

	crt_key = INVALID_KEY;

	ASSERT(!user || !*user, "SERVER", "Received invalid user.",
		return &crt_key);

	user_str = std::string(*user);
	ASSERT(
		active_users.find(user_str) !=  active_users.end(),
		"SERVER",
		("User " + user_str + " is already logged in.").c_str(),
		return &crt_key
	);

	db[++key] = std::move(client_t(user_str));
	active_users.emplace(std::move(user_str));

	return &key;
}

response_t *logout_1_svc(u_long *key, struct svc_req *cl)
{
	static response_t resp;
	std::string user;

	resp =  ERROR;

	ASSERT(!key, "SERVER", "Received invalid session key.", return &resp);
	ASSERT(
		db.find(*key) == db.end(),
		"SERVER",
		("Session key " + std::to_string(*key) + " not found").c_str(),
		return &resp;
	);

	active_users.erase(db[*key].user);
	db.erase(*key);

	resp = OK;
	return &resp;
}

response_t *add_update_1_svc(values_request_t *req, struct svc_req *cl)
{
	static response_t resp;
	resp =  ERROR;

	ASSERT(!req, "SERVER", "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "SERVER", "Unknown session key", return &resp);

	auto &data = cli_it->second.data;
	if (UPDATE == req->method)
		ASSERT(
			data.find(req->id) == data.end(),
			"SERVER",
			("Data id " + std::to_string(req->id)
				+ " doesn't exist in the database").c_str(),
			return &resp
		);
	else
		ASSERT(
			data.find(req->id) != data.end(),
			"SERVER",
			("Data id " + std::to_string(req->id)
				+ " already exists in the database").c_str(),
			return &resp
		);

	data.emplace(
		req->id,
		data_t(req->values.values_len, req->values.values_val)
	);

	cli_it->second.update_stats(req->id);

	resp = OK;
	return &resp;
}

read_response_t *read_entry_1_svc(entry_request_t *req, struct svc_req *cl)
{
	static read_response_t resp;

	resp.status = ERROR;
	free(resp.data.data_val);
	memset(&resp.data, 0, sizeof(resp.data));

	ASSERT(!req, "SERVER", "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "SERVER", "Unknown session key", return &resp);

	auto data_it = cli_it->second.data.find(req->id);
	ASSERT(
		data_it == cli_it->second.data.end(),
		"SERVER",
		("Data id " + std::to_string(req->id)
			+ " doesn't exist in the database").c_str(),
		return &resp
	);

	resp.data.data_val = new float[data_it->second.len];
	ASSERT(!resp.data.data_val, "SERVER", "Memory allocation failed",
		perror("read_entry"); return &resp);

	memcpy(
		resp.data.data_val,
		data_it->second.values,
		data_it->second.len * sizeof(*resp.data.data_val)
	);

	resp.data.data_len = data_it->second.len;
	resp.status = OK;

	return &resp;
}

response_t *del_1_svc(entry_request_t *req, struct svc_req *cl)
{
	static response_t resp;
	int num_del;

	resp = ERROR;

	ASSERT(!req, "SERVER", "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "SERVER", "Unknown session key", return &resp);

	num_del = cli_it->second.data.erase(req->id);
	ASSERT(
		!num_del,
		"SERVER",
		("Data id " + std::to_string(req->id) + " not found").c_str(),
		return &resp
	);

	resp = OK;
	return &resp;
}

load_response_t *load_1_svc(u_long *key, struct svc_req *cl)
{
	static load_response_t resp;
	int id;
	size_t num_entries;
	float min, max, avg, med;
	data_t file_data;

	if (resp.ids.ids_val)
		free(resp.ids.ids_val);

	resp.status = ERROR;
	memset(&resp.ids, 0, sizeof(resp.ids));

	ASSERT(
		!key || INVALID_KEY == *key,
		"SERVER",
		"Incorrect session key",
		return &resp
	);

	auto cli_it = db.find(*key);
	ASSERT(
		cli_it == db.end(),
		"SERVER",
		("Session key " + std::to_string(*key)
			+ " doesn't exist in database").c_str(),
		return &resp;
	);
	ASSERT(
		!cli_it->second.data.empty(),
		"SERVER",
		("The client " + cli_it->second.user
			+ " has already made changes to their database").c_str(),
		return &resp;
	);

	std::ifstream in(cli_it->second.user + ".db", std::ios::binary);

	in.read((char *)&num_entries, sizeof(num_entries));
	ASSERT(
		!num_entries,
		"SERVER",
		("The database file of user " + cli_it->second.user
			+ " is empty").c_str(),
		return &resp
	);

	in.read((char *)&cli_it->second.min, sizeof(cli_it->second.min));
	in.read((char *)&cli_it->second.max, sizeof(cli_it->second.max));
	in.read((char *)&cli_it->second.avg, sizeof(cli_it->second.avg));
	in.read((char *)&cli_it->second.med, sizeof(cli_it->second.med));

	resp.ids.ids_val = new int[num_entries];
	ASSERT(
		!resp.ids.ids_val,
		"SERVER",
		"Memory allocation failed",
		perror("new"); return &resp
	);

	for (size_t i = 0; i != num_entries; ++i) {
		in.read((char *)&id, sizeof(id));
		in.read((char *)&file_data.len, sizeof(file_data.len));
		in.read((char *)&file_data.min, sizeof(file_data.min));
		in.read((char *)&file_data.max, sizeof(file_data.max));
		in.read((char *)&file_data.avg, sizeof(file_data.avg));
		in.read((char *)&file_data.med, sizeof(file_data.med));

		file_data.values = new float[file_data.len];
		ASSERT(
			!file_data.values,
			"SERVER",
			"Memory allocation failed",
			perror("new"); return &resp
		);

		for (u_int j = 0; j != file_data.len; ++j)
			in.read((char *)(file_data.values + j), sizeof(*file_data.values));

		cli_it->second.data.emplace(id, std::move(file_data));
		resp.ids.ids_val[i] = id;
	}

	in.close();

	resp.ids.ids_len = num_entries;
	resp.status = OK;

	return &resp;
}

response_t *store_1_svc(u_long *key, struct svc_req *cl)
{
	static response_t resp;
	size_t num_entries;

	resp = ERROR;

	ASSERT(
		!key || INVALID_KEY == *key,
		"SERVER",
		"Incorrect session key",
		return &resp
	);

	auto cli_it = db.find(*key);
	ASSERT(
		cli_it == db.end(),
		"SERVER",
		("Session key " + std::to_string(*key)
			+ " doesn't exist in database").c_str(),
		return &resp;
	);
	ASSERT(
		cli_it->second.data.empty(),
		"SERVER",
		("The client " + cli_it->second.user + "'s database is empty").c_str(),
		return &resp;
	);

	std::ofstream out(cli_it->second.user + ".db", std::ios::binary);

	num_entries = cli_it->second.data.size();
	out.write((char *)&num_entries, sizeof(num_entries));
	out.write((char *)&cli_it->second.min, sizeof(cli_it->second.min));
	out.write((char *)&cli_it->second.max, sizeof(cli_it->second.max));
	out.write((char *)&cli_it->second.avg, sizeof(cli_it->second.avg));
	out.write((char *)&cli_it->second.med, sizeof(cli_it->second.med));

	for (const auto &entry : cli_it->second.data) {
		out.write((char *)&entry.first, sizeof(entry.first));
		out.write((char *)&entry.second.len, sizeof(entry.second.len));
		out.write((char *)&entry.second.min, sizeof(entry.second.min));
		out.write((char *)&entry.second.max, sizeof(entry.second.max));
		out.write((char *)&entry.second.avg, sizeof(entry.second.avg));
		out.write((char *)&entry.second.med, sizeof(entry.second.med));

		for (u_int i = 0; i != entry.second.len; ++i)
			out.write(
				(char *)(entry.second.values + i),
				sizeof(*entry.second.values)
			);
	}

	out.close();

	resp = OK;
	return &resp;
}

get_response_t *get_stats_1_svc(entry_request_t *req, struct svc_req *cl)
{
	static get_response_t resp;
	memset(&resp, 0, sizeof(resp));
	resp.status = ERROR;

	ASSERT(!req, "SERVER", "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "SERVER", "Unknown session key", return &resp);

	auto data_it = cli_it->second.data.find(req->id);
	ASSERT(
		data_it == cli_it->second.data.end(),
		"SERVER",
		("Data id " + std::to_string(req->id)
			+ " doesn't exist in the database").c_str(),
		return &resp
	);

	resp.min = data_it->second.min;
	resp.max = data_it->second.max;
	resp.avg = data_it->second.avg;
	resp.med = data_it->second.med;

	resp.status = OK;
	return &resp;
}

get_response_t *get_stats_all_1_svc(u_long *key, svc_req *cl)
{
	static get_response_t resp;
	memset(&resp, 0, sizeof(resp));
	resp.status = ERROR;

	ASSERT(!key, "SERVER", "Incorrect request", return &resp);

	auto cli_it = db.find(*key);
	ASSERT(cli_it == db.end(), "SERVER", "Unknown session key", return &resp);

	resp.min = cli_it->second.min;
	resp.max = cli_it->second.max;
	resp.avg = cli_it->second.avg;
	resp.med = cli_it->second.med;

	resp.status = OK;
	return &resp;
}
