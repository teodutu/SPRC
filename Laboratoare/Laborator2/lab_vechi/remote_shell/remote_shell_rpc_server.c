#include <stdio.h>
#include <time.h>
#include <string.h>
#include <rpc/rpc.h>

#include "remote_shell.h"

struct shell_data *run_cmd_1_svc(struct shell_data *cmd, struct svc_req *cl) {
	static struct shell_data resp = { 0 };
	FILE *proc = popen(cmd->data, "r");

	if (!proc) {
		fprintf(stderr, "Failed launching shell command: %s\n", cmd->data);
		return NULL;
	}

	resp.response_len = fread(resp.data, sizeof(*resp.data),
		MAX(cmd->response_len, sizeof(resp.data) - 1), proc);

	fclose(proc);

	return &resp;
}
