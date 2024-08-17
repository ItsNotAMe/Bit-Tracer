#pragma once

#include <string>

class BitImage
{
public:
    BitImage() {}
    BitImage(const std::string& filePath);
    ~BitImage();

    int width() const { return (m_fdata == nullptr) ? 0 : m_imageWidth; }
    int height() const { return (m_fdata == nullptr) ? 0 : m_imageHeight; }

    bool load(const std::string& filePath);

    const unsigned char* pixelData(int x, int y) const;
private:
    void convertToBytes();
private:
    static int clamp(int x, int low, int high);
    static unsigned char floatToByte(float value);
private:
    const int m_bytesPerPixel = 3;
    float* m_fdata = nullptr; // Linear floating point pixel data
    unsigned char* m_bdata = nullptr; // Linear 8-bit pixel data
    int m_imageWidth = 0; // Loaded image width
    int m_imageHeight = 0; // Loaded image height
    int m_bytesPerScanline = 0;
};