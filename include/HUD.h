#pragma once

#include <opencv2/opencv.hpp>

void drawHUD(cv::Mat& frame,
             double fps,
             const char* currentMode);
