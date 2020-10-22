struct client_data {
	int num;
	char client_name[32];
};

program CLOSEST_PRIME_PROG {
	version CLOSEST_PRIME_VERS {
		int get_closest_prime(struct client_data) = 1;
	} = 1;
} = 1;
