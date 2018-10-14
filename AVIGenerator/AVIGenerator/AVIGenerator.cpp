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

struct avi_file_handle_close
{
	inline void operator()(std::FILE* fp) const noexcept
	{
		std::fclose(fp);
	}
};

using unique_avi_file_handle = std::unique_ptr<std::FILE, avi_file_handle_close>;

template <class Caller>
bool generateFrames(unsigned width, unsigned height, Caller&& get_value, unsigned frames,
	double val_min, double val_max, unsigned char* frData)
{
	auto cbPadding = std::uint32_t(width & 3);
	for (auto f = 0; f < frames; ++f)
	{
		for (unsigned l = 0; l < height; ++l)
		{
			for (unsigned k = 0; k < width; ++k)
			{
				bool successCode = ValToRGB(get_value(k, l, f), val_min, val_max, (RGBTRIPLE*) frData);
				if (!successCode)
					return false;
			}
			memcpy(frData.get(), 0, cbPadding));
		}
	}
	return true;
}

constexpr std::uint32_t make_fcc(char b0, char b1, char b2, char b3)
{
	return (std::uint32_t(b3) << 24) | (std::uint32_t(b2 << 16) ) | (std::uint32_t(b1 << 8) ) | (std::uint32_t(b0));
}

template <std::size_t N>
constexpr auto make_fcc(const char (&fcc_str)[N]) -> std::enable_if_t<N == 5, std::uint32_t>
{
	return fcc_str[5 - 1] == '\0' ? make_fcc(fcc_str[0], fcc_str[1], fcc_str[2], fcc_str[3]) : throw std::invalid_argument("Invalid FCC identifuer");
}

template <std::size_t N>
constexpr auto make_fcc(const char(&fcc_str)[N]) -> std::enable_if_t<N == 4, std::uint32_t>
{
	return fcc_str[3] != '\0' ? make_fcc(fcc_str[0], fcc_str[1], fcc_str[2], fcc_str[3]) : throw std::invalid_argument("Invalid FCC identifuer");
}

template <class Caller>
void generateAVI(const char* file_name, Caller&& get_value, unsigned width, unsigned height, unsigned frames, double val_min, double val_max, bool discard_file)
{
	Chunk RIFF;
	RIFF.chunk_id() = make_fcc("RIFF");
	Chunk AVI;
	AVI.chunk_id() = make_fcc("AVI "); //'AVI '

	List hdrl;
	hdrl.chunk_id() = le2be(hdrl.LIST_ID);
	hdrl.list_type() = le2be(0x6864726c); //'hdrl'
	MainAVIHeader mainAVI;
	List strl;
	strl.chunk_id() = le2be(strl.LIST_ID);
	strl.list_type() = le2be(0x7374726c); //'strl'
	Chunk streamHeader; //delete
	streamHeader.chunk_id() = le2be(0x73747268); //'strh'
	AVIStreamHeader strh(strl.list_data(), 0);

	Chunk streamFormat(strl.list_data(), sizeof(strh));
	streamFormat.chunk_id() = le2be(0x73747266); //'strf'
	BitmapInfoHeaderPtr bmInfo;

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
	strh.rect() = RECT(0, 0, le2be(width), le2be(-1 * height));

	streamHeader.chunk_size() = le2be(sizeof(AVIStreamHeader));
	streamHeader.chunk_data() = strh.data();
	
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

	streamFormat.chunk_size() = bmInfo.size;
	streamFormat.chunk_data() = bmInfo.data();

	//STRL
	strl.list_data() = streamHeader.data();

	memcpy(strl.listData.get(), &streamHeader.ckID, sizeof(streamHeader.ckID));
	memcpy(strl.listData.get(), &streamHeader.ckSize, sizeof(streamHeader.ckSize));
	memcpy(strl.listData.get(), &streamHeader.ckData, streamHeader.ckSize);

	//MOVI
	List movi;
	movi.chunk_id() = le2be(movi.LIST_ID);
	movi.list_type() = le2be(0x6d6f7669); //'movi'

	Chunk frame;
	frame.chunk_id() = le2be(0x00006462);
	frame.chunk_size() = le2be((frames * 24 * width + std::uint32_t(width & 3)) * height);
	generateFrames(width, height, get_value, frames, val_min, val_max, frame.chunk_data());

	movi.chunk_size() = le2be(frame.chunk_size() + sizeof(frame.chunk_id()) + sizeof(movi.list_type()));
	movi.chunk_data() = frame.data();

	auto aviFile = unique_avi_file_handle(std::fopen("animated.avi", "wb"));
	if (bool(aviFile) && !discard_file)
		return;


}

//void createAVI()
//{
//	Chunk RIFF;
//	RIFF.ckID = 0x52494646; //'RIFF'
//	Chunk AVI;
//	AVI.ckID = 0x415564920; //'AVI '
//
//	List hdrl;
//	hdrl.listType = 0x6864726c; //'hdrl'
//	MainAVIHeader mainAVI;
//	List strl;
//	strl.listType = 0x7374726c; //'strl'
//	Chunk streamHeader;
//	streamHeader.ckID = 0x73747268; //'strh'
//	AVIStreamHeader strh;
//	Chunk streamFormat;
//	streamFormat.ckID = 0x73747266; //'strf'
//	BITMAPINFOHEADER bmInfo;
//	auto f1 = unique_bmp_file_handle(std::fopen("f_1.bmp", "rb"));
//	if (bool(f1))
//		return;
//	//читаем bitmapinfo
//	fseek(f1.get(), 14, SEEK_CUR);
//	fread(&bmInfo, sizeof(BITMAPINFOHEADER), 1, f1.get());
//	fseek(f1.get(), 0, SEEK_END);
//	std::size_t frameSize = ftell(f1.get()); 
//	fseek(f1.get(), 0, SEEK_SET);
//
//	List movi;
//	movi.listType = 0x6d6f7669; //'movi'
//	//strf - BIMAPINFO
//
//	Chunk frame;
//	frame.ckID = 0x00006462;
//
//	//читаем 5 файлов
//	auto f1 = unique_bmp_file_handle(std::fopen("f_1.bmp", "rb"));
//	if (bool(f1))
//		return;
//	auto f2 = unique_bmp_file_handle(std::fopen("f_2.bmp", "rb"));
//	if (bool(f2))
//		return;
//	auto f3 = unique_bmp_file_handle(std::fopen("f_3.bmp", "rb"));
//	if (bool(f3))
//		return;
//	auto f4 = unique_bmp_file_handle(std::fopen("f_4.bmp", "rb"));
//	if (bool(f4))
//		return;
//	auto f5 = unique_bmp_file_handle(std::fopen("f_5.bmp", "rb"));
//	if (bool(f5))
//		return;
//	std::unique_ptr<unsigned char[]> buff;
//	std::fread(buff.get(), frameSize, 1, f1.get());
//	std::fread(buff.get(), frameSize, 1, f2.get());
//	std::fread(buff.get(), frameSize, 1, f3.get());
//	std::fread(buff.get(), frameSize, 1, f4.get());
//	std::fread(buff.get(), frameSize, 1, f5.get());
//}