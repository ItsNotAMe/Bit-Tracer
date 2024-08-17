#include "Common.h"

#include "RayTracer.h"
#include "hittable/Sphere.h"
#include "hittable/Quad.h"
#include "texture/CheckerTexture.h"
#include "texture/ImageTexture.h"
#include "texture/NoiseTexture.h"
#include "materials/Material.h"
#include "materials/Lambertian.h"
#include "materials/Metal.h"
#include "materials/Dielectric.h"
#include "materials/DiffuseLight.h"

void example1()
{
    RayTracerSettings settings;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.LookFrom = Point3(-2, 2, 1);
    settings.Camera.LookAt = Point3(0, 0, -1);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Camera.VFOV = 20;
    settings.Background = Color(0.70, 0.80, 1.00);
    settings.DefocusAngle = 10.0;
    settings.FocusDistance = 3.4;
    RayTracer tracer(settings);

    auto materialGround = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto materialCenter = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto materialLeft = std::make_shared<Dielectric>(1.5);
    auto materialBubble = std::make_shared<Dielectric>(1.00 / 1.50);
    auto materialRight = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    tracer.addObject(std::make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, materialGround));
    tracer.addObject(std::make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, materialCenter));
    tracer.addObject(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, materialLeft));
    tracer.addObject(std::make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.4, materialBubble));
    tracer.addObject(std::make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, materialRight));

    tracer.render("output/example1.png");
}

void example2()
{
    RayTracerSettings settings;
    settings.Background = Color(0.70, 0.80, 1.00);
    RayTracer tracer(settings);

    float R = std::cos(PI / 4);

    auto material_left = std::make_shared<Lambertian>(Color(0, 0, 1));
    auto material_right = std::make_shared<Lambertian>(Color(1, 0, 0));

    tracer.addObject(std::make_shared<Sphere>(Point3(-R, 0, -1), R, material_left));
    tracer.addObject(std::make_shared<Sphere>(Point3(R, 0, -1), R, material_right));

    tracer.render("output/example2.png");
}

void finalScene()
{
    RayTracerSettings settings;
    settings.Width = 1200;
    settings.SamplesPerPixel = 500;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(13, 2, 3);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
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
                    tracer.addObject(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
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

    tracer.render("output/final_scene.png");
}

void bouncingBalls()
{
    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(13, 2, 3);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
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
}

void checkeredSpheres()
{
    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(13, 2, 3);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
    RayTracer tracer(settings);

    auto checker = std::make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));

    tracer.addObject(std::make_shared<Sphere>(Point3(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
    tracer.addObject(std::make_shared<Sphere>(Point3(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));

    tracer.render("output/checkered_balls.png");
}

void earth()
{
    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(0, 0, 12);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
    RayTracer tracer(settings);

    auto earthTexture = std::make_shared<ImageTexture>("earthmap.jpg");
    auto earthSurface = std::make_shared<Lambertian>(earthTexture);
    auto globe = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface);
    tracer.addObject(globe);

    tracer.render("output/earth.png");
}

void imageSphere(const std::string& imagePath)
{
    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(12, 0, 0);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
    RayTracer tracer(settings);

    auto earthTexture = std::make_shared<ImageTexture>(imagePath);
    auto earthSurface = std::make_shared<Lambertian>(earthTexture);
    auto globe = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earthSurface);
    tracer.addObject(globe);

    tracer.render("output/image_sphere.png");
}

void perlinSpheres()
{
    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(13, 2, 3);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
    RayTracer tracer(settings);

    auto pertext = std::make_shared<NoiseTexture>(4);
    tracer.addObject(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    tracer.addObject(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    tracer.render("output/perlin_spheres.png");
}

void quads()
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 80;
    settings.Camera.LookFrom = Point3(0, 0, 9);
    settings.Camera.LookAt = Point3(0, 0, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    settings.Background = Color(0.70, 0.80, 1.00);
    RayTracer tracer(settings);

    // Materials
    auto left_red = std::make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto back_green = std::make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto right_blue = std::make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lower_teal = std::make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    tracer.addObject(std::make_shared<Quad>(Point3(-3, -2, 5), Vec3(0, 0, -4), Vec3(0, 4, 0), left_red));
    tracer.addObject(std::make_shared<Quad>(Point3(-2, -2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
    tracer.addObject(std::make_shared<Quad>(Point3(3, -2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
    tracer.addObject(std::make_shared<Quad>(Point3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
    tracer.addObject(std::make_shared<Quad>(Point3(-2, -3, 5), Vec3(4, 0, 0), Vec3(0, 0, -4), lower_teal));

    tracer.render("output/quads.png");
}

void simpleLight()
{
    RayTracerSettings settings;
    settings.Width = 400;
    settings.SamplesPerPixel = 100;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 20;
    settings.Camera.LookFrom = Point3(26, 3, 6);
    settings.Camera.LookAt = Point3(0, 2, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    auto pertext = std::make_shared<NoiseTexture>(4);
    tracer.addObject(std::make_shared<Sphere>(Point3(0,-1000,0), 1000, std::make_shared<Lambertian>(pertext)));
    tracer.addObject(std::make_shared<Sphere>(Point3(0,2,0), 2, std::make_shared<Lambertian>(pertext)));

    auto difflight = std::make_shared<DiffuseLight>(Color(4,4,4));
    tracer.addObject(std::make_shared<Sphere>(Point3(0,7,0), 2, difflight));
    tracer.addObject(std::make_shared<Quad>(Point3(3,1,-2), Vec3(2,0,0), Vec3(0,2,0), difflight));

    tracer.render("output/simple_light.png");
}

void cornellBox()
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = 600;
    settings.SamplesPerPixel = 200;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 40;
    settings.Camera.LookFrom = Point3(278, 278, -800);
    settings.Camera.LookAt = Point3(278, 278, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    auto red   = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    tracer.addObject(std::make_shared<Quad>(Point3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), green));
    tracer.addObject(std::make_shared<Quad>(Point3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), red));
    tracer.addObject(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130,0,0), Vec3(0,0,-105), light));
    tracer.addObject(std::make_shared<Quad>(Point3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white));

    // tracer.addObject(box(Point3(130, 0, 65), Point3(295, 165, 230), white));
    // tracer.addObject(box(Point3(265, 0, 295), Point3(430, 330, 460), white));

    tracer.render("output/cornell_box.png");
}
