#include <rpc/rpc.h>

#include "sum.h"

int *get_sum_1_svc(struct sum_data *data, struct svc_req *cl) {
	static int sum = 0;

	if (!data) {
		fprintf(stderr, "Received NULL pointer operand.\n");
		return NULL;
	}

	sum = data->x + data->y;
	return &sum;
}
