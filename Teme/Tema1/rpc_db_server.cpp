#include <iostream>
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
	static response_t resp = ERROR;
	std::string user;

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
	static response_t resp = ERROR;

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

	std::cout << data[req->id].len << '\n';
	std::cout << data[req->id].values[0] << "\n\n";

	resp = OK;
	return &resp;
}

read_response_t *read_1_svc(read_request_t *req, struct svc_req *cl)
{
	static read_response_t resp = { .status = ERROR, .data = { 0 } };

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

	free(resp.data.data_val);

	resp.data.data_val = new float[data_it->second.len];
	ASSERT(!resp.data.data_val, "SERVER", "Memory allocation failed",
		perror("read"); return &resp);

	memcpy(
		resp.data.data_val,
		data_it->second.values,
		data_it->second.len * sizeof(*resp.data.data_val)
	);

	resp.status = OK;
	resp.data.data_len = data_it->second.len;

	return &resp;
}
