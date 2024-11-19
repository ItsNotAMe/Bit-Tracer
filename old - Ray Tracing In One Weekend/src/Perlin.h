#pragma once
#include "Common.h"

class Perlin
{
public:
    Perlin();

    float noise(const Point3& p) const;
    float turb(const Point3& p, int depth) const;
private:
    static void perlinGeneratePerm(int* p);
    static void permute(int* p, int n);
    static float perlinInterp(const Vec3 c[2][2][2], float u, float v, float w);
private:
    static const int s_pointCount = 256;
    Vec3 m_randVec[s_pointCount];
    int m_permX[s_pointCount];
    int m_permY[s_pointCount];
    int m_permZ[s_pointCount];
};