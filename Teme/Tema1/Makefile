RPC = rpcgen
CC = g++
CFLAGS = -std=c++14 -Wall -Wextra -Wpedantic
LDFLAGS = -lnsl
PROGRAM = rpc_db
SRC_SERVER = $(PROGRAM)_svc.c $(PROGRAM)_xdr.c
SRC_CLIENT = $(PROGRAM)_clnt.c $(PROGRAM)_xdr.c

.PHONY: build clean

build: server client

server: $(SRC_SERVER) $(PROGRAM)_server.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

client: $(SRC_CLIENT) $(PROGRAM)_client.cpp client_utils.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SRC_SERVER) $(SRC_CLIENT): $(PROGRAM).x
	$(RPC) -C $^

clean:
	rm -f client server $(PROGRAM).h $(SRC_SERVER) $(SRC_CLIENT)
