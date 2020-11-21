#include "client_data.h"


ClientData::ClientData():
	num_elem(0), min(INFINITY), max(-INFINITY), avg(NAN), med(NAN)
{ }

ClientData::ClientData(const std::string &user):
	num_elem(0), min(INFINITY), max(-INFINITY), avg(NAN), med(NAN), user(user)
{ }

void ClientData::update_stats(int id)
{
	auto &data_entry = data[id];
	size_t new_num_elem = num_elem + data_entry.len;

	if (std::isnan(avg))
		avg = 0.f;

	min = min > data_entry.min ? data_entry.min : min;
	max = max < data_entry.max ? data_entry.max : max;
	avg = (avg * num_elem + data_entry.avg * data_entry.len) / new_num_elem;
	num_elem = new_num_elem;

	update_median();
}

void ClientData::read_stats(std::ifstream &in)
{
	in.read((char *)&min, sizeof(min));
	in.read((char *)&max, sizeof(max));
	in.read((char *)&avg, sizeof(avg));
	in.read((char *)&med, sizeof(med));
}

void ClientData::write_stats(std::ofstream &out)
{
	out.write((char *)&min, sizeof(min));
	out.write((char *)&max, sizeof(max));
	out.write((char *)&avg, sizeof(avg));
	out.write((char *)&med, sizeof(med));
}

void ClientData::update_median()
{
	std::vector<float> all_vals;
	size_t total_count;

	for (const auto &entry : data)
		all_vals.insert(
			all_vals.end(),
			entry.second.values,
			entry.second.values + entry.second.len
		);

	total_count = all_vals.size();
	std::nth_element(
		all_vals.begin(),
		all_vals.begin() + total_count / 2,
		all_vals.end()
	);

	if (total_count & 1)
		med = all_vals[total_count / 2];
	else
		med = (
			all_vals[total_count / 2]
			+ *std::max_element(
				all_vals.begin(),
				all_vals.begin() + total_count / 2
			)
		) / 2;
}
