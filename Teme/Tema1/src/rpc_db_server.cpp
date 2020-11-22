#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>

#include "rpc_db.h"

#include "utils.h"
#include "client_data.h"


static u_long key;
static std::unordered_set<std::string> active_users;
static std::unordered_map<u_long, ClientData> db;


u_long *login_1_svc(char **user, struct svc_req *cl)
{
	std::string user_str;
	static u_long crt_key;

	crt_key = INVALID_KEY;

	ASSERT(!user || !*user, "Received invalid user.", return &crt_key);

	user_str = std::string(*user);
	ASSERT(
		active_users.find(user_str) !=  active_users.end(),
		("User " + user_str + " is already logged in.").c_str(),
		return &crt_key
	);

	db[++key] = std::move(ClientData(user_str));
	active_users.emplace(std::move(user_str));

	return &key;
}

status_t *logout_1_svc(u_long *key, struct svc_req *cl)
{
	static status_t resp;
	std::string user;

	resp =  ERROR;

	ASSERT(!key, "Received invalid session key.", return &resp);
	ASSERT(
		db.find(*key) == db.end(),
		("Session key " + std::to_string(*key) + " not found").c_str(),
		return &resp;
	);

	active_users.erase(db[*key].user);
	db.erase(*key);

	resp = OK;
	return &resp;
}

status_t *add_update_1_svc(add_update_request_t *req, struct svc_req *cl)
{
	static status_t resp;
	resp =  ERROR;

	ASSERT(!req, "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "Unknown session key", return &resp);

	auto &data = cli_it->second.data;
	if (UPDATE == req->method)
		ASSERT(
			data.find(req->id) == data.end(),
			("Data id " + std::to_string(req->id)
				+ " doesn't exist in the database").c_str(),
			return &resp
		);
	else
		ASSERT(
			data.find(req->id) != data.end(),
			("Data id " + std::to_string(req->id)
				+ " already exists in the database").c_str(),
			return &resp
		);

	data[req->id] = DataEntry(req->values.values_len, req->values.values_val);
	cli_it->second.update_stats(req->id);

	resp = OK;
	return &resp;
}

read_response_t *read_entry_1_svc(entry_request_t *req, struct svc_req *cl)
{
	static read_response_t resp;

	resp.status = ERROR;
	delete[] resp.data.data_val;
	memset(&resp.data, 0, sizeof(resp.data));

	ASSERT(!req, "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "Unknown session key", return &resp);

	auto data_it = cli_it->second.data.find(req->id);
	ASSERT(
		data_it == cli_it->second.data.end(),
		("Data id " + std::to_string(req->id)
			+ " doesn't exist in the database").c_str(),
		return &resp
	);

	resp.data.data_val = new float[data_it->second.len];
	ASSERT(!resp.data.data_val, "Memory allocation failed",
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

status_t *del_1_svc(entry_request_t *req, struct svc_req *cl)
{
	static status_t resp;
	int num_del;

	resp = ERROR;

	ASSERT(!req, "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "Unknown session key", return &resp);

	num_del = cli_it->second.data.erase(req->id);
	ASSERT(
		!num_del,
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
	float min, max, avg, med;
	DataEntry file_data;

	resp.status = ERROR;
	delete[] resp.ids.ids_val;
	memset(&resp.ids, 0, sizeof(resp.ids));

	ASSERT(!key || INVALID_KEY == *key, "Incorrect session key", return &resp);

	auto cli_it = db.find(*key);
	ASSERT(
		cli_it == db.end(),
		("Session key " + std::to_string(*key)
			+ " doesn't exist in database").c_str(),
		return &resp;
	);
	ASSERT(
		!cli_it->second.data.empty(),
		("The client " + cli_it->second.user
			+ " has already made changes to their database").c_str(),
		return &resp;
	);

	std::ifstream in(cli_it->second.user + ".rpcdb", std::ios::binary);
	ASSERT(
		!in,
		("Database file of client " + cli_it->second.user
			+ " doesn't exist").c_str(),
		return &resp;
	);

	in.read((char *)&cli_it->second.num_elem, sizeof(cli_it->second.num_elem));
	ASSERT(
		!cli_it->second.num_elem,
		("The database file of user " + cli_it->second.user
			+ " is empty").c_str(),
		return &resp
	);

	cli_it->second.read_stats(in);

	resp.ids.ids_val = new int[cli_it->second.num_elem];
	ASSERT(
		!resp.ids.ids_val,
		"Memory allocation failed",
		perror("new"); return &resp
	);

	for (size_t i = 0; i != cli_it->second.num_elem; ++i) {
		in.read((char *)&id, sizeof(id));
		file_data.read_stats(in);

		file_data.values = new float[file_data.len];
		ASSERT(
			!file_data.values,
			"Memory allocation failed",
			perror("new"); return &resp
		);

		for (u_int j = 0; j != file_data.len; ++j)
			in.read((char *)(file_data.values + j), sizeof(*file_data.values));

		cli_it->second.data.emplace(id, std::move(file_data));
		resp.ids.ids_val[i] = id;
	}

	in.close();

	resp.ids.ids_len = cli_it->second.num_elem;
	resp.status = OK;

	return &resp;
}

status_t *store_1_svc(u_long *key, struct svc_req *cl)
{
	static status_t resp;
	size_t num_entries;

	resp = ERROR;

	ASSERT(!key || INVALID_KEY == *key, "Incorrect session key", return &resp);

	auto cli_it = db.find(*key);
	ASSERT(
		cli_it == db.end(),
		("Session key " + std::to_string(*key)
			+ " doesn't exist in database").c_str(),
		return &resp;
	);
	ASSERT(
		cli_it->second.data.empty(),
		("The client " + cli_it->second.user + "'s database is empty").c_str(),
		return &resp;
	);

	std::ofstream out(cli_it->second.user + ".rpcdb", std::ios::binary);

	num_entries = cli_it->second.data.size();
	out.write((char *)&num_entries, sizeof(num_entries));
	cli_it->second.write_stats(out);

	for (auto &entry : cli_it->second.data) {
		if (!entry.second.len)
			continue;

		out.write((char *)&entry.first, sizeof(entry.first));
		entry.second.write_stats(out);

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

	ASSERT(!req, "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "Unknown session key", return &resp);

	auto data_it = cli_it->second.data.find(req->id);
	ASSERT(
		data_it == cli_it->second.data.end(),
		("Data id " + std::to_string(req->id)
			+ " doesn't exist in the database").c_str(),
		return &resp
	);
	ASSERT(
		!data_it->second.len,
		("There is no data with id " + std::to_string(req->id)).c_str(),
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

	ASSERT(!key, "Incorrect request", return &resp);

	auto cli_it = db.find(*key);
	ASSERT(cli_it == db.end(), "Unknown session key", return &resp);
	ASSERT(!cli_it->second.num_elem, "No entries in database", return &resp);

	resp.min = cli_it->second.min;
	resp.max = cli_it->second.max;
	resp.avg = cli_it->second.avg;
	resp.med = cli_it->second.med;

	resp.status = OK;
	return &resp;
}
