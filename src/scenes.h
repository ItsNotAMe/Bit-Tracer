#include "Common.h"

#include "RayTracer.h"
#include "hittable/Sphere.h"
#include "hittable/Quad.h"
#include "hittable/ConstantMedium.h"
#include "hittable/BVH.h"
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
    tracer.addObject(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    tracer.addObject(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    auto difflight = std::make_shared<DiffuseLight>(Color(4, 4, 4));
    tracer.addObject(std::make_shared<Sphere>(Point3(0, 7, 0), 2, difflight));
    tracer.addObject(std::make_shared<Quad>(Point3(3, 1, -2), Vec3(2, 0, 0), Vec3(0, 2, 0), difflight));

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

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    tracer.addObject(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    tracer.addObject(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    std::shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));
    tracer.addObject(box1);

    std::shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));
    tracer.addObject(box2);

    tracer.render("output/cornell_box.png");
}

void cornellSmoke()
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

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));

    tracer.addObject(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    tracer.addObject(std::make_shared<Quad>(Point3(113, 554, 127), Vec3(330, 0, 0), Vec3(0, 0, 305), light));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    std::shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));

    std::shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));

    tracer.addObject(std::make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
    tracer.addObject(std::make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

    tracer.render("output/cornell_smoke.png");
}

void finalScene2(int imageWidth, int samplesPerPixel, int maxDepth)
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = imageWidth;
    settings.SamplesPerPixel = samplesPerPixel;
    settings.MaxDepth = maxDepth;
    settings.Camera.VFOV = 40;
    settings.Camera.LookFrom = Point3(478, 278, -600);
    settings.Camera.LookAt = Point3(278, 278, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = randomFloat(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    tracer.addObject(std::make_shared<BVHNode>(boxes1));

    auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));
    tracer.addObject(std::make_shared<Quad>(Point3(123, 554, 147), Vec3(300, 0, 0), Vec3(0, 0, 265), light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto sphere_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    tracer.addObject(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

    tracer.addObject(std::make_shared<Sphere>(Point3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    tracer.addObject(std::make_shared<Sphere>(
        Point3(0, 150, 145), 50, std::make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = std::make_shared<Sphere>(Point3(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
    tracer.addObject(boundary);
    tracer.addObject(std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(Point3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
    tracer.addObject(std::make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
    tracer.addObject(std::make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
    auto pertext = std::make_shared<NoiseTexture>(0.2);
    tracer.addObject(std::make_shared<Sphere>(Point3(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++)
    {
        boxes2.add(std::make_shared<Sphere>(Point3::random(0, 165), 10, white));
    }

    tracer.addObject(std::make_shared<Translate>(
        std::make_shared<RotateY>(
            std::make_shared<BVHNode>(boxes2), 15),
        Vec3(-100, 270, 395)
    )
    );

    tracer.render("output/final_scene2_" + std::to_string(imageWidth) + "_" + std::to_string(samplesPerPixel) + "_" + std::to_string(maxDepth) + ".png");
}

void cornellBoxBook3()
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = 600;
    settings.SamplesPerPixel = 1000;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 40;
    settings.Camera.LookFrom = Point3(278, 278, -800);
    settings.Camera.LookAt = Point3(278, 278, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    tracer.addObject(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    tracer.addObject(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    std::shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));
    tracer.addObject(box1);

    std::shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));
    tracer.addObject(box2);

    tracer.addLight(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), std::shared_ptr<Material>()));

    tracer.render("output/cornell_box_book_3.png");
}

void cornellBoxAlluminium()
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = 600;
    settings.SamplesPerPixel = 1000;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 40;
    settings.Camera.LookFrom = Point3(278, 278, -800);
    settings.Camera.LookAt = Point3(278, 278, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    tracer.addObject(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    tracer.addObject(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    std::shared_ptr<Material> aluminum = std::make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
    std::shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), aluminum);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));
    tracer.addObject(box1);

    std::shared_ptr<Hittable> box2 = box(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));
    tracer.addObject(box2);

    tracer.addLight(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), std::shared_ptr<Material>()));

    tracer.render("output/cornell_box_alluminium.png");
}

void cornellBoxGlassSphere()
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = 600;
    settings.SamplesPerPixel = 1000;
    settings.MaxDepth = 50;
    settings.Camera.VFOV = 40;
    settings.Camera.LookFrom = Point3(278, 278, -800);
    settings.Camera.LookAt = Point3(278, 278, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    // Cornell box sides
    tracer.addObject(std::make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 0, 555), Vec3(0, 555, 0), green));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(0, 0, -555), Vec3(0, 555, 0), red));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, -555), white));
    tracer.addObject(std::make_shared<Quad>(Point3(555, 0, 555), Vec3(-555, 0, 0), Vec3(0, 555, 0), white));

    // Light
    tracer.addObject(std::make_shared<Quad>(Point3(213, 554, 227), Vec3(130, 0, 0), Vec3(0, 0, 105), light));

    // Box
    std::shared_ptr<Hittable> box1 = box(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));
    tracer.addObject(box1);

    // Glass Sphere
    auto glass = std::make_shared<Dielectric>(1.5);
    tracer.addObject(std::make_shared<Sphere>(Point3(190, 90, 190), 90, glass));

    // Light Sources
    auto emptyMaterial = std::shared_ptr<Material>();
    tracer.addLight(std::make_shared<Quad>(Point3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), emptyMaterial));
    tracer.addLight(std::make_shared<Sphere>(Point3(190, 90, 190), 90, emptyMaterial));

    tracer.render("output/cornell_box_glass_sphere.png");
}

void finalScene2WithImportance(int imageWidth, int samplesPerPixel, int maxDepth)
{
    RayTracerSettings settings;
    settings.AspectRatio = 1.0f;
    settings.Width = imageWidth;
    settings.SamplesPerPixel = samplesPerPixel;
    settings.MaxDepth = maxDepth;
    settings.Camera.VFOV = 40;
    settings.Camera.LookFrom = Point3(478, 278, -600);
    settings.Camera.LookAt = Point3(278, 278, 0);
    settings.Camera.UpVec = Vec3(0, 1, 0);
    RayTracer tracer(settings);

    HittableList boxes1;
    auto ground = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = randomFloat(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    tracer.addObject(std::make_shared<BVHNode>(boxes1));

    auto light = std::make_shared<DiffuseLight>(Color(7, 7, 7));
    tracer.addObject(std::make_shared<Quad>(Point3(123, 554, 147), Vec3(300, 0, 0), Vec3(0, 0, 265), light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto sphere_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    tracer.addObject(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

    tracer.addObject(std::make_shared<Sphere>(Point3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    tracer.addObject(std::make_shared<Sphere>(
        Point3(0, 150, 145), 50, std::make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = std::make_shared<Sphere>(Point3(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
    tracer.addObject(boundary);
    tracer.addObject(std::make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(Point3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
    tracer.addObject(std::make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
    tracer.addObject(std::make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
    auto pertext = std::make_shared<NoiseTexture>(0.2);
    tracer.addObject(std::make_shared<Sphere>(Point3(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++)
    {
        boxes2.add(std::make_shared<Sphere>(Point3::random(0, 165), 10, white));
    }

    tracer.addObject(std::make_shared<Translate>(
        std::make_shared<RotateY>(
            std::make_shared<BVHNode>(boxes2), 15),
        Vec3(-100, 270, 395)
    )
    );

    // Light Sources
    auto emptyMaterial = std::shared_ptr<Material>();
    tracer.addLight(std::make_shared<Quad>(Point3(423, 554, 412), Vec3(-300, 0, 0), Vec3(0, 0, -265), emptyMaterial));

    tracer.render("output/final_scene2_Importance_" + std::to_string(imageWidth) + "_" + std::to_string(samplesPerPixel) + "_" + std::to_string(maxDepth) + ".png");
}
