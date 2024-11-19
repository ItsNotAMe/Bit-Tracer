#include "Common.h"

#include <chrono>
#include <iomanip>

#include "Scenes.h"

int main(int argc, char** argv)
{
    auto begin = std::chrono::steady_clock::now();

    int scene = 17;
    int samples = 50;
    if (argc >= 2)
        scene = std::stoi(argv[1]);
    if (argc >= 3)
        samples = std::stoi(argv[2]);
    
    switch (scene)
    {
        case 1: example1(samples);  break;
        case 2: example2(samples); break;
        case 3: finalScene(samples);  break;
        case 4: bouncingBalls(samples);  break;
        case 5: checkeredSpheres(samples); break;
        case 6: earth(samples); break;
        case 7: imageSphere(samples, "thinking.png"); break;
        case 8: perlinSpheres(samples); break;
        case 9: quads(samples); break;
        case 10: simpleLight(samples); break;
        case 11: cornellBox(samples); break;
        case 12: cornellSmoke(samples); break;
        case 13: finalScene2(samples, 800, 40); break;
        case 14: finalScene2(samples, 400, 4); break;
        case 15: cornellBoxBook3(samples); break;
        case 16: cornellBoxAlluminium(samples); break;
        case 17: cornellBoxGlassSphere(samples); break;
        case 18: finalScene2WithImportance(samples, 800, 40); break;
        case 19: finalScene2WithImportance(samples, 400, 4); break;
        case 20: triangles(samples); break;
        case 21: bunny(samples); break;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "Time to render: " << formatDuration(std::chrono::duration_cast<std::chrono::microseconds>(end - begin)) << std::endl;

    return 0;
}