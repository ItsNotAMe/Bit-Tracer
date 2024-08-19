#include "RayTracer.h"

#include <direct.h>
#include <vector>
#include <functional>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "core/ThreadPool.h"
#include "materials/Material.h"
#include "hittable/BVH.h"
#include "pdf/HittablePDF.h"
#include "pdf/CosinePDF.h"
#include "pdf/MixturePDF.h"

std::mutex RayTracer::s_imageMutex;

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

        int perm[m_imageHeight];
        for (int i = 0; i < m_imageHeight; i++)
            perm[i] = i;

        for (int i = m_imageHeight - 1; i > 0; i--)
        {
            int target = randomInt(0, i);
            int tmp = perm[i];
            perm[i] = perm[target];
            perm[target] = tmp;
        }

        for (int y : perm)
        {
            pool.addTask([&, y]() {
                for (int x = 0; x < m_imageWidth; x++)
                {
                    Color pixelColor = { 0 };
                    for (int sy = 0; sy < m_sqrtSPP; sy++)
                    {
                        for (int sx = 0; sx < m_sqrtSPP; sx++)
                        {
                            Ray ray = getRay(x, y, sx, sy);
                            pixelColor += rayColor(ray, BVHObjects, m_maxDepth);
                        }
                    }
                    setPixel(image, m_imageWidth, x, y, m_pixelSamplesScale * pixelColor);
                    renderedPixels++;
                }
                renderedLines++;
                });
        }

        int prevRenderedLines = 0;
        int prevRenderedPixels = 0;
        uint64_t avgTimeLines = 0;
        uint64_t avgTimePixels = 0;
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
            uint64_t linesEstimate = m_imageHeight * avgTimeLines - elapsedTime;
            if (elapsedTime > m_imageHeight * avgTimeLines)
                linesEstimate = 0;
            uint64_t pixelsEstimate = m_imageWidth * m_imageHeight * avgTimePixels - elapsedTime;
            if (elapsedTime > m_imageWidth * m_imageHeight * avgTimePixels)
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

    m_sqrtSPP = int(std::sqrt(m_samplesPerPixel));
    m_pixelSamplesScale = 1.0 / (m_sqrtSPP * m_sqrtSPP);
    m_recipSqrtSPP = 1.0 / m_sqrtSPP;
}

Ray RayTracer::getRay(int x, int y, int sx, int sy) const
{
    // Construct a camera ray originating from the defocus disk and directed at a randomly
    // sampled point around the pixel location x, y for stratified sample square sx, sy.

    Vec3 offset = sampleSquareStratified(sx, sy);
    Point3 pixelSample = m_pixel00Loc
        + ((x + offset.x()) * m_pixelDeltaU)
        + ((y + offset.y()) * m_pixelDeltaV);

    Point3 rayOrigin = (m_defocusAngle <= 0) ? m_camera.LookFrom : defocusDistSample();
    Vec3 rayDirection = pixelSample - rayOrigin;
    float rayTime = randomFloat();

    return Ray(rayOrigin, rayDirection, rayTime);
}

Vec3 RayTracer::sampleSquareStratified(int sx, int sy) const
{
    // Returns the vector to a random point in the square sub-pixel specified by grid
    // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

    float px = ((sx + randomFloat()) * m_recipSqrtSPP) - 0.5;
    float py = ((sy + randomFloat()) * m_recipSqrtSPP) - 0.5;

    return Vec3(px, py, 0);
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

    ScatterRecord srec;
    Color emissionColor = rec.Mat->emitted(r, rec, rec.u, rec.v, rec.Point);

    if (!rec.Mat->scatter(r, rec, srec))
        return emissionColor;

    if (srec.SkipPDF)
        return srec.Attenuation * rayColor(srec.SkipPDFRay, objects, depth - 1);

    std::shared_ptr<PDF> p = srec.PDF;
    if (m_lights.size() > 0)
    {
        auto pLight = std::make_shared<HittablePDF>(m_lights, rec.Point);
        p = std::make_shared<MixturePDF>(pLight, srec.PDF);
    }

    Ray scattered = Ray(rec.Point, p->generate(), r.time());
    float pdfValue = p->value(scattered.direction());

    float scatteringPDF = rec.Mat->scatteringPDF(r, rec, scattered);

    Color scatterColor = (srec.Attenuation * scatteringPDF * rayColor(scattered, objects, depth-1)) / pdfValue;

    return emissionColor + scatterColor;
}

void RayTracer::setPixel(std::vector<uint8_t>& image, int imageWidth, int x, int y, const Color& pixelColor) const
{
    float r = pixelColor.x();
    float g = pixelColor.y();
    float b = pixelColor.z();

    // Replace NaN components with zero.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

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

// void RayTracer::addToPixel(std::vector<float>& imageIntensities, int imageWidth, int x, int y, const Color& pixelColor) const
// {
//     float r = pixelColor.x();
//     float g = pixelColor.y();
//     float b = pixelColor.z();

//     // Write out the pixel color components.
//     std::scoped_lock<std::mutex> lock(s_imageMutex);
//     imageIntensities[3 * (y * imageWidth + x)] += r;
//     imageIntensities[3 * (y * imageWidth + x) + 1] += g;
//     imageIntensities[3 * (y * imageWidth + x) + 2] += b;
// }