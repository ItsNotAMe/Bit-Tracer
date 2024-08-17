#include "Common.h"

#include <chrono>
#include <iomanip>

#include "Scenes.h"

int main()
{
    auto begin = std::chrono::steady_clock::now();

    switch (7)
    {
        case 1: example1();  break;
        case 2: example2(); break;
        case 3: finalScene();  break;
        case 4: bouncingBalls();  break;
        case 5: checkeredSpheres(); break;
        case 6: earth(); break;
        case 7: imageSphere("thinking.png"); break;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2) << "Time to render: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0f << "s" << std::endl;

    return 0;
}