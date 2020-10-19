RPC = rpcgen
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
LDFLAGS = -lnsl

build: closest_prime.x
	rpcgen -C closest_prime.x
	$(CC) $(CFLAGS) rpc_server.c closest_prime_svc.c closest_prime_xdr.c -o server $(LDFLAGS)
	$(CC) $(CFLAGS) rpc_client.c closest_prime_clnt.c closest_prime_xdr.c -o client $(LDFLAGS)

clean:
	rm -f client server closest_prime.h closest_prime_svc.c closest_prime_clnt.c closest_prime_xdr.c
