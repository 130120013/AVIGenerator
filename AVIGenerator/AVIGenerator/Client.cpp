#include <iostream>
#include "AVIGenerator.h"
#include <cstdint>

int main()
{
	generateAVI("123.avi", [](std::int32_t x, std::int32_t y, std::int32_t t) -> std::int32_t
	{
		return x * y + t;
	}, 1000, 1000, 26, 0, 1000025, true);
	//void generateAVI(const char* file_name, Caller&& get_value, unsigned width, unsigned height, unsigned frames, double val_min, double val_max, bool discard_file)
	return 0;
}