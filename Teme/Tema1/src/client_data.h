#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>

#include "data_entry.h"


#ifdef __cplusplus
extern "C" {
#endif

class ClientData {
private:
	void update_median();

public:
	size_t num_elem;
	float min, max, avg, med;
	std::string user;
	std::unordered_map<int, DataEntry> data;

	ClientData();
	ClientData(const std::string &user);

	void update_stats(int id);
	void read_stats(std::ifstream &in);
	void write_stats(std::ofstream &out);
};

#ifdef __cplusplus
}
#endif

#endif  // CLIENT_DATA_H