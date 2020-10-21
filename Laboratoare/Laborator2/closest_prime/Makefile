RPC = rpcgen
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O3
LDFLAGS = -lnsl
SRC_SERVER = closest_prime_svc.c closest_prime_xdr.c
SRC_CLIENT = closest_prime_clnt.c closest_prime_xdr.c

.PHONY: build clean

build: server client

server: $(SRC_SERVER) closest_prime_rpc_server.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client: $(SRC_CLIENT) closest_prime_rpc_client.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SRC_SERVER) $(SRC_CLIENT): closest_prime.x
	$(RPC) -C $^

clean:
	rm -f client server closest_prime.h $(SRC_SERVER) $(SRC_CLIENT)
