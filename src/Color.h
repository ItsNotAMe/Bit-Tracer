#pragma once

#include <vector>

#include "Vec3.h"

using Color = Vec3;

void setPixel(std::vector<uint8_t>& image, int imageWidth, int x, int y, const Color& pixelColor)
{
    auto r = pixelColor.x();
    auto g = pixelColor.y();
    auto b = pixelColor.z();

    // Translate the [0,1] component values to the byte range [0,255].
    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    // Write out the pixel color components.
    image[3 * (y * imageWidth + x)] = rbyte;
    image[3 * (y * imageWidth + x) + 1] = gbyte;
    image[3 * (y * imageWidth + x) + 2] = bbyte;
}