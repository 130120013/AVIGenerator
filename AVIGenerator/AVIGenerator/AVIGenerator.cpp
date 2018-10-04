#include "AVIGenerator.h"

std::uint32_t dwLittleEndianToBigEndian(std::uint32_t num) //need overload for long
{
	return ((num >> 24) & 0xff) | // move byte 3 to byte 0
		((num << 8) & 0xff0000) | // move byte 1 to byte 2
		((num >> 8) & 0xff00) | // move byte 2 to byte 1
		((num << 24) & 0xff000000); // byte 0 to byte 3
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