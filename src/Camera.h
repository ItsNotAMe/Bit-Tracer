#pragma once
#include "Common.h"

struct Camera
{
    Point3 LookFrom = Point3(0, 0, 0); // Point camera is looking from
    Point3 LookAt = Point3(0, 0, -1); // Point camera is looking at
    Vec3 UpVec = Vec3(0, 1, 0); // Camera-relative "up" direction
    float VFOV = 90;
};