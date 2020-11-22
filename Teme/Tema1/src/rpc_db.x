/**
 * Toate raspunsurile serverului folosesc acest enum pentru a indica daca au
 * fost efectuate cu succes sau nu. In acest ultim caz, eventualele date primite
 * pe langa acest raspuns nu sunt valide.
 */
enum status_t {
	OK = 0,
	ERROR
};

enum method_t {
	ADD = 0,
	UPDATE
};

/**
 * Distinctia intre PRC-urile add() si update() se face prin intermediul
 * campului `method_t`.
 */
struct add_update_request_t {
	unsigned long sess_key;
	int id;
	method_t method;
	float values<>;
};

struct get_response_t  {
	status_t status;
	float min;
	float max;
	float avg;
	float med;
};

/**
 * Tipul parametrilor unor requesturi care vizeaza intrari individuale in baza
 * de date: `read`, `del`, `get_stat`
 */
struct entry_request_t {
	unsigned long sess_key;
	int id;
};

struct read_response_t  {
	status_t status;
	float data<>;
};

struct load_response_t {
	status_t status;
	int ids<>;
};


program RPC_DB_PROGRAM {
	version RPC_DB_VERS {
		unsigned long login(string) = 1;
		status_t logout(unsigned long) = 2;
		status_t add_update(add_update_request_t) = 3;
		read_response_t read_entry(entry_request_t) = 4;
		status_t del(entry_request_t) = 5;
		load_response_t load(unsigned long) = 6;
		status_t store(unsigned long) = 7;
		get_response_t get_stats(entry_request_t) = 8;
		get_response_t get_stats_all(unsigned long) = 9;
	} = 1;
} = 1;
