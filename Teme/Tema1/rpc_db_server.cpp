#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include <string.h>

#include "rpc_db.h"

#include "utils.h"


struct data_t {
	size_t len;
	float *values;
};

struct client_t {
	std::string user;
	std::unordered_map<int, data_t> data;

	client_t() {}
	client_t(const std::string &user): user(user) { }
};

static u_long key;
static std::unordered_map<u_long, std::string> sessions;
static std::unordered_set<std::string> active_users;
static std::unordered_map<u_long, client_t> db;


u_long *login_1_svc(char **user, struct svc_req *cl)
{
	std::string user_str;
	static u_long crt_key;

	// TODO: Vezi de ce nu-i palce ULONG_MAX
	crt_key = UINT_MAX;

	ASSERT(!user || !*user, "SERVER", "Received invalid user.",
		return &crt_key);

	user_str = std::string(*user);
	ASSERT(
		active_users.find(user_str) !=  active_users.end(),
		"SERVER",
		("User " + user_str + " is already logged in.").c_str(),
		return &crt_key
	);

	sessions[key] = user_str;
	db[key] = client_t(user_str);
	active_users.insert(user_str);

	crt_key = key++;

	return &crt_key;
}

response_t *logout_1_svc(u_long *key, struct svc_req *cl)
{
	static response_t resp = ERROR;
	std::string user;

	ASSERT(!key, "SERVER", "Received invalid session key.", return &resp);
	ASSERT(
		sessions.find(*key) == sessions.end(),
		"SERVER",
		("Session key " + std::to_string(*key) + " not found").c_str(),
		return &resp;
	);

	active_users.erase(sessions[*key]);
	sessions.erase(*key);
	db.erase(*key);

	resp = OK;
	return &resp;
}
