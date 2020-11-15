#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

#include "rpc_db.h"

#include "client_utils.h"
#include "utils.h"


#define SERVER_ADDRESS		"localhost"
#define RPC_DB_PROG			1
#define RPC_DB_VERS			1
#define PROTOCOL			"tcp"

#define EXIT_CMD			"exit"
#define LOGIN_CMD			"login"
#define LOGOUT_CMD			"logout"
#define ADD_CMD				"add"
#define UPDATE_CMD			"update"
#define DEL_CMD				"del"
#define READ_CMD			"read"
#define LOAD_CMD			"load"
#define STORE_CMD			"store"
#define GET_STAT_CMD		"get_stat"
#define GET_STAT_ALL_CMD	"get_stat_all"


int main(void)
{
	CLIENT *handle;
	response_t resp;
	response_t *resp_ptr;
	get_response_t *get_resp;
	read_response_t *read_resp;
	load_response_t *load_resp;
	u_long key;
	bool logged_in = false;
	std::string input;
	std::string cmd;
	std::istringstream iss;

	handle = clnt_create(SERVER_ADDRESS, RPC_DB_PROG, RPC_DB_VERS, PROTOCOL);
	ASSERT(
		!handle,
		"CLIENT",
		"Failed to create client handle",
		clnt_pcreateerror(""); return -1;
	);

read_input:
	while (std::getline(std::cin, input)) {
		iss = std::istringstream(input);

		iss >> cmd;

		if (!cmd.compare(EXIT_CMD))
			break;
		if (!cmd.compare(LOGIN_CMD)) {
			if (logged_in) {
				std::cerr << "Already logged in!\n";
				continue;
			}

			key = login_cmd(iss, handle);
			if (INVALID_KEY != key)
				logged_in = true;
		} else if (!cmd.compare(LOGOUT_CMD)) {
			resp_ptr = logout_1(&key, handle);
			ASSERT(
				!resp_ptr || ERROR == *resp_ptr,
				"CLIENT",
				"LOGOUT command failed",
				clnt_perror(handle, "")
			);

			logged_in = false;
		} else if (!cmd.compare(ADD_CMD)) {
			resp = add_update_cmd(key, iss, handle, ADD);
			ASSERT(ERROR == resp, "CLIENT", "ADD command failed", ;);
		} else if (!cmd.compare(UPDATE_CMD)) {
			resp = add_update_cmd(key, iss, handle, UPDATE);
			ASSERT(ERROR == resp, "CLIENT", "UPDATE command failed", ;);
		} else if (!cmd.compare(READ_CMD)) {
			read_resp = read_cmd(key, iss, handle);
			ASSERT(
				!read_resp || ERROR == read_resp->status,
				"CLIENT",
				"READ command failed",
				clnt_perror(handle, ""); goto read_input
			);

			std::cout << "Data: ";
			for (u_int i = 0; i != read_resp->data.data_len; ++i)
				std::cout << read_resp->data.data_val[i] << ' ';
			std::cout << '\n';
		} else if (!cmd.compare(DEL_CMD)) {
			resp = del_cmd(key, iss, handle);
			ASSERT(ERROR == resp, "CLIENT", "DEL command failed", ;);
		} else if (!cmd.compare(LOAD_CMD)) {
			load_resp = load_1(&key, handle);
			ASSERT(
				!load_resp || ERROR == load_resp->status,
				"CLIENT",
				"LOAD command failed",
				clnt_perror(handle, ""); continue
			);

			std::cout << "Loaded ids: ";
			for (u_int i = 0; i != load_resp->ids.ids_len; ++i)
				std::cout << load_resp->ids.ids_val[i] << ' ';
			std::cout << '\n';
		} else if (!cmd.compare(STORE_CMD)) {
			resp_ptr = store_1(&key, handle);
			ASSERT(
				!resp_ptr || ERROR == *resp_ptr,
				"CLIENT",
				"STORE command failed",
				clnt_perror(handle, ""); goto read_input
			);
		} else if (!cmd.compare(GET_STAT_CMD)) {
			get_resp = get_stats_cmd(key, iss, handle);
			ASSERT(
				!get_resp || ERROR == get_resp->status,
				"CLIENT",
				"GET_STAT command failed",
				clnt_perror(handle, ""); goto read_input
			);

			std::cout << "Stats: min = " << get_resp->min
				<< "; max = " << get_resp->max
				<< "; average = " << get_resp->avg
				<< "; median = " << get_resp->med << '\n';
		} else if (!cmd.compare(GET_STAT_ALL_CMD)) {
			get_resp = get_stats_all_1(&key, handle);
			ASSERT(
				!get_resp || ERROR == get_resp->status,
				"CLIENT",
				"GET_STAT_ALL command failed",
				clnt_perror(handle, ""); goto read_input
			);

			std::cout << "Stats: min = " << get_resp->min
				<< "; max = " << get_resp->max
				<< "; average = " << get_resp->avg
				<< "; median = " << get_resp->med << '\n';
		} else
			std::cerr << "Unknown command!\n";
	}

	return 0;
}

