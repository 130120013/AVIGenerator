#include "AVIGenerator.h"
#include <type_traits>

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

template <class Caller>
void generateAVI(const char* file_name, Caller&& get_value, unsigned width, unsigned height, unsigned frames, double val_min, double val_max, bool discard_file)
{
	Chunk RIFF;
	RIFF.ckID = le2be(0x52494646); //'RIFF'
	Chunk AVI;
	AVI.ckID = le2be(0x415564920); //'AVI '

	List hdrl;
	hdrl.listType = le2be(0x6864726c); //'hdrl'
	MainAVIHeader mainAVI;
	List strl;
	strl.listType = le2be(0x7374726c); //'strl'
	Chunk streamHeader;
	streamHeader.ckID = le2be(0x73747268); //'strh'
	AVIStreamHeader strh;
	streamHeader.ckSize = le2be(sizeof(AVIStreamHeader));
	Chunk streamFormat;
	streamFormat.ckID = le2be(0x73747266); //'strf'
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
	strh.rcFrame() = RECT(0, 0, width, -1 * height);

	//STRF
	streamFormat.ckSize = bmInfo.size;

	bmInfo->biSize = le2be(40);
	bmInfo->biWidth = le2be((std::uint32_t) width);
	bmInfo->biHeight = le2be((std::uint32_t) height);
	bmInfo->biPlanes = le2be(1);
	bmInfo->biBitCount = le2be(24);
	bmInfo->biCompression = 0;
	bmInfo->biSizeImage = 0;
	bmInfo->biXPelsPerMeter = 0;
	bmInfo->biYPelsPerMeter = 0;
	bmInfo->biClrUsed = 0;
	bmInfo->biClrImportant = 0;

	memcpy(streamFormat.ckData.get(), &bmInfo->biSize, sizeof(bmInfo->biSize));
	memcpy(streamFormat.ckData.get(), &bmInfo->biWidth, sizeof(bmInfo->biWidth));
	memcpy(streamFormat.ckData.get(), &bmInfo->biHeight, sizeof(bmInfo->biHeight));
	memcpy(streamFormat.ckData.get(), &bmInfo->biPlanes, sizeof(bmInfo->biPlanes));
	memcpy(streamFormat.ckData.get(), &bmInfo->biBitCount, sizeof(bmInfo->biBitCount));
	memcpy(streamFormat.ckData.get(), &bmInfo->biCompression, sizeof(bmInfo->biCompression));
	memcpy(streamFormat.ckData.get(), &bmInfo->biSizeImage, sizeof(bmInfo->biSizeImage));
	memcpy(streamFormat.ckData.get(), &bmInfo->biXPelsPerMeter, sizeof(bmInfo->biXPelsPerMeter));
	memcpy(streamFormat.ckData.get(), &bmInfo->biYPelsPerMeter, sizeof(bmInfo->biYPelsPerMeter));
	memcpy(streamFormat.ckData.get(), &bmInfo->biClrUsed, sizeof(bmInfo->biClrUsed));
	memcpy(streamFormat.ckData.get(), &bmInfo->biClrImportant, sizeof(bmInfo->biClrImportant));
		   
	//STRL
	memcpy(strl.listData.get(), &streamHeader.ckID, sizeof(streamHeader.ckID));
	memcpy(strl.listData.get(), &streamHeader.ckSize, sizeof(streamHeader.ckSize));
	memcpy(strl.listData.get(), &streamHeader.ckData, streamHeader.ckSize);

	//MOVI
	List movi;
	movi.listType = le2be(0x6d6f7669); //'movi'

	Chunk frame;
	frame.ckID = le2be(0x00006462);
	frame.ckSize = le2be((frames * 24 * width + std::uint32_t(width & 3)) * height);
	generateFrames(width, height, get_value, frames, val_min, val_max, frame.ckData);

	memcpy(movi.listData.get(), &frame, sizeof(frame));
	movi.listSize = le2be(sizeof(frame) + sizeof(movi.listType));
	
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