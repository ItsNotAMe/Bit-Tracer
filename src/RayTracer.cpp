#include "RayTracer.h"

#include <direct.h>
#include <vector>
#include <functional>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "ThreadPool.h"
#include "materials/Material.h"
#include "hittable/BVH.h"

std::mutex RayTracer::s_imageMutex;

RayTracer::RayTracer(RayTracerSettings settings)
    : m_aspectRatio(settings.AspectRatio), m_imageWidth(settings.Width), m_camera(settings.Camera),
    m_samplesPerPixel(settings.SamplesPerPixel), m_maxDepth(settings.MaxDepth),
    m_defocusAngle(settings.DefocusAngle), m_focusDistance(settings.FocusDistance)
{
    initialize();
}

void RayTracer::render(const std::string& outputFile) const
{
    HittableList BVHObjects = HittableList(std::make_shared<BVHNode>(m_objects));

    std::vector<float> imageIntensities(m_imageWidth * m_imageHeight * 3);
    ThreadPool pool(std::thread::hardware_concurrency());
    for (int sample = 0; sample < m_samplesPerPixel; sample++)
    {
        pool.addTask([&]() {
            for (int y = 0; y < m_imageHeight; y++)
            {
                for (int x = 0; x < m_imageWidth; x++)
                {
                    Ray ray = getRay(x, y);
                    Color pixelColor = rayColor(ray, BVHObjects, m_maxDepth);
                    addToPixel(imageIntensities, m_imageWidth, x, y, pixelColor);
                }
            }
            });
    }

    while (1)
    {
        int remaining = pool.getQueueSize() + pool.getCountWorking();
        std::clog << "\rSamples remaining: " << remaining << " Currently working on: " << pool.getCountWorking() << "  " << std::flush;
        if (remaining == 0)
            break;
        pool.waitForUpdate();
    }

    std::vector<uint8_t> image(m_imageWidth * m_imageHeight * 3);
    for (int y = 0; y < m_imageHeight; y++)
    {
        for (int x = 0; x < m_imageWidth; x++)
        {
            setPixel(image, m_imageWidth, x, y,
                Color(imageIntensities[3 * (y * m_imageWidth + x)] / m_samplesPerPixel,
                    imageIntensities[3 * (y * m_imageWidth + x) + 1] / m_samplesPerPixel,
                    imageIntensities[3 * (y * m_imageWidth + x) + 2] / m_samplesPerPixel));
        }
    }

    // TODO: split work better between threads in case on part of the image takes longer to render
    // std::atomic<int> renderedLines = 0;
    // std::vector<uint8_t> image(m_imageWidth * m_imageHeight * 3);
    // {
    //     int nThreads = std::thread::hardware_concurrency();
    //     ThreadPool pool(nThreads);
    //     for (int i = 0; i < nThreads; i++)
    //     {
    //         pool.addTask([&](int yStart, int yEnd) {
    //             for (int y = yStart; y < yEnd; y++)
    //             {
    //                 for (int x = 0; x < m_imageWidth; x++)
    //                 {
    //                     Color pixelColor = { 0 };
    //                     for (int sample = 0; sample < m_samplesPerPixel; sample++)
    //                     {
    //                         Ray ray = getRay(x, y);

    //                         pixelColor += rayColor(ray, BVHObjects, m_maxDepth);
    //                     }
    //                     pixelColor /= m_samplesPerPixel;
    //                     setPixel(image, m_imageWidth, x, y, pixelColor);
    //                 }
    //                 renderedLines++;
    //             }
    //             }, i * m_imageHeight / nThreads, (i + 1) == nThreads ? m_imageHeight : (i + 1) * m_imageHeight / nThreads);
    //     }

    //     while (1)
    //     {
    //         std::clog << "\rLines rendered: " << renderedLines << "/" << m_imageHeight << std::flush;
    //         if (renderedLines == m_imageHeight)
    //             break;
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //     }
    // }

    _mkdir("output");
    stbi_write_png(outputFile.c_str(), m_imageWidth, m_imageHeight, 3, image.data(), m_imageWidth * 3);

    std::clog << "\rDone.                                        \n";
}

void RayTracer::setSettings(RayTracerSettings settings)
{
    m_aspectRatio = settings.AspectRatio;
    m_imageWidth = settings.Width;
    m_camera = settings.Camera;
    m_samplesPerPixel = settings.SamplesPerPixel;
    m_maxDepth = settings.MaxDepth;
    initialize();
}

void RayTracer::initialize()
{
    m_imageHeight = int(m_imageWidth / m_aspectRatio);
    m_imageHeight = (m_imageHeight < 1) ? 1 : m_imageHeight;

    float theta = degreesToRadians(m_camera.VFOV);
    float h = std::tan(theta / 2);
    float viewportHeight = 2 * h * m_focusDistance;
    float viewportWidth = viewportHeight * (float(m_imageWidth) / m_imageHeight);

    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    m_w = unitVector(m_camera.LookFrom - m_camera.LookAt);
    m_u = unitVector(cross(m_camera.UpVec, m_w));
    m_v = cross(m_w, m_u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    Vec3 viewportU = viewportWidth * m_u;
    Vec3 viewportV = viewportHeight * -m_v;

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    m_pixelDeltaU = viewportU / m_imageWidth;
    m_pixelDeltaV = viewportV / m_imageHeight;

    // Calculate the location of the upper left pixel.
    Vec3 viewportUpperLeft = m_camera.LookFrom - m_focusDistance * m_w - viewportU / 2 - viewportV / 2;
    m_pixel00Loc = viewportUpperLeft + 0.5 * (m_pixelDeltaU + m_pixelDeltaV);

    // Calculate the camera defocus disk basis vectors.
    float defocusRadius = m_focusDistance * std::tan(degreesToRadians(m_defocusAngle / 2));
    m_defocusDiskU = m_u * defocusRadius;
    m_defocusDiskV = m_v * defocusRadius;
}

Ray RayTracer::getRay(int x, int y) const
{
    // Construct a camera ray originating from the defocus disk and directed at a randomly
    // sampled point around the pixel location x, y.

    Vec3 offset = sampleSquare();
    Point3 pixelSample = m_pixel00Loc
        + ((x + offset.x()) * m_pixelDeltaU)
        + ((y + offset.y()) * m_pixelDeltaV);

    Point3 rayOrigin = (m_defocusAngle <= 0) ? m_camera.LookFrom : defocusDistSample();
    Vec3 rayDirection = pixelSample - rayOrigin;
    float rayTime = randomFloat();

    return Ray(rayOrigin, rayDirection, rayTime);
}

Vec3 RayTracer::sampleSquare() const
{
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return Vec3(randomFloat() - 0.5, randomFloat() - 0.5, 0);
}

Vec3 RayTracer::defocusDistSample() const
{
    // Returns a random point in the camera defocus disk.
    Vec3 p = randomInUnitDisk();
    return m_camera.LookFrom + (p[0] * m_defocusDiskU) + (p[1] * m_defocusDiskV);
}

Color RayTracer::rayColor(const Ray& r, Hittable& objects, int depth) const
{
    if (depth <= 0)
        return Color(0);

    HitRecord rec;
    if (objects.hit(r, rec, Interval(0.001f, INF)))
    {
        Ray scattered;
        Color attenuation;
        if (rec.Mat->scatter(r, rec, attenuation, scattered))
            return attenuation;// * rayColor(scattered, objects, depth - 1);
        return Color(0);
    }

    Vec3 unitDirection = unitVector(r.direction());
    float a = 0.5 * (unitDirection.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void RayTracer::setPixel(std::vector<uint8_t>& image, int imageWidth, int x, int y, const Color& pixelColor) const
{
    float r = pixelColor.x();
    float g = pixelColor.y();
    float b = pixelColor.z();

    // Apply a linear to gamma transform for gamma 2
    r = linearToGamma(r);
    g = linearToGamma(g);
    b = linearToGamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000f, 0.999f);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    // std::scoped_lock<std::mutex> lock(s_imageMutex);
    image[3 * (y * imageWidth + x)] = rbyte;
    image[3 * (y * imageWidth + x) + 1] = gbyte;
    image[3 * (y * imageWidth + x) + 2] = bbyte;
}

void RayTracer::addToPixel(std::vector<float>& imageIntensities, int imageWidth, int x, int y, const Color& pixelColor) const
{
    float r = pixelColor.x();
    float g = pixelColor.y();
    float b = pixelColor.z();

    // Write out the pixel color components.
    std::scoped_lock<std::mutex> lock(s_imageMutex);
    imageIntensities[3 * (y * imageWidth + x)] += r;
    imageIntensities[3 * (y * imageWidth + x) + 1] += g;
    imageIntensities[3 * (y * imageWidth + x) + 2] += b;
}