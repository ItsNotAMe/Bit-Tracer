#pragma once
#include "Common.h"

#include <mutex>
#include <string>

#include "Hittable.h"
#include "HittableList.h"

struct RayTracerSettings
{
    int Width = 400;
    float AspectRatio = 16.0f / 9.0f;
    float VFOV = 90;
    Point3 LookFrom = Point3(0, 0, 0); // Point camera is looking from
    Point3 LookAt = Point3(0, 0, -1); // Point camera is looking at
    Vec3 UpVec = Vec3(0, 1, 0); // Camera-relative "up" direction
    int SamplesPerPixel = 10;
    int MaxDepth = 10;

    float DefocusAngle = 0; // Variation angle of rays through each pixel
    float FocusDistance = 10; // Distance from camera lookfrom point to plane of perfect focus
};

class RayTracer
{
public:
    RayTracer(RayTracerSettings settings);

    void render(const std::string& outputFile) const;

    void setSettings(RayTracerSettings settings);
    void addObject(std::shared_ptr<Hittable> obj) { m_objects.add(obj); }
private:
    void initialize();
    Ray getRay(int x, int y) const;
    Vec3 sampleSquare() const;
    Vec3 defocusDistSample() const;
    Color rayColor(const Ray& r, int depth) const;
    void setPixel(std::vector<uint8_t>& image, int imageWidth, int x, int y, const Color& pixelColor) const;
    void addToPixel(std::vector<float>& imageIntensities, int imageWidth, int x, int y, const Color& pixelColor) const;
private:
    float m_aspectRatio;
    int m_imageWidth;
    int m_imageHeight;

    float m_vfov;
    Point3 m_lookFrom;
    Point3 m_lookAt;
    Vec3 m_vUp;

    int m_samplesPerPixel;
    int m_maxDepth;

    Point3 m_pixel00Loc; // Location of pixel 0, 0
    Vec3 m_pixelDeltaU; // Offset to pixel to the right
    Vec3 m_pixelDeltaV; // Offset to pixel below
    Vec3 m_u, m_v, m_w; // Camera frame basis vectors

    float m_defocusAngle = 0;
    float m_focusDistance = 10;
    Vec3 m_defocusDiskU; // Defocus disk horizontal radius
    Vec3 m_defocusDiskV; // Defocus disk vertical radius

    HittableList m_objects;

    static std::mutex s_imageMutex;
};