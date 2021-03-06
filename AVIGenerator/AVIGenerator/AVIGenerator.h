#include <cstdint>
#include <memory>
#include <cstdio>
#include <vector>
#include <future>
#include <system_error>
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
	AVIStreamHeader(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset){}

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

constexpr std::size_t g_strlBufferSize = List::STRUCT_SIZE + 2 * Chunk::STRUCT_SIZE + AVIStreamHeader::STRUCT_SIZE + BitmapInfoHeaderPtr::STRUCT_SIZE;
constexpr std::size_t g_hdrlBufferSize = List::STRUCT_SIZE + Chunk::STRUCT_SIZE + MainAVIHeader::STRUCT_SIZE + g_strlBufferSize;
constexpr std::size_t moviBufferSize(std::size_t width, std::size_t height, std::size_t frames) 
{
	return List::STRUCT_SIZE + frame_chunks_size(width, height, frames);
}
constexpr std::size_t riffBufferSize(std::size_t width, std::size_t height, std::size_t frames)
{
	return RIFFHeader::STRUCT_SIZE + g_hdrlBufferSize + moviBufferSize(width, height, frames);
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

class AviOutputFile
{
	unique_avi_file_handle m_file;
	std::mutex m_mtx_file; //guards fseek and fwrite
public:
	AviOutputFile() = default;
	inline AviOutputFile(const char* pszFileName) :m_file(unique_avi_file_handle(std::fopen(pszFileName, "wb"))) {}
	AviOutputFile(const AviOutputFile&) = delete;
	AviOutputFile& operator=(const AviOutputFile&) = delete;
	AviOutputFile(AviOutputFile&&) = delete;
	AviOutputFile& operator=(AviOutputFile&&) = delete;

	inline bool is_opened() const
	{
		return bool(m_file);
	}
	inline std::size_t write_at(std::size_t where, const void* pData, std::size_t cbData)
	{
		if (!cbData)
			return where;
		std::lock_guard<std::mutex> lock(m_mtx_file);
		std::fseek(m_file.get(), long(where), SEEK_SET);
		auto cbResult = std::fwrite(pData, 1, cbData, m_file.get());
		if (!cbResult)
			throw std::system_error(errno, std::generic_category());
		return where + cbResult;
	}
	inline FILE* file() const
	{
		return m_file.get();
	}
};

template <class Caller>
bool generateFrames(unsigned width, unsigned height, Caller&& get_value, unsigned frames, double val_min, double val_max, AviOutputFile& output)
{
	auto cbPadding = aligned_byte_width(width) - width * COLOR_BYTE_DEPTH;
	std::vector<std::future<bool>> futures;

	futures.reserve(frames);
	std::size_t frChunkSize = frame_chunk_size(width, height);
	std::size_t FRAME_BUFFER_SIZE =  frChunkSize * 5;
	std::size_t FRAMES_PER_BUFFER = FRAME_BUFFER_SIZE / frChunkSize;
	auto pFrameBuf = std::make_unique<std::uint8_t[]>(FRAMES_PER_BUFFER * frChunkSize);
	auto offset = RIFFHeader::STRUCT_SIZE + g_hdrlBufferSize + Chunk::STRUCT_SIZE;
	for (unsigned f1 = 0; f1 < frames; f1 += (unsigned) FRAMES_PER_BUFFER)
	{
		for (unsigned f = 0; f < FRAMES_PER_BUFFER; ++f)
		{
			futures.emplace_back(std::async(std::launch::async, [frChunkSize, cbPadding, f, f1, &pFrameBuf](unsigned width, unsigned height, std::reference_wrapper<std::decay_t<Caller>> get_value,
				double val_min, double val_max) -> bool
			{
				std::size_t current_frame_offset = frChunkSize * f;
				Chunk frame(pFrameBuf.get(), current_frame_offset);
				frame.chunk_id() = make_fcc("00db");
				frame.chunk_size() = unsigned(frame_size(width, height));
				for (unsigned l = 0; l < height; ++l)
				{
					for (unsigned k = 0; k < width; ++k)
					{
						bool successCode = ValToRGB(get_value(k, l, f + f1), val_min, val_max, (RGBTRIPLE*)(frame.chunk_data() + aligned_byte_width(width) * l + COLOR_BYTE_DEPTH * k));
						if (!successCode)
							return false;
					}
					memset(frame.chunk_data() + aligned_byte_width(width) * l + COLOR_BYTE_DEPTH * width, 0, cbPadding);

				}
				return true;
			}, width, height, std::ref(get_value), val_min, val_max));

		}
	
		for (std::size_t iFut = f1; iFut < f1 + FRAMES_PER_BUFFER; ++iFut)
		{
			if (!futures[iFut].get())
				return false;
		}
		offset = output.write_at(offset, pFrameBuf.get(), frChunkSize * FRAMES_PER_BUFFER);

	}

	return true;
}

template <class Caller>
void generateAVI(const char* file_name, Caller&& get_value, unsigned width, unsigned height, unsigned frames, double val_min, double val_max, bool discard_file)
{
	AviOutputFile aviFile(file_name);
	if (aviFile.is_opened() && !discard_file)
		return;
	auto cbRiff = riffBufferSize(width, height, frames);

	std::unique_ptr<std::uint8_t[]> mem(generateAVIStructures(width, height, frames));

	RIFFHeader Riff(mem.get());

	if (fwrite(Riff.data(), 1, cbRiff, aviFile.file()) < cbRiff)
		throw std::runtime_error("Could not write to a file");
	generateFrames(width, height, get_value, frames, val_min, val_max, aviFile);
}

#endif // !AVI_GENERATOR
 