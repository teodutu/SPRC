#include <stdio.h>
#include <rpc/rpc.h>

#include "check.h"

#define PROTOCOL "tcp"
#define SERVER_ADDRESS "sprc2.dfilip.xyz"

int main(int argc, char const *argv[])
{
	CLIENT *handle;
	char **resp;
	struct student stud;

	if (argc != 3) {
		fprintf(stderr, "Usage:\n\t%s <STUDENT_NAME> <STUDENT_GROUP>\n",
			argv[0]);
		return -1;
	}

	handle = clnt_create(SERVER_ADDRESS, CHECK_PROG, CHECK_VERS, PROTOCOL);
	if (!handle) {
		perror("Failed to create client handle");
		clnt_pcreateerror(argv[0]);
		return -2;
	}

	stud.nume = (char *)argv[1];
	stud.grupa = (char *)argv[2];

	resp = grade_1(&stud, handle);
	if (!resp) {
		perror("RPC failed");
		return -3;
	}
	printf("RPC server esponse:\n\t%s", *resp);

	clnt_destroy(handle);
	free(*resp);
	xdr_free((xdrproc_t)xdr_char, (char *)resp);

	return 0;
}
