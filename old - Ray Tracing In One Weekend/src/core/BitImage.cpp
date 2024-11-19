#include "BitImage.h"

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
#pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <cstdlib>
#include <iostream>

BitImage::BitImage(const std::string& filePath)
{
    // Loads image data from the specified file. If the RTW_IMAGES environment variable is
    // defined, looks only in that directory for the image file. If the image was not found,
    // searches for the specified image file first from the current directory, then in the
    // images/ subdirectory, then the _parent's_ images/ subdirectory, and then _that_
    // parent, on so on, for six levels up. If the image was not loaded successfully,
    // width() and height() will return 0.

    auto imageDir = getenv("BIT_IMAGES");

    // Hunt for the image file in some likely locations.
    if (imageDir && load(std::string(imageDir) + "/" + filePath)) return;
    if (load(filePath)) return;
    if (load("images/" + filePath)) return;
    if (load("../images/" + filePath)) return;
    if (load("../../images/" + filePath)) return;
    if (load("../../../images/" + filePath)) return;
    if (load("../../../../images/" + filePath)) return;
    if (load("../../../../../images/" + filePath)) return;
    if (load("../../../../../../images/" + filePath)) return;

    std::cerr << "ERROR: Could not load image file '" << filePath << "'.\n";
}

BitImage::~BitImage()
{
    delete[] m_bdata;
    STBI_FREE(m_fdata);
}

bool BitImage::load(const std::string& filePath)
{
    // Loads the linear (gamma=1) image data from the given file name. Returns true if the
    // load succeeded. The resulting data buffer contains the three [0.0, 1.0]
    // floating-point values for the first pixel (red, then green, then blue). Pixels are
    // contiguous, going left to right for the width of the image, followed by the next row
    // below, for the full height of the image.

    int n = m_bytesPerPixel; // Dummy out parameter: original components per pixel
    m_fdata = stbi_loadf(filePath.c_str(), &m_imageWidth, &m_imageHeight, &n, m_bytesPerPixel);
    if (m_fdata == nullptr) return false;

    m_bytesPerScanline = m_imageWidth * m_bytesPerPixel;
    convertToBytes();
    return true;
}

const unsigned char* BitImage::pixelData(int x, int y) const
{
    // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
    // data, returns magenta.
    static unsigned char magenta[] = { 255, 0, 255 };
    if (m_bdata == nullptr) return magenta;

    x = clamp(x, 0, m_imageWidth);
    y = clamp(y, 0, m_imageHeight);

    return m_bdata + y * m_bytesPerScanline + x * m_bytesPerPixel;
}

void BitImage::convertToBytes()
{
    // Convert the linear floating point pixel data to bytes, storing the resulting byte
    // data in the `bdata` member.

    int total_bytes = m_imageWidth * m_imageHeight * m_bytesPerPixel;
    m_bdata = new unsigned char[total_bytes];

    // Iterate through all pixel components, converting from [0.0, 1.0] float values to
    // unsigned [0, 255] byte values.

    auto* bptr = m_bdata;
    auto* fptr = m_fdata;
    for (int i = 0; i < total_bytes; i++, fptr++, bptr++)
        *bptr = floatToByte(*fptr);
}

int BitImage::clamp(int x, int low, int high)
{
    // Return the value clamped to the range [low, high).
    if (x <= low)
        return low;
    if (x < high)
        return x;
    return high - 1;
}

unsigned char BitImage::floatToByte(float value)
{
    if (value <= 0.0)
        return 0;
    if (1.0 <= value)
        return 255;
    return static_cast<unsigned char>(256.0 * value);
}

// Restore MSVC compiler warnings
#ifdef _MSC_VER
#pragma warning (pop)
#endif