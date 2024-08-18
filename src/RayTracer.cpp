#include "RayTracer.h"

#include <direct.h>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "core/ThreadPool.h"
#include "materials/Material.h"
#include "hittable/BVH.h"

std::mutex RayTracer::s_imageMutex;

std::string formatDuration(std::chrono::microseconds microseconds)
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

void eraseLines(int count, std::stringstream& ss)
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

RayTracer::RayTracer(RayTracerSettings settings)
    : m_aspectRatio(settings.AspectRatio), m_imageWidth(settings.Width), m_camera(settings.Camera),
    m_samplesPerPixel(settings.SamplesPerPixel), m_maxDepth(settings.MaxDepth), m_background(settings.Background),
    m_defocusAngle(settings.DefocusAngle), m_focusDistance(settings.FocusDistance)
{
    initialize();
}

void RayTracer::render(const std::string& outputFile) const
{
    auto begin = std::chrono::steady_clock::now();

    HittableList BVHObjects = HittableList(std::make_shared<BVHNode>(m_objects));

    // std::vector<std::atomic<float>> imageIntensities(m_imageWidth * m_imageHeight * 3);
    // ThreadPool pool(std::thread::hardware_concurrency());
    // for (int sample = 0; sample < m_samplesPerPixel; sample++)
    // {
    //     pool.addTask([&]() {
    //         for (int y = 0; y < m_imageHeight; y++)
    //         {
    //             for (int x = 0; x < m_imageWidth; x++)
    //             {
    //                 Ray ray = getRay(x, y);
    //                 Color pixelColor = rayColor(ray, BVHObjects, m_maxDepth);
    //                 addToPixel(imageIntensities, m_imageWidth, x, y, pixelColor);
    //             }
    //         }
    //         });
    // }

    // while (1)
    // {

    //     auto end = std::chrono::steady_clock::now();
    //     float elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0f;
    //     int remaining = pool.getQueueSize() + pool.getCountWorking();
    //     std::clog << "\rSamples remaining: " << remaining << "    Currently working on: " << pool.getCountWorking()
    //                 << std::fixed << std::setprecision(2) << "    Elapsed time: " << elapsedTime << "s  " << std::flush;
    //     if (remaining == 0)
    //         break;
    //     pool.waitForUpdate();
    // }

    // std::vector<uint8_t> image(m_imageWidth * m_imageHeight * 3);
    // for (int y = 0; y < m_imageHeight; y++)
    // {
    //     for (int x = 0; x < m_imageWidth; x++)
    //     {
    //         setPixel(image, m_imageWidth, x, y,
    //             Color(imageIntensities[3 * (y * m_imageWidth + x)] / m_samplesPerPixel,
    //                 imageIntensities[3 * (y * m_imageWidth + x) + 1] / m_samplesPerPixel,
    //                 imageIntensities[3 * (y * m_imageWidth + x) + 2] / m_samplesPerPixel));
    //     }
    // }

    std::atomic<int> renderedLines = 0;
    std::atomic<int> renderedPixels = 0;
    std::vector<uint8_t> image(m_imageWidth * m_imageHeight * 3);
    {
        int nThreads = std::thread::hardware_concurrency();
        ThreadPool pool(nThreads);
        for (int y = m_imageHeight - 1; y >= 0; y--)
        {
            pool.addTask([&, y]() {
                for (int x = 0; x < m_imageWidth; x++)
                {
                    Color pixelColor = { 0 };
                    for (int sample = 0; sample < m_samplesPerPixel; sample++)
                    {
                        Ray ray = getRay(x, y);

                        pixelColor += rayColor(ray, BVHObjects, m_maxDepth);
                    }
                    pixelColor /= m_samplesPerPixel;
                    setPixel(image, m_imageWidth, x, y, pixelColor);
                    renderedPixels++;
                }
                renderedLines++;
                });
        }

        int prevRenderedLines = 0;
        int prevRenderedPixels = 0;
        int avgTimeLines = 0;
        int avgTimePixels = 0;
        std::clog << "\n\n\n\n\n\n";
        while (1)
        {
            auto end = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            if (prevRenderedLines != renderedLines)
            {
                prevRenderedLines = renderedLines;
                avgTimeLines = elapsedTime / float(renderedLines);
            }
            if (prevRenderedPixels != renderedPixels)
            {
                prevRenderedPixels = renderedPixels;
                avgTimePixels = elapsedTime / float(renderedPixels);
            }
            int linesEstimate = m_imageHeight * avgTimeLines - elapsedTime;
            if (avgTimeLines == 0)
                linesEstimate = 0;
            int pixelsEstimate = m_imageWidth * m_imageHeight * avgTimePixels - elapsedTime;
            if (avgTimePixels == 0)
                pixelsEstimate = 0;
            std::stringstream ss;
            eraseLines(7, ss);
            ss << "Lines rendered: " << renderedLines << "/" << m_imageHeight << "\n"
                << "Pixels rendered: " << renderedPixels << "/" << m_imageWidth * m_imageHeight << "\n"
                << "Elapsed time: " << formatDuration(std::chrono::microseconds(elapsedTime)) << "\n"
                << "Average time per line: " << formatDuration(std::chrono::microseconds(avgTimeLines)) << "\n"
                << "Average time per pixel: " << formatDuration(std::chrono::microseconds(avgTimePixels)) << "\n"
                << "Estimate time left by lines: " << formatDuration(std::chrono::microseconds(linesEstimate)) << "\n"
                << "Estimate time left by pixels: " << formatDuration(std::chrono::microseconds(pixelsEstimate));
            std::clog << ss.str() << std::flush;
            if (renderedLines == m_imageHeight)
                break;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
    }


    _mkdir("output");
    stbi_write_png(outputFile.c_str(), m_imageWidth, m_imageHeight, 3, image.data(), m_imageWidth * 3);

    std::clog << "\nDone.\n";
}

void RayTracer::setSettings(RayTracerSettings settings)
{
    m_aspectRatio = settings.AspectRatio;
    m_imageWidth = settings.Width;
    m_camera = settings.Camera;
    m_samplesPerPixel = settings.SamplesPerPixel;
    m_maxDepth = settings.MaxDepth;
    m_background = settings.Background;
    m_defocusAngle = settings.DefocusAngle;
    m_focusDistance = settings.FocusDistance;
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

    // If the ray hits nothing, return the background color.
    if (!objects.hit(r, rec, Interval(0.001, INF)))
        return m_background;

    Ray scattered;
    Color attenuation;
    Color emissionColor = rec.Mat->emitted(rec.u, rec.v, rec.Point);

    if (!rec.Mat->scatter(r, rec, attenuation, scattered))
        return emissionColor;

    Color scatterColor = attenuation * rayColor(scattered, objects, depth - 1);

    return emissionColor + scatterColor;
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

void RayTracer::addToPixel(std::vector<std::atomic<float>>& imageIntensities, int imageWidth, int x, int y, const Color& pixelColor) const
{
    float r = pixelColor.x();
    float g = pixelColor.y();
    float b = pixelColor.z();

    // Write out the pixel color components.
    // std::scoped_lock<std::mutex> lock(s_imageMutex);
    imageIntensities[3 * (y * imageWidth + x)].fetch_add(r);
    imageIntensities[3 * (y * imageWidth + x) + 1] += g;
    imageIntensities[3 * (y * imageWidth + x) + 2] += b;
}