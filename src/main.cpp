#include <iostream>
#include <direct.h>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Vec3.h"
#include "Color.h"
#include "Ray.h"

Color rayColor(const Ray &r)
{
    Vec3 unitDirection = unitVector(r.direction());
    float a = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

int main()
{
    float aspectRatio = 16.0f / 9.0f;
    int imageWidth = 400;

    // Calculate the image height, and ensure that it's at least 1.
    int imageHeight = int(imageWidth / aspectRatio);
    imageHeight = (imageHeight < 1) ? 1 : imageHeight;

    // Viewport widths less than one are ok since they are real valued.
    float focalLength = 1.0f;
    float viewportHeight = 2.0f;
    float viewportWidth = viewportHeight * (float(imageWidth) / imageHeight);
    Point3 cameraPos = {0};

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewportU = Vec3(viewportWidth, 0, 0);
    auto viewportV = Vec3(0, -viewportHeight, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixelDeltaU = viewportU / imageWidth;
    auto pixelDeltaV = viewportV / imageHeight;

    // Calculate the location of the upper left pixel.
    auto viewportUpperLeft = cameraPos - Vec3(0, 0, focalLength) - viewportU / 2 - viewportV / 2;
    auto pixel00Loc = viewportUpperLeft + 0.5 * (pixelDeltaU + pixelDeltaV);

    std::vector<uint8_t> image(imageWidth * imageHeight * 3);

    for (int y = 0; y < imageHeight; y++)
    {
        std::clog << "\rScanlines remaining: " << (imageHeight - y) << ' ' << std::flush;
        for (int x = 0; x < imageWidth; x++)
        {
            Point3 pixelPos = pixel00Loc + (x * pixelDeltaU) + (y * pixelDeltaV);
            auto rayDirection = pixelPos - cameraPos;
            Ray ray(cameraPos, rayDirection);

            Color pixelColor = rayColor(ray);
            setPixel(image, imageWidth, x, y, pixelColor);
        }
    }

    _mkdir("output");
    stbi_write_png("output/image.png", imageWidth, imageHeight, 3, image.data(), imageWidth * 3);

    std::clog << "\rDone.                 \n";
    return 0;
}