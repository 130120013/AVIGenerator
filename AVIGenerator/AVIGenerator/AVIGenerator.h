#include <cstdint>
#include <memory>
#include <vector>
#include <cstdio>

#ifndef AVI_GENERATOR
#define AVI_GENERATOR

struct Chunk 
{
	std::uint32_t ckID;
	std::uint32_t ckSize;
	std::unique_ptr<unsigned char[]> ckData; // contains headers or video/audio data dwSize
};

struct List
{
	const std::uint32_t dwList = 0x4C495354;
	std::uint32_t listSize;
	std::uint32_t listType;
	std::unique_ptr<unsigned char[]> listData; // contains Lists and Chunks
};

struct MainAVIHeader
{
	std::uint32_t fcc = 0x61766968; // avih
	std::uint32_t cb; //= sizeof(MainAVIHeader) - 8; 
	std::uint32_t dwMicroSecPerFrame;
	std::uint32_t dwMaxBytesPerSec;
	std::uint32_t dwPaddingGranularity;
	std::uint32_t dwFlags;
	std::uint32_t dwTotalFrames;
	std::uint32_t dwInitialFrames;
	std::uint32_t dwStreams;
	std::uint32_t dwSuggestedBufferSize;
	std::uint32_t dwWidth;
	std::uint32_t dwHeight;
	std::uint32_t dwReserved[4];
};

struct RECT 
{
	long left;
	long top;
	long right;
	long bottom;
};

struct AVIStreamHeader 
{
	std::uint32_t fccType = 0x76696473; //vids - contains video data
	std::uint32_t fccHandler; //http://www.fourcc.org/codecs.php
	std::uint32_t dwFlags;
	std::uint16_t wPriority;
	std::uint16_t wLanguage;
	std::uint32_t dwInitialFrames;
	std::uint32_t dwScale;
	std::uint32_t dwRate;
	std::uint32_t dwStart;
	std::uint32_t dwLength;
	std::uint32_t dwSuggestedBufferSize;
	std::uint32_t dwQuality;
	std::uint32_t dwSampleSize;
	RECT rcFrame;
};

struct BITMAPINFOHEADER 
{
	std::uint32_t biSize;
	long  biWidth;
	long  biHeight;
	std::uint16_t  biPlanes;
	std::uint16_t  biBitCount;
	std::uint32_t biCompression;
	std::uint32_t biSizeImage;
	long  biXPelsPerMeter;
	long  biYPelsPerMeter;
	std::uint32_t biClrUsed;
	std::uint32_t biClrImportant;
};

//STRF - BITMAPINFO structure

struct AVIIndexEntry
{
	std::uint32_t ckid;
	std::uint32_t dwFlags;
	std::uint32_t dwChunkOffset;
	std::uint32_t dwChunkLength;
};
//00db

/*
DIB  Bits  '##db'
	 BYTE  abBits[];
*/


void createAVI();

#endif // !AVI_GENERATOR
