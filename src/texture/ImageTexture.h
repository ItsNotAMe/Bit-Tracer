#pragma once
#include "Common.h"

#include "Texture.h"
#include "BitImage.h"

class ImageTexture : public Texture
{
public:
    ImageTexture(const std::string& filePath)
        : m_image(filePath)
    {
    }

    Color value(float u, float v, const Point3& p) const override
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (m_image.height() <= 0) return Color(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Interval(0, 1).clamp(u);
        v = 1.0f - Interval(0, 1).clamp(v);  // Flip V to image coordinates

        int x = int(u * m_image.width());
        int y = int(v * m_image.height());
        auto pixel = m_image.pixelData(x, y);

        float colorScale = 1.0f / 255.0f;
        return Color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
    }
private:
    BitImage m_image;
};