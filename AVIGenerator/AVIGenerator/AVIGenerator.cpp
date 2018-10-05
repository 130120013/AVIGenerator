#include "AVIGenerator.h"

template <class T>
T LittleEndianToBigEndian(T num)
{
	if (sizeof(T) == 32) //for std::uint32_t and long x32
	{
		return ((num >> 24) & 0xFF) |
			((num << 8) & 0xFF0000) |
			((num >> 8) & 0xFF00) |
			((num << 24) & 0xFF000000);
	}
	num = ((num << 8) & 0xFF00FF00FF00FF00ULL) | ((num >> 8) & 0x00FF00FF00FF00FFULL); //for long x64
	num = ((num << 16) & 0xFFFF0000FFFF0000ULL) | ((num >> 16) & 0x0000FFFF0000FFFFULL);
	return (num << 32) | ((num >> 32) & 0xFFFFFFFFULL);
}

void createAVI()
{
	List RIFFList;
	RIFFList.listType = 0x52494646; //'RIFF'
	//RIFFList.listType = 0x41564920; //'AVI '
	

	List hdrl;
	hdrl.listType = 0x6864726c; //'hdrl'

	MainAVIHeader mainHeader;
	mainHeader.fcc = 0x61766968; // 'avih'
}