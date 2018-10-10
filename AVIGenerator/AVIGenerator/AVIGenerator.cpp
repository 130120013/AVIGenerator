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
constexpr auto LittleEndianToBigEndian(T num) noexcept -> std::enable_if_t<sizeof(T) == 1, T>
{
	return num;
}

template <class T>
constexpr auto LittleEndianToBigEndian(T num) noexcept -> std::enable_if_t<(sizeof(T) > 1), T>
{
	return T(LittleEndianToBigEndian(typename LE_BE_conversion<T>::half_type(typename LE_BE_conversion<T>::internal_type(num) >> (sizeof(T) * 4)) |
		LittleEndianToBigEndian(typename LE_BE_conversion<T>::half_type(num))));
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
	RIFF.ckID = LittleEndianToBigEndian(0x52494646); //'RIFF'
	Chunk AVI;
	AVI.ckID = LittleEndianToBigEndian(0x415564920); //'AVI '

	List hdrl;
	hdrl.listType = LittleEndianToBigEndian(0x6864726c); //'hdrl'
	MainAVIHeader mainAVI;
	List strl;
	strl.listType = LittleEndianToBigEndian(0x7374726c); //'strl'
	Chunk streamHeader;
	streamHeader.ckID = LittleEndianToBigEndian(0x73747268); //'strh'
	AVIStreamHeader strh;
	streamHeader.ckSize = LittleEndianToBigEndian(sizeof(AVIStreamHeader));
	Chunk streamFormat;
	streamFormat.ckID = LittleEndianToBigEndian(0x73747266); //'strf'
	BITMAPINFOHEADER bmInfo;
	streamFormat.ckSize = sizeof(BITMAPINFOHEADER);

	bmInfo->biSize = LittleEndianToBigEndian(40);
	bmInfo->biWidth = LittleEndianToBigEndian((std::uint32_t) width);
	bmInfo->biHeight = LittleEndianToBigEndian((std::uint32_t) height);
	bmInfo->biPlanes = LittleEndianToBigEndian(1);
	bmInfo->biBitCount = LittleEndianToBigEndian(24);
	bmInfo->biCompression = 0;
	bmInfo->biSizeImage = 0;
	bmInfo->biXPelsPerMeter = 0;
	bmInfo->biYPelsPerMeter = 0;
	bmInfo->biClrUsed = 0;
	bmInfo->biClrImportant = 0;

	memcpy(streamFormat.ckData.get(), &bmInfo, sizeof(BITMAPINFOHEADER));

	auto aviFile = unique_avi_file_handle(std::fopen("animated.avi", "wb"));
	if (bool(aviFile) && !discard_file)
		return;

	List movi;
	movi.listType = LittleEndianToBigEndian(0x6d6f7669); //'movi'
								//strf - BIMAPINFO

	Chunk frame;
	frame.ckID = LittleEndianToBigEndian(0x00006462);
	frame.ckSize = LittleEndianToBigEndian((frames * 24 * width + std::uint32_t(width & 3)) * height);
	generateFrames(width, height, get_value, frames, val_min, val_max, frame.ckData);

	memcpy(movi.listData.get(), &frame, sizeof(frame));
	movi.listSize = LittleEndianToBigEndian(sizeof(frame) + sizeof(movi.listType));




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