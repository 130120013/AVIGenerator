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
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 36);
	}
	inline std::uint32_t& dwReserved() const //dwReserved[4];
	{
		return *reinterpret_cast<std::uint32_t*>(this->chunk_data() + 40);
	}
	//static constexpr std::uint32_t FCC = 0x61766968;
	static constexpr std::size_t STRUCT_SIZE = 56; //size without Chunk's fields
};

/////////////

struct RECT
{
	RECT() = default;
	RECT(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset) {}

	inline std::int16_t& left() const
	{
		return *reinterpret_cast<std::int16_t*>(this->data());
	}
	inline std::int16_t& top() const
	{
		return *reinterpret_cast<std::int16_t*>(this->data() + sizeof(std::int16_t));
	}
	inline std::int16_t& right() const
	{
		return *reinterpret_cast<std::int16_t*>(this->data() + 2 * sizeof(std::int16_t));
	}
	inline std::int16_t& bottom() const
	{
		return *reinterpret_cast<std::int16_t*>(this->data() + 3 * sizeof(std::int16_t));
	}
	inline std::uint8_t* data() const
	{
		return m_ptr;
	}
	inline std::size_t size() const
	{
		return STRUCT_SIZE;
	}
	static constexpr std::size_t STRUCT_SIZE = sizeof(std::uint32_t) * 2;
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
	static constexpr std::size_t STRUCT_SIZE = 48 + 2 * sizeof(long);
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

constexpr std::size_t COLOR_BIT_DEPTH = 24;
constexpr std::size_t COLOR_BYTE_DEPTH = COLOR_BIT_DEPTH / 8;

constexpr std::size_t aligned_byte_width(std::size_t width)
{
	return ((COLOR_BYTE_DEPTH * width) & 3) == 0?
		COLOR_BYTE_DEPTH * width:COLOR_BYTE_DEPTH * width + 4 - ((COLOR_BYTE_DEPTH * width) & 3);
}

constexpr std::size_t frame_size(std::size_t width, std::size_t height)
{
	return aligned_byte_width(width) * height;
}

constexpr std::size_t frame_chunk_size(std::size_t width, std::size_t height)
{
	return Chunk::STRUCT_SIZE + frame_size(width, height);
}

constexpr std::size_t frame_chunks_size(std::size_t width, std::size_t height, std::size_t frames)
{
	return (Chunk::STRUCT_SIZE + frame_size(width, height)) * frames;
}

constexpr std::size_t g_strlBufferSize = 2 * Chunk::STRUCT_SIZE + AVIStreamHeader::STRUCT_SIZE + BitmapInfoHeaderPtr::STRUCT_SIZE;
constexpr std::size_t g_hdrlBufferSize = Chunk::STRUCT_SIZE + MainAVIHeader::STRUCT_SIZE + g_strlBufferSize;
constexpr std::size_t moviBufferSize(std::size_t width, std::size_t height, std::size_t frames) 
{
	return List::STRUCT_SIZE + frame_chunks_size(width, height, frames);
}
constexpr std::size_t riffBufferSize(std::size_t width, std::size_t height, std::size_t frames)
{
	return RIFFHeader::STRUCT_SIZE + g_hdrlBufferSize + moviBufferSize(width, height, frames) + 2 * List::STRUCT_SIZE;
}

constexpr std::uint32_t make_fcc(char b0, char b1, char b2, char b3)
{
	return (std::uint32_t(b3) << 24) | (std::uint32_t(b2 << 16)) | (std::uint32_t(b1 << 8)) | (std::uint32_t(b0));
}

template <std::size_t N>
constexpr auto make_fcc(const char(&fcc_str)[N]) -> std::enable_if_t<N == 5, std::uint32_t>
{
	return fcc_str[5 - 1] == '\0' ? make_fcc(fcc_str[0], fcc_str[1], fcc_str[2], fcc_str[3]) : throw std::invalid_argument("Invalid FCC identifuer");
}

template <std::size_t N>
constexpr auto make_fcc(const char(&fcc_str)[N]) -> std::enable_if_t<N == 4, std::uint32_t>
{
	return fcc_str[3] != '\0' ? make_fcc(fcc_str[0], fcc_str[1], fcc_str[2], fcc_str[3]) : throw std::invalid_argument("Invalid FCC identifuer");
}

template <class Caller>
bool generateFrames(unsigned width, unsigned height, Caller&& get_value, unsigned frames,
	double val_min, double val_max, unsigned char* frData)
{
	auto cbPadding = aligned_byte_width(width) - width * COLOR_BYTE_DEPTH;
	for (unsigned f = 0; f < frames; ++f)
	{
		auto current_frame_offset = unsigned(frame_chunks_size(width, height, f));
		Chunk frame(frData, current_frame_offset);
		frame.chunk_id() = make_fcc("00db");
		frame.chunk_size() = unsigned(frame_size(width, height));
		for (unsigned l = 0; l < height; ++l)
		{
			for (unsigned k = 0; k < width; ++k)
			{
				bool successCode = ValToRGB(get_value(k, l, f), val_min, val_max, (RGBTRIPLE*)(frame.chunk_data() + aligned_byte_width(width) * l + COLOR_BYTE_DEPTH * k));
				if (!successCode)
					return false;
			}
			memset(frame.chunk_data() + aligned_byte_width(width) * l + COLOR_BYTE_DEPTH * width, 0, cbPadding);
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
	auto cbRiff = riffBufferSize(width, height, frames);

	std::unique_ptr<std::uint8_t[]> mem(generateAVIStructures(width, height, frames));

	RIFFHeader Riff(mem.get());
	generateFrames(width, height, get_value, frames, -10, 10, Riff.chunk_data() + List::STRUCT_SIZE + g_hdrlBufferSize);

	if (fwrite(Riff.data(), 1, cbRiff, aviFile.get()) < cbRiff)
		throw std::runtime_error("Could not write to a file");
}

#endif // !AVI_GENERATOR
 