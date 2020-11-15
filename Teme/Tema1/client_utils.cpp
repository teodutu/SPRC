#include "client_utils.h"
#include "utils.h"

u_long login_cmd(std::istringstream &iss, CLIENT *handle)
{
	std::string user;
	u_long *key;
	char *user_c_str;

	iss >> user;
	user_c_str = (char *)user.c_str();

	key = login_1(&user_c_str, handle);
	ASSERT(!key, "CLIENT", "Login filed",
		clnt_perror(handle, ""); return UINT_MAX);

	return *key;
}

response_t logout_cmd(const u_long key, CLIENT *handle)
{
	response_t *resp = logout_1((u_long *)&key, handle);
	ASSERT(!resp, "CLIENT", "Logout failed",
		clnt_perror(handle, ""); return ERROR);

	return *resp;
}

response_t add_update_cmd(
	const u_long key,
	std::istringstream &iss,
	CLIENT *handle,
	method_t method
) {
	values_request_t req;
	response_t *resp;

	req.sess_key = key;
	req.method = method;

	iss >> req.id >> req.values.values_len;
	ASSERT(
		!req.id || !req.values.values_len || !handle,
		"CLIENT",
		"Incorrect GET command",
		return ERROR
	);

	req.values.values_val = new float[req.values.values_len];
	ASSERT(
		!req.values.values_val,
		"CLIENT",
		"Failed to allocate data array",
		clnt_perror(handle, ""); return ERROR
	);

	for (u_int i = 0; i != req.values.values_len; ++i)
		iss >> req.values.values_val[i];

	resp = add_update_1(&req, handle);
	ASSERT(!resp, "CLIENT", "Failed RPC",
		clnt_perror(handle, "add"); return ERROR);

	return *resp;
}

read_response_t *read_cmd(u_long key, std::istringstream &iss, CLIENT *handle)
{
	entry_request_t req;

	ASSERT(!key || !handle, "CLIENT", "Incorrect parameter", return NULL);

	req.sess_key = key;
	iss >> req.id;

	return read_entry_1(&req, handle);
}

response_t del_cmd(u_long key, std::istringstream &iss, CLIENT *handle)
{
	entry_request_t req;

	ASSERT(!key || !handle, "CLIENT", "Incorrect parameter", return ERROR);

	req.sess_key = key;
	iss >> req.id;

	return *del_1(&req, handle);
}

get_response_t *get_stats_cmd(
	u_long key,
	std::istringstream &iss,
	CLIENT *handle
) {
	entry_request_t req;

	ASSERT(!key || !handle, "CLIENT", "Incorrect parameter", return NULL);

	req.sess_key = key;
	iss >> req.id;

	return get_stats_1(&req, handle);
}
