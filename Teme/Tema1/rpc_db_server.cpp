#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <string.h>

#include "rpc_db.h"

#include "utils.h"


// TOOD: foloseste structura din XDR
struct data_t {
	u_int len;
	float *values;

	// TODO: copiaza ptr in loc de memcpy?
	data_t() { }
	data_t(u_int l, float *v): len(l)
	{
		values = new float[l];
		memcpy(values, v, l * sizeof(*values));
	}
};

struct client_t {
	std::string user;
	std::unordered_map<int, data_t> data;

	client_t() {}
	client_t(const std::string &user): user(user) { }
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

	data[req->id] = std::move(data_t(
		req->values.values_len,
		req->values.values_val
	));

	resp = OK;
	return &resp;
}

read_response_t *read_entry_1_svc(read_del_request_t *req, struct svc_req *cl)
{
	static read_response_t resp;

	resp.status = ERROR;
	free(resp.data.data_val);
	memset(&resp.data, 0, sizeof(resp.data));

	ASSERT(!req, "SERVER", "Incorrect request", return &resp);

	auto cli_it = db.find(req->sess_key);
	ASSERT(cli_it == db.end(), "SERVER", "Unknown session key", return &resp);

	auto &data = cli_it->second.data;
	auto data_it = data.find(req->id);

	ASSERT(
		data_it == data.end(),
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

	resp.status = OK;
	resp.data.data_len = data_it->second.len;

	return &resp;
}

response_t *del_1_svc(read_del_request_t *req, struct svc_req *cl)
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

		file_data.values = new float[file_data.len];
		ASSERT(
			!file_data.values,
			"SERVER",
			"Memory allocation failed",
			perror("new"); return &resp
		);

		for (u_int j = 0; j != file_data.len; ++j)
			in.read((char *)(file_data.values + j), sizeof(*file_data.values));

		cli_it->second.data[id] = std::move(file_data);
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

	for (const auto &entry : cli_it->second.data) {
		out.write((char *)&entry.first, sizeof(entry.first));
		out.write((char *)&entry.second.len, sizeof(entry.second.len));

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
