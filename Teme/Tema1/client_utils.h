#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_ 1

#include <string>
#include <sstream>

#include "rpc_db.h"


u_long login_cmd(std::istringstream &iss, CLIENT *handle);

response_t logout_cmd(u_long key, CLIENT *client);

response_t add_update_cmd(
	const u_long key,
	std::istringstream &iss,
	CLIENT *handle,
	method_t method
);

read_response_t *read_cmd(u_long key, std::istringstream &iss, CLIENT *client);

#endif  /* CLIENT_UTILS_H_ */
