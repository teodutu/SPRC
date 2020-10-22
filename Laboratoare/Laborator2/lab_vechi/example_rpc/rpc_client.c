/**
	Sisteme de programe pentru retele de calculatoare

	Copyright (C) 2008 Ciprian Dobre & Florin Pop
	Univerity Politehnica of Bucharest, Romania

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 */

#include <stdio.h>
#include <time.h>
#include <rpc/rpc.h>

#include "load.h"
#define RMACHINE "localhost"

int main(int argc, char *argv[]){

	/* variabila clientului */
	CLIENT *handle;

	double *res;

	handle=clnt_create(
		RMACHINE,		/* numele masinii unde se afla server-ul */
		LOAD_PROG,		/* numele programului disponibil pe server */
		LOAD_VERS,		/* versiunea programului */
		"tcp");			/* tipul conexiunii client-server */

	if(handle == NULL) {
		perror("");
		return -1;
	}
	res = get_load_1(NULL, handle);
	printf( "The server load is:  %lf\n", *res);

	return 0;
}
