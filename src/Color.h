#pragma once
#include "Utility.h"

#include "Vec3.h"

using Color = Vec3;

inline float linearToGamma(double linearComponent)
{
    if (linearComponent > 0)
        return std::sqrt(linearComponent);

    return 0;
}