#include "Timing.h"

double calculateFPS(std::chrono::steady_clock::time_point& previousTime)
{
    auto currentTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
   
    previousTime = currentTime;
    return 1.0 / deltaTime;;
}
