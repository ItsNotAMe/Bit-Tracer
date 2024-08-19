#pragma once

// Common Headers

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <chrono>
#include <string>
#include <sstream>

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

inline std::string formatDuration(std::chrono::microseconds microseconds)
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(microseconds);
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
    secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);
    auto hour = std::chrono::duration_cast<std::chrono::hours>(mins);
    mins -= std::chrono::duration_cast<std::chrono::minutes>(hour);

    std::stringstream ss;
    ss << hour.count() << "h::" << mins.count() << "m::" << secs.count() << "s::" << ms.count() << "ms";
    return ss.str();
}

inline void eraseLines(int count, std::stringstream& ss)
{
    if (count > 0)
    {
        ss << "\x1b[2K"; // Delete current line
        // i=1 because we included the first line
        for (int i = 1; i < count; i++)
        {
            ss
                << "\x1b[1A" // Move cursor up one
                << "\x1b[2K"; // Delete the entire line
        }
        ss << "\r"; // Resume the cursor at beginning of line
    }
}