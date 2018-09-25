#include <cstdint>
#include <memory>

#ifndef AVI_GENERATOR
#define AVI_GENERATOR

struct Chunk 
{
	std::uint32_t dwFourCC;
	std::uint32_t dwSize;
	std::unique_ptr<unsigned char> data; // contains headers or video/audio data dwSize
};

typedef struct List
{
	std::uint32_t dwList;
	std::uint32_t dwSize;
	std::uint32_t dwFourCC;
	std::unique_ptr<unsigned char> data; // contains Lists and Chunks
};
struct MainAVIHeader
{
	std::uint32_t fcc;
	std::uint32_t cb;
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
	std::uint32_t fccType;
	std::uint32_t fccHandler;
	std::uint32_t  dwFlags;
	std::uint16_t   wPriority;
	std::uint16_t   wLanguage;
	std::uint32_t  dwInitialFrames;
	std::uint32_t  dwScale;
	std::uint32_t  dwRate;
	std::uint32_t  dwStart;
	std::uint32_t  dwLength;
	std::uint32_t  dwSuggestedBufferSize;
	std::uint32_t  dwQuality;
	std::uint32_t  dwSampleSize;
	RECT   rcFrame;
};

//STRF - BITMAPINFO structure

struct AVIIndexEntry{
	std::uint32_t ckid;
	std::uint32_t dwFlags;
	std::uint32_t dwChunkOffset;
	std::uint32_t dwChunkLength;
};

void createAVI();

#endif // !AVI_GENERATOR
