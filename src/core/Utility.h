#pragma once

// Common Headers

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

// Constants

#define INF std::numeric_limits<float>::infinity()
#define PI 3.1415926535897932385f

// Utility Functions

inline float degreesToRadians(float degrees)
{
    return degrees * PI / 180.0;
}

inline float randomFloat()
{
    // Returns a random float in [0,1).
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::mt19937 generator;
    return distribution(generator);
}

inline float randomFloat(float min, float max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * randomFloat();
}

inline int randomInt(int min, int max)
{
    // Returns a random int in [min,max].
    return int(randomFloat(min, max + 1));
}