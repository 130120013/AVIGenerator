#include <cstdint>
#ifndef IMAGE_GENERATOR
#define IMAGE_GENERATOR

struct RGBTRIPLE
{
	std::uint8_t rgbBlue;
	std::uint8_t rgbGreen;
	std::uint8_t rgbRed;
};

struct BitmapInfoHeaderPtr
{
	BitmapInfoHeaderPtr() = default;
	BitmapInfoHeaderPtr(std::uint8_t* pBuf, std::uint32_t offset = 0) :m_ptr(pBuf + offset) {}
	inline std::uint32_t& Size() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data());
	}
	inline std::int32_t& Width() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 4);
	}
	inline std::int32_t& Height() const
	{
		return *reinterpret_cast<std::int32_t*>(this->data() + 8);
	}
	inline std::uint16_t& Plains() const
	{
		return *reinterpret_cast<std::uint16_t*>(this->data() + 12);
	}
	inline std::uint16_t& BitCount() const
	{
		return *reinterpret_cast<std::uint16_t*>(this->data() + 14);
	}
	inline std::uint32_t& Compression() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 16);
	}
	inline std::uint32_t& SizeImage() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 20);
	}
	inline std::uint32_t& XPelsPerMeter() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 24);
	}
	inline std::uint32_t& YPelsPerMeter() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 28);
	}
	inline std::uint32_t& ClrUsed() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 32);
	}
	inline std::uint32_t& ClrImportant() const
	{
		return *reinterpret_cast<std::uint32_t*>(this->data() + 36);
	}
	inline std::uint8_t* data() const
	{
		return m_ptr;
	}
	static constexpr std::uint32_t STRUCT_SIZE = 40;
private:
	std::uint8_t* m_ptr = nullptr;
};

#define RANGES 8
#define RANGE_COLORS 255
#define LAST_RANGE_COLORS 1
#define MAX_COLORS ((RANGES - 1) * RANGE_COLORS + LAST_RANGE_COLORS)

static bool ValToRGB(double nVal, double nMin, double nMax, RGBTRIPLE* colour);

#endif // !IMAGE_GENERATOR

