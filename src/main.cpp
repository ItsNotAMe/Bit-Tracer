#include "Common.h"

#include <chrono>
#include <iomanip>

#include "RayTracer.h"
#include "hittable/Sphere.h"
#include "materials/Material.h"
#include "materials/Lambertian.h"
#include "materials/Metal.h"
#include "materials/Dielectric.h"

int main()
{
    auto begin = std::chrono::steady_clock::now();

    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(13, 2, 3);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.DefocusAngle = 0.6f;
    settings.FocusDistance = 10.0f;
    RayTracer tracer(settings);

    auto groundMaterial = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    tracer.addObject(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto chooseMat = randomFloat();
            Point3 center(a + 0.9 * randomFloat(), 0.2, b + 0.9 * randomFloat());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9)
            {
                std::shared_ptr<Material> sphereMaterial;

                if (chooseMat < 0.8)
                {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    auto center2 = center + Vec3(0, randomFloat(0, .5f), 0);
                    tracer.addObject(std::make_shared<Sphere>(center, center2, 0.2, sphereMaterial));
                }
                else if (chooseMat < 0.95)
                {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = randomFloat(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    tracer.addObject(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
                else
                {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    tracer.addObject(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    tracer.addObject(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    tracer.addObject(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    tracer.addObject(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    tracer.render("output/bouncing_balls.png");

    auto end = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2) << "Time to render: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0f << "s" << std::endl;

    return 0;
}