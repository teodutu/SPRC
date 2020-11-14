enum response_t {
	OK = 0,
	ERROR
};

enum method_t {
	ADD = 0,
	UPDATE
};

struct values_request_t {
	unsigned long sess_key;
	int id;
	method_t method;
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

struct read_request_t {
	unsigned long sess_key;
	int id;
};

struct read_response_t  {
	response_t status;
	float data<>;
};


program RPC_DB_PROGRAM {
	version RPC_DB_VERS {
		unsigned long login(string) = 1;
		response_t logout(unsigned long) = 2;
		response_t add_update(values_request_t) = 3;
		read_response_t read(read_request_t) = 4;
	} = 1;
} = 1;
