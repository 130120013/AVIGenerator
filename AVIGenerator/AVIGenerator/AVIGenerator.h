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

	inline std::uint32_t& chunk_id() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data());
	}
	inline std::uint32_t& chunk_size() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 4);
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
		return STRUCT_SIZE + chunk_size();
	}
	static constexpr std::size_t STRUCT_SIZE = 8;
private:
	std::uint8_t* m_ptr = nullptr;
};

struct List: Chunk
{
	List() = default;
	List(std::uint8_t* pBuf, std::uint32_t offset = 0) :Chunk(pBuf, offset){}

	inline std::uint32_t& list_type() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data());
	}
	inline std::uint8_t* list_data() const
	{
		return this->chunk_data() + 4;
	}
	//static constexpr std::uint32_t LIST_ID = make_fcc("LIST");
	static constexpr std::size_t STRUCT_SIZE = 12;
private:
	std::uint8_t* m_ptr = nullptr;
};

struct RIFFHeader : Chunk
{
	RIFFHeader() = default;
	RIFFHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :Chunk(pBuf, offset) {}

	inline std::uint32_t& file_type() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data());
	}
	inline std::uint8_t* file_data() const
	{
		return this->chunk_data() + 4;
	}
	//static constexpr std::uint32_t RIFF_ID = make_fcc("RIFF");
	static constexpr std::size_t STRUCT_SIZE = 12;
private:
	std::uint8_t* m_ptr = nullptr;
};


struct MainAVIHeader:Chunk
{
	MainAVIHeader() = default;
	MainAVIHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :Chunk(pBuf, offset){}

	inline std::uint32_t& dwMicroSecPerFrame() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data());
	}
	inline std::uint32_t& dwMaxBytesPerSec() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 4);
	}
	inline std::uint32_t& dwPaddingGranularity() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 8);
	}
	inline std::uint32_t& dwFlags() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 12);
	}
	inline std::uint32_t& dwTotalFrames() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 16);
	}
	inline std::uint32_t& dwInitialFrames() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 20);
	}
	inline std::uint32_t& dwStreams() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 24);
	}
	inline std::uint32_t& dwSuggestedBufferSize() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 28);
	}
	inline std::uint32_t& dwWidth() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 32);
	}
	inline std::uint32_t& dwHeight() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 32);
	}
	inline std::uint32_t& dwReserved() const //dwReserved[4];
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 36);
	}
	//static constexpr std::uint32_t FCC = 0x61766968;
	static constexpr std::size_t STRUCT_SIZE = 56; //size without Chunk's fields
private:
	std::uint8_t* m_ptr = nullptr;
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
		return *reinterpret_cast<std::int32_t*>(this->data() + sizeof(std::int32_t));
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
	static constexpr std::size_t STRUCT_SIZE = sizeof(std::uint32_t) * 4;
private:
	std::uint8_t* m_ptr = nullptr;
};

struct AVIStreamHeader
{
	AVIStreamHeader() = default;
	AVIStreamHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset)
	{
		//this->fccType() = FCCType;
	}

	inline std::uint32_t& fccType() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data());
	}
	inline std::uint32_t& fccHandler() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 4);
	}
	inline std::uint32_t& dwFlags() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 8);
	}
	inline std::uint16_t& wPriority() const
	{
		return *reinterpret_cast<std::uint16_t*>(this->data() + 12);
	}
	inline std::uint16_t& wLanguage() const
	{
		return *reinterpret_cast<std::uint16_t*>(this->data() + 14);
	}
	inline std::uint32_t& dwInitialFrames() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 16);
	}
	inline std::uint32_t& dwScale() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 20);
	}
	inline std::uint32_t& dwRate() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 24);
	}
	inline std::uint32_t& dwStart() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 28);
	}
	inline std::uint32_t& dwLength() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 32);
	}
	inline std::uint32_t& dwSuggestedBufferSize() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 36);
	}
	inline std::uint32_t& dwQuality() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 40);
	}
	inline std::uint32_t& dwSampleSize() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 44);
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
	//static constexpr std::uint32_t FCC = 0x76696473;
	static constexpr std::size_t STRUCT_SIZE = 48 + 4 * sizeof(long);
private:
	std::uint8_t* m_ptr = nullptr;
};

struct avi_file_handle_close
{
	inline void operator()(std::FILE* fp) const noexcept
	{
		std::fclose(fp);
	}
};

using unique_avi_file_handle = std::unique_ptr<std::FILE, avi_file_handle_close>;

std::unique_ptr<std::uint8_t[]> generateAVIStructures(unsigned width, unsigned height, unsigned frames);

template <class Caller>
bool generateFrames(unsigned width, unsigned height, Caller&& get_value, unsigned frames,
	double val_min, double val_max, unsigned char* frData)
{
	auto cbPadding = std::uint32_t(width & 3);
	for (unsigned f = 0; f < frames; ++f)
	{
		for (unsigned l = 0; l < height; ++l)
		{
			for (unsigned k = 0; k < width; ++k)
			{
				bool successCode = ValToRGB(get_value(k, l, f), val_min, val_max, (RGBTRIPLE*)(frData + 24 * k * l * (f + 1)));
				if (!successCode)
					return false;
			}
			memcpy(frData + 24 * width * (l + 1) * (f + 1), 0, cbPadding); //sizeof(rgbBlue) + sizeof(rgbGreen) + sizeof(rgbRed)
		}
	}
	return true;
}

template <class Caller>
void generateAVI(const char* file_name, Caller&& get_value, unsigned width, unsigned height, unsigned frames, double val_min, double val_max, bool discard_file)
{
	auto aviFile = unique_avi_file_handle(std::fopen(file_name, "wb"));
	if (bool(aviFile) && !discard_file)
		return;

	std::size_t strlBufferSize = List::STRUCT_SIZE + 2 * Chunk::STRUCT_SIZE + AVIStreamHeader::STRUCT_SIZE + BitmapInfoHeaderPtr::STRUCT_SIZE;
	std::size_t hdrlBufferSize = List::STRUCT_SIZE + MainAVIHeader::STRUCT_SIZE + strlBufferSize;
	std::size_t moviBufferSize = List::STRUCT_SIZE + Chunk::STRUCT_SIZE + (frames * 24 * width + std::uint32_t(width & 3)) * height;
	std::size_t riffBufferSize = RIFFHeader::STRUCT_SIZE + Chunk::STRUCT_SIZE + hdrlBufferSize + moviBufferSize;

	std::unique_ptr<std::uint8_t[]> mem(generateAVIStructures(width, height, frames));

	RIFFHeader Riff(mem.get());
	generateFrames(width, height, get_value, frames, -10, 10, Riff.chunk_data() + hdrlBufferSize + 5 * sizeof(std::uint32_t));

	if (fwrite(Riff.data(), 1, riffBufferSize, aviFile.get()) < riffBufferSize)
		return;
}

#endif // !AVI_GENERATOR
 