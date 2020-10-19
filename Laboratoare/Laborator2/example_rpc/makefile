#/**
#	Sisteme de programe pentru retele de calculatoare
#	
#	Copyright (C) 2008 Ciprian Dobre & Florin Pop
#	Univerity Politehnica of Bucharest, Romania
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# */

build:
	rpcgen -C load.x
	gcc -o server rpc_server.c load_svc.c -lnsl -Wall
	gcc -o client rpc_client.c load_clnt.c -lnsl -Wall

clean:
	rm -f client server load.h load_svc.c load_clnt.c
