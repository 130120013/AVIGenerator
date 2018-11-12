#include <iostream>
#include "AVIGenerator.h"
#include <cmath>
#include <cstdint>

int main()
{
	generateAVI("124.avi", [](double x, double y, double t) -> auto
	{
		//return x * x + y * y + t * t * 44;
		return std::cos((x - 960) * 3.14 / 540) * std::cos((y - 540) * 3.14 / 540) + std::cos((t - 125) * 3.14 / 125);
		//return t / 124.;
	}, 1920, 1080, 250, -2, 2, true);
	//void generateAVI(const char* file_name, Caller&& get_value, unsigned width, unsigned height, unsigned frames, double val_min, double val_max, bool discard_file)
	return 0;
}