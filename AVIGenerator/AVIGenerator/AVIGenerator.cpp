#include "AVIGenerator.h"

template <class T>
T LittleEndianToBigEndian(T num)
{
	if (sizeof(T) == 32) //for std::uint32_t and long x32
	{
		return ((num >> 24) & 0xFF) |
			((num << 8) & 0xFF0000) |
			((num >> 8) & 0xFF00) |
			((num << 24) & 0xFF000000);
	}
	num = ((num << 8) & 0xFF00FF00FF00FF00ULL) | ((num >> 8) & 0x00FF00FF00FF00FFULL); //for long x64
	num = ((num << 16) & 0xFFFF0000FFFF0000ULL) | ((num >> 16) & 0x0000FFFF0000FFFFULL);
	return (num << 32) | ((num >> 32) & 0xFFFFFFFFULL);
}

struct bmp_file_handle_close
{
	inline void operator()(std::FILE* fp) const noexcept
	{
		std::fclose(fp);
	}
};

using unique_bmp_file_handle = std::unique_ptr<std::FILE, bmp_file_handle_close>;

void createAVI()
{
	Chunk RIFF;
	RIFF.ckID = 0x52494646; //'RIFF'
	Chunk AVI;
	AVI.ckID = 0x415564920; //'AVI '

	List hdrl;
	hdrl.listType = 0x6864726c; //'hdrl'
	MainAVIHeader mainAVI;
	List strl;
	strl.listType = 0x7374726c; //'strl'
	Chunk streamHeader;
	streamHeader.ckID = 0x73747268; //'strh'
	AVIStreamHeader strh;
	Chunk streamFormat;
	streamFormat.ckID = 0x73747266; //'strf'
	BITMAPINFOHEADER bmInfo;
	auto f1 = unique_bmp_file_handle(std::fopen("f_1.bmp", "rb"));
	if (bool(f1))
		return;
	//читаем bitmapinfo
	fseek(f1.get(), 14, SEEK_CUR);
	fread(&bmInfo, sizeof(BITMAPINFOHEADER), 1, f1.get());
	fseek(f1.get(), 0, SEEK_END);
	std::size_t frameSize = ftell(f1.get()); 
	fseek(f1.get(), 0, SEEK_SET);

	List movi;
	movi.listType = 0x6d6f7669; //'movi'
	//strf - BIMAPINFO

	Chunk frame;
	frame.ckID = 0x00006462;

	//читаем 5 файлов
	auto f1 = unique_bmp_file_handle(std::fopen("f_1.bmp", "rb"));
	if (bool(f1))
		return;
	auto f2 = unique_bmp_file_handle(std::fopen("f_2.bmp", "rb"));
	if (bool(f2))
		return;
	auto f3 = unique_bmp_file_handle(std::fopen("f_3.bmp", "rb"));
	if (bool(f3))
		return;
	auto f4 = unique_bmp_file_handle(std::fopen("f_4.bmp", "rb"));
	if (bool(f4))
		return;
	auto f5 = unique_bmp_file_handle(std::fopen("f_5.bmp", "rb"));
	if (bool(f5))
		return;
	std::unique_ptr<unsigned char[]> buff;
	std::fread(buff.get(), frameSize, 1, f1.get());
	std::fread(buff.get(), frameSize, 1, f2.get());
	std::fread(buff.get(), frameSize, 1, f3.get());
	std::fread(buff.get(), frameSize, 1, f4.get());
	std::fread(buff.get(), frameSize, 1, f5.get());


}