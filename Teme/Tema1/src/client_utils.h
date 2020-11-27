#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_ 1

#include <string>
#include <sstream>

#include <unistd.h>

#include "rpc_db.h"


#ifdef __cplusplus
extern "C" {
#endif

u_long login_cmd(std::istringstream &iss, CLIENT *handle);

status_t logout_cmd(u_long key, CLIENT *handle);

status_t add_update_cmd(
	const u_long key,
	std::istringstream &iss,
	CLIENT *handle,
	method_t method
);

read_response_t *read_cmd(u_long key, std::istringstream &iss, CLIENT *handle);

status_t del_cmd(u_long key, std::istringstream &iss, CLIENT *handle);

get_response_t *get_stats_cmd(
	u_long key,
	std::istringstream &iss,
	CLIENT *handle
);

void sleep_cmd(std::istringstream &iss);

#ifdef __cplusplus
}
#endif

#endif  /* CLIENT_UTILS_H_ */
