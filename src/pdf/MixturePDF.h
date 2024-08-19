#pragma once
#include "Common.h"

#include "PDF.h"

class MixturePDF : public PDF
{
public:
    MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1)
    {
        m_p[0] = p0;
        m_p[1] = p1;
    }

    float value(const Vec3& direction) const override
    {
        return 0.5 * m_p[0]->value(direction) + 0.5 * m_p[1]->value(direction);
    }

    Vec3 generate() const override
    {
        if (randomFloat() < 0.5)
            return m_p[0]->generate();
        else
            return m_p[1]->generate();
    }
private:
    std::shared_ptr<PDF> m_p[2];
};