#include "AVIGenerator.h"
#include <type_traits>
#include <stdexcept>

template <class T>
struct LE_BE_conversion
{
	static_assert(std::is_integral_v<T> && (sizeof(T) == 8 || sizeof(T) == 4 || sizeof(T) == 2), "T must be an integral type of 8, 4 or 2 bytes");
	typedef T type;
	typedef std::conditional_t<
		sizeof(T) == 8, 
		std::uint64_t,
		std::conditional_t<
			sizeof(T) == 4,
			std::uint32_t,
			std::conditional_t<
				sizeof(T) == 2,
				std::uint16_t, 
				std::uint8_t
			>
		>
	> internal_type;
	
	typedef std::conditional_t<
		sizeof(T) == 8,
		std::uint32_t,
		std::conditional_t<
			sizeof(T) == 4,
			std::uint16_t,
			std::uint8_t
		>
	> half_type;
};

template <class T>
constexpr auto le2be(T num) noexcept -> std::enable_if_t<sizeof(T) == 1, T>
{
	return num;
}

template <class T>
constexpr auto le2be(T num) noexcept -> std::enable_if_t<(sizeof(T) > 1), T>
{
	typedef LE_BE_conversion<T> converter;
	return T(typename converter::internal_type(le2be(typename converter::half_type(typename LE_BE_conversion<T>::internal_type(num) >> (sizeof(T) * 4)))) |
		typename converter::internal_type(le2be(typename LE_BE_conversion<T>::half_type(num))) << (sizeof(T) * 4));
}

std::unique_ptr<std::uint8_t[]> generateAVIStructures(unsigned width, unsigned height, unsigned frames)
{
	auto cbRiff = riffBufferSize(width, height, frames);
	auto buff = std::make_unique<std::uint8_t[]>(cbRiff);

	RIFFHeader RIFF(buff.get());
	RIFF.chunk_id() = make_fcc("RIFF");
	RIFF.chunk_size() = unsigned(cbRiff - RIFFHeader::STRUCT_SIZE + sizeof(std::uint32_t));
	RIFF.file_type() = make_fcc("AVI ");

	//hdrl
	List hdrl(RIFF.data(), RIFFHeader::STRUCT_SIZE);
	hdrl.chunk_id() = make_fcc("LIST");
	hdrl.chunk_size() = g_hdrlBufferSize + sizeof(std::uint32_t);
	hdrl.list_type() = make_fcc("hdrl"); //'hdrl'

	//avih
	MainAVIHeader mainAVI(hdrl.list_data(), 0);
	mainAVI.chunk_id() = make_fcc("avih");
	mainAVI.chunk_size() = MainAVIHeader::STRUCT_SIZE;
	mainAVI.dwMicroSecPerFrame() = 0;
	mainAVI.dwMaxBytesPerSec() = (frames * 24 * width + std::uint32_t(width & 3)) * 25 * height;
	mainAVI.dwPaddingGranularity() = 4;
	mainAVI.dwFlags() = 0;
	mainAVI.dwTotalFrames() = frames / 25;
	mainAVI.dwInitialFrames() = 0;
	mainAVI.dwStreams() = 1;
	mainAVI.dwSuggestedBufferSize() = 0;
	mainAVI.dwWidth() = width;
	mainAVI.dwHeight() = height;
	std::uint32_t reserved = 0;
	memset(&mainAVI.dwReserved(), 0, 4 * sizeof(std::uint32_t));

	//STRL 
	List strl(hdrl.list_data(), MainAVIHeader::STRUCT_SIZE + Chunk::STRUCT_SIZE);
	strl.chunk_id() = make_fcc("LIST");
	strl.chunk_size() = g_strlBufferSize + sizeof(std::uint32_t);
	strl.list_type() = make_fcc("strl"); //'strl'
	Chunk streamHeader(strl.list_data(), 0);
	streamHeader.chunk_id() = make_fcc("strh"); //'strh'
	streamHeader.chunk_size() = AVIStreamHeader::STRUCT_SIZE;
	AVIStreamHeader strh(streamHeader.chunk_data());
	strh.fccType() = make_fcc("vids");

	Chunk streamFormat(strl.list_data(), Chunk::STRUCT_SIZE + AVIStreamHeader::STRUCT_SIZE);
	streamFormat.chunk_id() = make_fcc("strf"); //'strf'
	streamFormat.chunk_size() = BitmapInfoHeaderPtr::STRUCT_SIZE;
	BitmapInfoHeaderPtr bmInfo(streamFormat.chunk_data());

	//STRH
	strh.fccHandler() = 0;
	strh.dwFlags() = 0;
	strh.wPriority() = 0;
	strh.wLanguage() = 0;
	strh.dwInitialFrames() = 0;
	strh.dwScale() = 1;
	strh.dwRate() = 25;
	strh.dwStart() = 0;
	strh.dwLength() = frames;
	strh.dwSuggestedBufferSize() = 0;
	strh.dwQuality() = -1;
	strh.dwSampleSize() = 0;
	RECT r(strh.rect());
	r.left() = 0;
	r.top() = 0;
	r.right() = width;
	r.bottom() = height;

	//STRF
	bmInfo.Size() = 40;
	bmInfo.Width() = (std::uint32_t) width;
	bmInfo.Height() = std::uint32_t() - (std::uint32_t) height;
	bmInfo.Plains() = 1;
	bmInfo.BitCount() = 24;
	bmInfo.Compression() = 0;
	bmInfo.SizeImage() = 0;
	bmInfo.XPelsPerMeter() = 0;
	bmInfo.YPelsPerMeter() = 0;
	bmInfo.ClrUsed() = 0;
	bmInfo.ClrImportant() = 0;

	//MOVI
	List movi(RIFF.chunk_data(), g_hdrlBufferSize);
	movi.chunk_id() = make_fcc("LIST");
	movi.chunk_size() = unsigned(frame_chunks_size(width, height, frames) + sizeof(std::remove_reference_t<decltype(movi.list_type())>));
	movi.list_type() = make_fcc("movi"); //'movi'

	return buff;
}




