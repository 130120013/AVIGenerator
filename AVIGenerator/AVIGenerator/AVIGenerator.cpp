#include "AVIGenerator.h"

void createAVI()
{
	List RIFFList;
	RIFFList.dwFourCC = 0x41564920; //'AVI '
	RIFFList.dwList = 0x52494646; //'RIFF'

	List hdrl;
	hdrl.dwFourCC = 0x6864726c; //'hdrl'
	hdrl.dwList = 0x52494646; //'RIFF'

	MainAVIHeader mainHeader;
	mainHeader.fcc = 0x61766968; // 'avih'
}