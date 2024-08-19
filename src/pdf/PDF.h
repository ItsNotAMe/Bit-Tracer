#pragma once
#include "Common.h"

#include "core/ONB.h"

class PDF
{
public:
    virtual ~PDF() {}

    virtual float value(const Vec3& direction) const = 0;
    virtual Vec3 generate() const = 0;
};