#pragma once

#include <chrono>

double calculateFPS(
    std::chrono::steady_clock::time_point& previousTime);
