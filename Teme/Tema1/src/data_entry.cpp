#include "data_entry.h"


DataEntry::DataEntry():
	len(0), min(INFINITY), max(-INFINITY), avg(NAN), med(NAN), values(nullptr)
{ }

DataEntry::DataEntry(u_int l, float *v):
	len(l), min(INFINITY), max(-INFINITY), avg(0.f)
{
	values = new float[l];
	for (u_int i = 0; i != l; ++i) {
		values[i] = v[i];

		min = min > v[i] ? v[i] : min;
		max = max < v[i] ? v[i] : max;
		avg += v[i];
	}

	avg /= l;

	std::nth_element(v, v + l / 2, v + l);
	if (l & 1)
		med = v[l / 2];
	else
		med = (v[l / 2] + *std::max_element(v, v + l / 2)) / 2;
}

DataEntry::DataEntry(DataEntry &&other)
{
	len = other.len;
	min = other.min;
	max = other.max;
	avg = other.avg;
	med = other.med;

	values = other.values;
	other.values = nullptr;
}

DataEntry& DataEntry::operator=(DataEntry &&other)
{
	if (&other != this) {
		len = other.len;
		min = other.min;
		max = other.max;
		avg = other.avg;
		med = other.med;

		values = other.values;
		other.values = nullptr;
	}

	return *this;
}

DataEntry::~DataEntry()
{
	delete[] values;
}

void DataEntry::read_stats(std::ifstream &in)
{
	in.read((char *)&len, sizeof(len));
	in.read((char *)&min, sizeof(min));
	in.read((char *)&max, sizeof(max));
	in.read((char *)&avg, sizeof(avg));
	in.read((char *)&med, sizeof(med));
}

void DataEntry::write_stats(std::ofstream &out)
{
	out.write((char *)&len, sizeof(len));
	out.write((char *)&min, sizeof(min));
	out.write((char *)&max, sizeof(max));
	out.write((char *)&avg, sizeof(avg));
	out.write((char *)&med, sizeof(med));
}
