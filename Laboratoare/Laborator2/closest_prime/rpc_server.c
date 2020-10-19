#include <stdio.h>
#include <time.h>
#include <rpc/rpc.h>
#include <syslog.h>

#include "closest_prime.h"

#define MAX_SQRT 46340  // sqrt(INT_MAX)
#define LOG_FILE_NAME "server"

static unsigned char *prime;

static void generate_primes(size_t len) {
	long long res;
	prime[0] = 2;  // 0b10

	for (int i = 1; i <= MAX_SQRT; ++i) {
		for (int j = 1; j <= MAX_SQRT; ++j) {
			res = 4LL * i * i + 1LL * j * j;
			if (res & 1 && res <= len && (res % 12 == 1 || res % 12 == 5)
			) {
				prime[res >> 4] ^= (1 << ((res & 0xf) >> 1));
			}

			res = 3LL * i * i + 1LL * j * j;
			if (res & 1 && res <= len && res % 12 == 7) {
				prime[res >> 4] ^= (1 << ((res & 0xf) >> 1));
			}

			res = 3LL * i * i - 1LL * j * j;
			if (i > j && res & 1 && res <= len && res % 12 == 11) {
				prime[res >> 4] ^= (1 << ((res & 0xf) >> 1));
			}
		}
	}

	for (int i = 5; i <= MAX_SQRT; i += 2) {
		if (prime[i >> 4] & (1 << ((i & 0xf) / 2))) {
			long long k = i * i;

			for (long long j = k; j <= len; j += k << 1) {
				prime[j >> 4] &= ~(1 << ((j & 0xf) >> 1));
			}
		}
	}
}

#define IS_PRIME(n) \
	(n == 2 || ((n & 1) && prime[n >> 4] & (1 << ((n & 0xf) >> 1))))

int *get_closest_prime_1_svc(struct client_data *data, struct svc_req *cl) {
	int low = data->num, high = data->num;
	static int ret = -1;

	openlog(LOG_FILE_NAME, 0, LOG_USER);

	if (!prime) {
		syslog(LOG_INFO, "[SERVER][INFO] Genreating primes array.");

		prime = calloc((INT_MAX >> 4) + 1, sizeof(*prime));
		if (!prime) {
			syslog(LOG_ERR, "[SERVER][FAULT] Failed to allocate primes array.");
			return ret;
		}

		generate_primes((INT_MAX >> 4) + 1);
		syslog(LOG_INFO, "[SERVER][INFO] Created primes array.");
	}

	for (;; --low, ++high) {
		if (low >= 0 && IS_PRIME(low)) {
			ret = low;
			break;
		}

		if (high <= INT_MAX && IS_PRIME(high)) {
			ret = high;
			break;
		}
	}

	syslog(LOG_ERR,
		"[SERVER][INFO] Returning prime number %d for client %s on input %d.\n",
		ret, data->client_name, data->num);
	closelog();

	return &ret;
}
