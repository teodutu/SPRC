#include <stdio.h>
#include <time.h>
#include <rpc/rpc.h>

#include "closest_prime.h"

#define PROTOCOL "tcp"

int main(int argc, char const *argv[])
{
	CLIENT *handle;
	struct client_data data;
	int *prime;

	if (argc != 4) {
		fprintf(stderr, "Usage:\n\t%s <CLIENT_NAME> <SERVER_IP> <NUMBER>\n",
			argv[0]);
		return -1;
	}

	handle = clnt_create(argv[2], CLOSEST_PRIME_PROG, CLOSEST_PRIME_VERS,
		PROTOCOL);
	if (!handle) {
		perror("Failed to create client handle");
		return -2;
	}

	data.num = atoi(argv[3]);
	data.client_name[31] = '\0';
	strncpy(data.client_name, argv[1], 31);

	prime = get_closest_prime_1(&data, handle);
	printf( "The closest prime to %d is: %d\n", data.num, *prime);

	clnt_destroy(handle);

	return 0;
}
