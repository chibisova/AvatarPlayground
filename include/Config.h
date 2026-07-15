#pragma once

#include <string>

namespace Config
{
    constexpr int ESC_KEY = 27;
    inline const std::string SCREENSHOT_DIR = "./screenshots";
    constexpr int HARRIS_THRESHOLD = 100;
    constexpr int MAX_TRACKED_POINTS = 100;
    constexpr int MIN_TRACKED_POINTS = 40;
    constexpr float LOWE_RATIO = 0.75f;
    inline cv::Mat CAMERA_INTRINSICS;
}
