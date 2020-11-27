#ifndef DATA_ENTRY_H_
#define DATA_ENTRY_H_

#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cmath>


#ifdef __cplusplus
extern "C" {
#endif

class DataEntry {
public:
	size_t len;
	float min, max, avg, med;
	float *values;

	DataEntry();

	DataEntry(u_int l, float *v);
	DataEntry(DataEntry &&other);
	DataEntry& operator=(DataEntry &&other);

	~DataEntry();

	void read_stats(std::ifstream &in);
	void write_stats(std::ofstream &out);
};

#ifdef __cplusplus
}
#endif

#endif  // DATA_ENTRY_H_
