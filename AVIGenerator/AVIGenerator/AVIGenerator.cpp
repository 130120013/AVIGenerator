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

std::unique_ptr<std::uint8_t[]> generateAVIStructures(unsigned width, unsigned height, unsigned frames)
{
	std::size_t strlBufferSize = List::STRUCT_SIZE + 2 * Chunk::STRUCT_SIZE + AVIStreamHeader::STRUCT_SIZE + BitmapInfoHeaderPtr::STRUCT_SIZE;
	std::size_t hdrlBufferSize = List::STRUCT_SIZE + Chunk::STRUCT_SIZE + MainAVIHeader::STRUCT_SIZE + strlBufferSize;
	std::size_t moviBufferSize = List::STRUCT_SIZE + Chunk::STRUCT_SIZE + (frames * 24 * width + std::uint32_t(width & 3)) * height;
	std::size_t riffBufferSize = RIFFHeader::STRUCT_SIZE + Chunk::STRUCT_SIZE + hdrlBufferSize + moviBufferSize;
	auto buff = std::make_unique<std::uint8_t[]>(riffBufferSize);

	RIFFHeader RIFF(buff.get());
	RIFF.chunk_id() = make_fcc("RIFF");
	RIFF.chunk_size() = riffBufferSize - Chunk::STRUCT_SIZE;
	Chunk AVI(RIFF.chunk_data());
	AVI.chunk_id() = make_fcc("AVI "); //'AVI '
	AVI.chunk_size() = le2be(hdrlBufferSize);

	//hdrl
	List hdrl(AVI.chunk_data());
	hdrl.chunk_id() = make_fcc("LIST");
	hdrl.chunk_size() = le2be(hdrlBufferSize);
	hdrl.list_type() = make_fcc("hdrl"); //'hdrl'

	//avih
	MainAVIHeader mainAVI(hdrl.list_data(), 0);
	mainAVI.chunk_id() = make_fcc("avih");
	mainAVI.chunk_size() = le2be(MainAVIHeader::STRUCT_SIZE);
	mainAVI.dwMicroSecPerFrame() = 0;
	mainAVI.dwMaxBytesPerSec() = le2be((frames * 24 * width + std::uint32_t(width & 3)) * 25 * height);
	mainAVI.dwPaddingGranularity() = le2be(4);
	mainAVI.dwFlags() = 0;
	mainAVI.dwTotalFrames() = le2be(frames / 25);
	mainAVI.dwInitialFrames() = 0;
	mainAVI.dwStreams() = le2be(1);
	mainAVI.dwSuggestedBufferSize() = 0;
	mainAVI.dwWidth() = le2be(width);
	mainAVI.dwHeight() = le2be(height);
	memset(&mainAVI.dwReserved(), 0, 4 * sizeof(std::uint32_t));

	//STRL 
	List strl(hdrl.list_data(), MainAVIHeader::STRUCT_SIZE);
	strl.chunk_id() = make_fcc("LIST");
	strl.chunk_size() = le2be(strlBufferSize);
	strl.list_type() = make_fcc("strl"); //'strl'
	Chunk streamHeader(strl.list_data(), 0);
	streamHeader.chunk_id() = make_fcc("strh"); //'strh'
	streamHeader.chunk_size() = le2be(AVIStreamHeader::STRUCT_SIZE); //переполнение size_t? или проблемы с le2be
	AVIStreamHeader strh(streamHeader.chunk_data());
	strh.fccType() = make_fcc("vids");

	Chunk streamFormat(strl.list_data(), Chunk::STRUCT_SIZE + AVIStreamHeader::STRUCT_SIZE);
	streamFormat.chunk_id() = make_fcc("strf"); //'strf'
	streamFormat.chunk_size() = le2be(BitmapInfoHeaderPtr::STRUCT_SIZE);
	BitmapInfoHeaderPtr bmInfo(streamFormat.chunk_data());

	//STRH
	strh.fccHandler() = 0;
	strh.dwFlags() = 0;
	strh.wPriority() = 0;
	strh.wLanguage() = 0;
	strh.dwInitialFrames() = 0;
	strh.dwScale() = le2be(frames / 25);
	strh.dwRate() = le2be(25);
	strh.dwStart() = 0;
	strh.dwLength() = le2be(frames / 25);
	strh.dwSuggestedBufferSize() = 0;
	strh.dwQuality() = le2be(-1);
	strh.dwSampleSize() = 0;
	RECT r(strh.rect());
	r.left() = 0;
	r.top() = 0;
	r.right() = le2be(width);
	r.bottom() = le2be(-1 * height);

	//STRF
	bmInfo.Size() = le2be(40);
	bmInfo.Width() = le2be((std::uint32_t) width);
	bmInfo.Height() = le2be((std::uint32_t) height);
	bmInfo.Plains() = le2be(1);
	bmInfo.BitCount() = le2be(24);
	bmInfo.Compression() = 0;
	bmInfo.SizeImage() = 0;
	bmInfo.XPelsPerMeter() = 0;
	bmInfo.YPelsPerMeter() = 0;
	bmInfo.ClrUsed() = 0;
	bmInfo.ClrImportant() = 0;

	//MOVI
	std::size_t framesSize = (frames * 24 * width + std::uint32_t(width & 3)) * height;
	List movi(RIFF.chunk_data(), hdrlBufferSize);
	movi.chunk_id() = make_fcc("LIST");
	movi.list_type() = make_fcc("movi"); //'movi'
	movi.chunk_size() = le2be(framesSize + Chunk::STRUCT_SIZE + sizeof(movi.list_type()));

	Chunk frame(movi.chunk_data());
	frame.chunk_id() = make_fcc("00db");
	frame.chunk_size() = le2be(framesSize);

	return buff;
}




