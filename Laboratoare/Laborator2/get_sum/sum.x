struct sum_data {
	int x;
	int y;
};

program SUM_PROG {
	version SUM_VERS {
		int get_sum(struct sum_data) = 1;
	} = 1;
} = 1;
