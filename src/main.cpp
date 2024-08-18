#include "Common.h"

#include <chrono>
#include <iomanip>

#include "Scenes.h"

int main()
{
    auto begin = std::chrono::steady_clock::now();

    switch (14)
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
        case 13: finalScene2(800, 10000, 40); break;
        default: finalScene2(400, 250, 4); break;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2) << "Time to render: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0f << "s" << std::endl;

    return 0;
}