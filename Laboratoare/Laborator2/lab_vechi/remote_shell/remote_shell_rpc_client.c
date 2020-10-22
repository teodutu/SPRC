#include <stdio.h>
#include <string.h>
#include <time.h>
#include <rpc/rpc.h>

#include "remote_shell.h"

#define PROTOCOL "tcp"

static int
create_shell_cmd(struct shell_data *cmd, int argc, char const *argv[])
{
	int i;
	size_t chars_left = sizeof(cmd->data) - 1;
	size_t crt_len;

	cmd->response_len = atoi(argv[0]);
	if (cmd->response_len <= 0 || cmd->response_len >= sizeof(cmd->data)) {
		fprintf(stderr, "Incorrect response length: %s\n", argv[argc - 1]);
		return -1;
	}

	cmd->data[chars_left] = '\0';
	for (i = 1; i != argc; ++i, chars_left -= crt_len) {
		crt_len = strlen(argv[i]);

		if (chars_left < crt_len)
			break;

		strcat(cmd->data, argv[i]);
		if (chars_left - crt_len > 1)
			strcat(cmd->data, " ");
		chars_left -= crt_len + 1;
	}

	return 0;
}

int main(int argc, char const *argv[])
{
	int ret;
	CLIENT *handle;
	struct shell_data cmd, *resp;

	if (argc < 4) {
		fprintf(stderr,
			"Usage:\n\t%s <SERVER_ADDRESS> <RESPONSE_LENGTH> <SHELL_COMMAND>\n",
			argv[0]);
		return -1;
	}

	handle = clnt_create(argv[1], REMOTE_SHELL_PROG, REMOTE_SHELL_VERS,
		PROTOCOL);
	if (!handle) {
		perror("Failed to create client handle");
		clnt_pcreateerror(argv[0]);
		return -2;
	}

	ret = create_shell_cmd(&cmd, argc - 2, argv + 2);
	if (ret < 0)
		return -3;

	resp = run_cmd_1(&cmd, handle);
	if (!resp || !resp->response_len) {
		fprintf(stderr, "RPC failed!\n");
		return -4;
	}

	printf("%s", resp->data);

	clnt_destroy(handle);

	return 0;
}
