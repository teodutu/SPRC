enum response_t {
	OK = 0,
	ERROR
};

struct values_request_t {
	unsigned long sess_key;
	float values<>;
};

enum metric_t {
	MIN = 0,
	MAX,
	AVG,
	MED
};

struct get_request_t  {
	unsigned long sess_key;
	metric_t metric;
};

program RPC_DB_PROGRAM {
	version RPC_DB_VERS {
		unsigned long login(string) = 1;
		response_t logout(unsigned long) = 2;
	} = 1;
} = 1;
