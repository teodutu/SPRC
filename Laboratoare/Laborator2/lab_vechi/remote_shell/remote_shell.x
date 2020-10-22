struct shell_data {
	unsigned short response_len;
	char data[8192];
};

program REMOTE_SHELL_PROG {
	version REMOTE_SHELL_VERS {
		struct shell_data run_cmd(struct shell_data) = 1;
	} = 1;
} = 1;
