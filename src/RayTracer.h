#pragma once
#include "Common.h"

#include <mutex>
#include <string>
#include <atomic>

#include "Camera.h"
#include "hittable/Hittable.h"
#include "hittable/HittableList.h"

struct RayTracerSettings
{
    Camera Camera;
    int Width = 400;
    float AspectRatio = 16.0f / 9.0f;
    int SamplesPerPixel = 10;
    int MaxDepth = 10;
    Color Background = Color(0);
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
    Color rayColor(const Ray& r, Hittable& objects, int depth) const;
    void setPixel(std::vector<uint8_t>& image, int imageWidth, int x, int y, const Color& pixelColor) const;
    void addToPixel(std::vector<std::atomic<float>>& imageIntensities, int imageWidth, int x, int y, const Color& pixelColor) const;
private:
    float m_aspectRatio;
    int m_imageWidth;
    int m_imageHeight;

    Camera m_camera;

    int m_samplesPerPixel;
    int m_maxDepth;

    Color m_background;

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