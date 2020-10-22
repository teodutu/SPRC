RPC = rpcgen
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
LDFLAGS = -lnsl
PROGRAM = check
SRC_CLIENT = $(PROGRAM)_clnt.c $(PROGRAM)_xdr.c

.PHONY: build clean

build: client

client: $(SRC_CLIENT) $(PROGRAM)_rpc_client.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	rm -f $(PROGRAM)_svc.c

$(SRC_SERVER) $(SRC_CLIENT): $(PROGRAM).x
	$(RPC) -C $^

clean:
	rm -f client $(PROGRAM)_svc.c $(PROGRAM).h $(SRC_CLIENT)
