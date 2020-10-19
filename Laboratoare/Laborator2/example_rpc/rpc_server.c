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

double * get_load_1_svc(void *p, struct svc_req *cl){
	static double load;

	FILE *fp;
	fp = popen("uptime | cut -d ',' -f4 | sed 's/^ //'","r");

	fscanf(fp, "%lf", &load);

	fclose(fp);
	return &load;
}
