#include "Common.h"

#include <chrono>
#include <iomanip>

#include "Scenes.h"

int main(int argc, char** argv)
{
    auto begin = std::chrono::steady_clock::now();

    int scene = 17;
    if (argc == 2)
        scene = std::stoi(argv[1]);
    
    switch (scene)
    {
        case 1: example1();  break;
        case 2: example2(); break;
        case 3: finalScene();  break;
        case 4: bouncingBalls();  break;
        case 5: checkeredSpheres(); break;
        case 6: earth(); break;
        case 7: imageSphere("thinking.png"); break;
        case 8: perlinSpheres(); break;
        case 9: quads(); break;
        case 10: simpleLight(); break;
        case 11: cornellBox(); break;
        case 12: cornellSmoke(); break;
        case 13: finalScene2(800, 1500, 40); break;
        case 14: finalScene2(400, 250, 4); break;
        case 15: cornellBoxBook3(); break;
        case 16: cornellBoxAlluminium(); break;
        case 17: cornellBoxGlassSphere(); break;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "Time to render: " << formatDuration(std::chrono::duration_cast<std::chrono::microseconds>(end - begin)) << "s" << std::endl;

    return 0;
}