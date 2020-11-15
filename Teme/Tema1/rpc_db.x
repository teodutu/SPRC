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

struct get_response_t  {
	response_t status;
	float min;
	float max;
	float avg;
	float med;
};

struct entry_request_t {
	unsigned long sess_key;
	int id;
};

struct read_response_t  {
	response_t status;
	float data<>;
};

struct load_response_t {
	response_t status;
	int ids<>;
};


program RPC_DB_PROGRAM {
	version RPC_DB_VERS {
		unsigned long login(string) = 1;
		response_t logout(unsigned long) = 2;
		response_t add_update(values_request_t) = 3;
		read_response_t read_entry(entry_request_t) = 4;
		response_t del(entry_request_t) = 5;
		load_response_t load(unsigned long) = 6;
		response_t store(unsigned long) = 7;
		get_response_t get_stats(entry_request_t) = 8;
	} = 1;
} = 1;
