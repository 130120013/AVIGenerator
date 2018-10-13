#include <cstdint>
#include <memory>
#include <cstdio>
#include "ImageGenerator.h"

#ifndef AVI_GENERATOR
#define AVI_GENERATOR

struct Chunk
{
	Chunk() = default;
	Chunk(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset) {}

	inline std::int32_t& chunk_id() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data());
	}
	inline std::int32_t& chunk_size() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 4);
	}
	inline std::uint8_t* chunk_data() const
	{
		return this->data() + 8;
	}
	inline std::uint8_t* data() const
	{
		return m_ptr;
	}
	inline std::size_t size() const
	{
		return STRUCT_SIZE + this->chunk_size();
	}

private:
	std::uint8_t* m_ptr = nullptr;
	static constexpr std::size_t STRUCT_SIZE = 8;
};

struct List: Chunk
{
	List() = default;
	List(std::uint8_t* pBuf, std::uint32_t offset = 0) :Chunk(pBuf, offset)
	{
	}

	inline std::int32_t& list_type() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data());
	}
	inline std::uint8_t& list_data() const
	{
		return *reinterpret_cast<std::uint8_t*>(this->chunk_data() + 4);
	}
	static constexpr std::uint32_t LIST_ID = 0x4C495354;
private:
	std::uint8_t* m_ptr = nullptr;
	static constexpr std::size_t STRUCT_SIZE = 12;
};

struct MainAVIHeader:Chunk
{
	MainAVIHeader() = default;
	MainAVIHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :Chunk(pBuf, offset)
	{
		//this->chunk_id() = FCC;
	}

	inline std::int32_t& dwMicroSecPerFrame() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data());
	}
	inline std::int32_t& dwMaxBytesPerSec() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 4);
	}
	inline std::int32_t& dwPaddingGranularity() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 8);
	}
	inline std::int32_t& dwFlags() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 12);
	}
	inline std::int32_t& dwTotalFrames() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 16);
	}
	inline std::int32_t& dwInitialFrames() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 20);
	}
	inline std::int32_t& dwStreams() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 24);
	}
	inline std::int32_t& dwSuggestedBufferSize() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 28);
	}
	inline std::int32_t& dwWidth() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 32);
	}
	inline std::int32_t& dwReserved() const
	{
		return *reinterpret_cast<std::int32_t*>(this->chunk_data() + 36);
	}
private:
	std::uint8_t* m_ptr = nullptr;
	static constexpr std::uint32_t FCC = 0x61766968;
	static constexpr std::size_t STRUCT_SIZE = 48;
};

/////////////

struct RECT
{
	RECT() = default;
	RECT(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset) {}

	inline std::int32_t& left() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data());
	}
	inline std::int32_t& top() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + sizeof(std::int32_t)));
	}
	inline std::int32_t& right() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 2 * sizeof(std::int32_t));
	}
	inline std::int32_t& bottom() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 3 * sizeof(std::int32_t));
	}
	inline std::uint8_t* data() const
	{
		return m_ptr;
	}
	inline std::size_t size() const
	{
		return STRUCT_SIZE;
	}

private:
	std::uint8_t* m_ptr = nullptr;
	static constexpr std::size_t STRUCT_SIZE = sizeof(std::int32_t) * 4;
};

struct AVIStreamHeader
{
	AVIStreamHeader() = default;
	AVIStreamHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset)
	{
		//this->fccType() = FCCType;
	}

	inline std::int32_t& fccType() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data());
	}
	inline std::int32_t& fccHandler() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 4);
	}
	inline std::int32_t& dwFlags() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 8);
	}
	inline std::int16_t& wPriority() const
	{
		return *reinterpret_cast<std::int16_t*>(this->data() + 12);
	}
	inline std::int16_t& wLanguage() const
	{
		return *reinterpret_cast<std::int16_t*>(this->data() + 14);
	}
	inline std::int32_t& dwInitialFrames() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 16);
	}
	inline std::int32_t& dwScale() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 20);
	}
	inline std::int32_t& dwRate() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 24);
	}
	inline std::int32_t& dwStart() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 28);
	}
	inline std::int32_t& dwLength() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 32);
	}
	inline std::int32_t& dwSuggestedBufferSize() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 36);
	}
	inline std::int32_t& dwQuality() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 40);
	}
	inline std::int32_t& dwSampleSize() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 44);
	}
	inline RECT rect() const
	{
		return RECT(this->data(), 48);
	}
	inline std::uint8_t* data() const
	{
		return m_ptr;
	}
	inline std::size_t size() const
	{
		return STRUCT_SIZE;
	}

private:
	std::uint8_t* m_ptr = nullptr;
	static constexpr std::size_t STRUCT_SIZE = 48 + 4 * sizeof(long);
	static constexpr std::uint32_t FCCType = 0x76696473;
};

#endif // !AVI_GENERATOR
