RPC = rpcgen
CC = g++

CFLAGS = -Wall -Wextra -Wpedantic -Werror \
	-Wno-unused-variable -Wno-unused-parameter -Wno-cast-function-type
CXXFLAGS = -std=c++14 $(CFLAGS)
LDFLAGS = -lnsl

PROGRAM = rpc_db

OBJ_SERVER = $(PROGRAM)_svc.o $(PROGRAM)_xdr.o data_entry.o client_data.o \
	$(PROGRAM)_server.o
OBJ_CLIENT = $(PROGRAM)_clnt.o $(PROGRAM)_xdr.o $(PROGRAM)_client.o \
	client_utils.o

SERVER = $(PROGRAM)_server
CLIENT = $(PROGRAM)_client


.PHONY: build clean

build: $(SERVER) $(CLIENT)

$(SERVER): $(OBJ_SERVER)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CLIENT): $(OBJ_CLIENT)
	$(CC) $^ -o $@ $(LDFLAGS)

$(PROGRAM)_xdr.c $(PROGRAM)_clnt.c $(PROGRAM)_svc.c: $(PROGRAM).x
	$(RPC) -C $^

clean:
	rm -f $(SERVER) $(CLIENT) *.c *.o *.rpcdb rpc_db.h
