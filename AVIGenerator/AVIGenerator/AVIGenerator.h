#include <cstdint>
#include <memory>
#include <cstdio>
#include "ImageGenerator.h"

#ifndef AVI_GENERATOR
#define AVI_GENERATOR

struct Chunk 
{
	Chunk() = default;
	Chunk(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf), m_offset(offset) {}

	inline std::int32_t& ckID() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + m_offset);
	}
	inline std::int32_t& ckSize() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 4));
	}
	inline uint8_t& ckData() const
	{
		return *reinterpret_cast<std::uint8_t*>(m_ptr + (m_offset + 8));
	}
private:
	std::uint8_t* m_ptr = nullptr;
	std::uint32_t m_offset = 0;

	//std::uint32_t ckID;
	//std::uint32_t ckSize;
	//std::unique_ptr<unsigned char[]> ckData; // contains headers or video/audio data dwSize
};

struct List
{
	List() = default;
	List(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf), m_offset(offset) {}

	inline std::int32_t& dwList() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + m_offset);
	}
	inline std::int32_t& listSize() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 4));
	}
	inline std::int32_t& listType() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 8));
	}
	inline uint8_t& listData() const
	{
		return *reinterpret_cast<std::uint8_t*>(m_ptr + (m_offset + 12));
	}
	static constexpr std::uint32_t dwListID = 0x4C495354;

private:
	std::uint8_t* m_ptr = nullptr;
	std::uint32_t m_offset = 0;

	//const std::uint32_t dwList = 0x4C495354;
	//std::uint32_t listSize;
	//std::uint32_t listType;
	//std::unique_ptr<unsigned char[]> listData; // contains Lists and Chunks
};

struct MainAVIHeader
{
	MainAVIHeader() = default;
	MainAVIHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf), m_offset(offset) {}

	inline std::int32_t& fcc() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + m_offset);
	}
	inline std::int32_t& cb() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 4));
	}
	inline std::int32_t& dwMicroSecPerFrame() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 8));
	}
	inline int32_t& dwMaxBytesPerSec() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 12));
	}
	inline std::int32_t& dwPaddingGranularity() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 16));
	}
	inline int32_t& dwFlags() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 20));
	}
	inline std::int32_t& dwTotalFrames() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 24));
	}
	inline int32_t& dwInitialFrames() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 28));
	}
	inline std::int32_t& dwStreams() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 32));
	}
	inline int32_t& dwSuggestedBufferSize() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 36));
	}
	inline std::int32_t& dwWidth() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 40));
	}
	inline int32_t& dwReserved() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 44));
	}
	static constexpr std::uint32_t FCC = 0x61766968;

private:
	std::uint8_t* m_ptr = nullptr;
	std::uint32_t m_offset = 0;

	//std::uint32_t fcc = 0x61766968; // avih
	//std::uint32_t cb; //= sizeof(MainAVIHeader) - 8; 
	//std::uint32_t dwMicroSecPerFrame;
	//std::uint32_t dwMaxBytesPerSec;
	//std::uint32_t dwPaddingGranularity;
	//std::uint32_t dwFlags;
	//std::uint32_t dwTotalFrames;
	//std::uint32_t dwInitialFrames;
	//std::uint32_t dwStreams;
	//std::uint32_t dwSuggestedBufferSize;
	//std::uint32_t dwWidth;
	//std::uint32_t dwHeight;
	//std::uint32_t dwReserved[4];
};

struct RECT 
{
	RECT() = default;
	RECT(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf), m_offset(offset) {}

	inline long& left() const
	{
		return *reinterpret_cast<long*>(m_ptr + m_offset);
	}
	inline long& top() const
	{
		return *reinterpret_cast<long*>(m_ptr + (m_offset + sizeof(long)));
	}
	inline long& right() const
	{
		return *reinterpret_cast<long*>(m_ptr + (m_offset + 2 * sizeof(long)));
	}
	inline long& bottom() const
	{
		return *reinterpret_cast<long*>(m_ptr + (m_offset + 3 * sizeof(long)));
	}
private:
	std::uint8_t* m_ptr = nullptr;
	std::uint32_t m_offset = 0;

	//long left;
	//long top;
	//long right;
	//long bottom;
};

struct AVIStreamHeader 
{
	AVIStreamHeader() = default;
	AVIStreamHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf), m_offset(offset) {}

	inline std::int32_t& fccType() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + m_offset);
	}
	inline std::int32_t& fccHandler() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 4));
	}
	inline std::int32_t& dwFlags() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 8));
	}
	inline int16_t& wPriority() const
	{
		return *reinterpret_cast<std::int16_t*>(m_ptr + (m_offset + 12));
	}
	inline std::int16_t& wLanguage() const
	{
		return *reinterpret_cast<std::int16_t*>(m_ptr + (m_offset + 14));
	}
	inline int32_t& dwInitialFrames() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 16));
	}
	inline std::int32_t& dwScale() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 20));
	}
	inline int32_t& dwRate() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 24));
	}
	inline std::int32_t& dwStart() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 28));
	}
	inline int32_t& dwLength() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 32));
	}
	inline std::int32_t& dwSuggestedBufferSize() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 36));
	}
	inline int32_t& dwQuality() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 40));
	}
	inline int32_t& dwSampleSize() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 44));
	}
	inline int32_t& rcFrame() const
	{
		return *reinterpret_cast<std::int32_t*>(m_ptr + (m_offset + 48));
	}
	static constexpr std::uint32_t FCCType = 0x76696473;

private:
	std::uint8_t* m_ptr = nullptr;
	std::uint32_t m_offset = 0;

	//std::uint32_t fccType = 0x76696473; //vids - contains video data
	//std::uint32_t fccHandler; //http://www.fourcc.org/codecs.php
	//std::uint32_t dwFlags;
	//std::uint16_t wPriority;
	//std::uint16_t wLanguage;
	//std::uint32_t dwInitialFrames;
	//std::uint32_t dwScale;
	//std::uint32_t dwRate;
	//std::uint32_t dwStart;
	//std::uint32_t dwLength;
	//std::uint32_t dwSuggestedBufferSize;
	//std::uint32_t dwQuality;
	//std::uint32_t dwSampleSize;
	//RECT rcFrame;
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
