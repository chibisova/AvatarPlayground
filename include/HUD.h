#pragma once

#include <opencv2/opencv.hpp>
#include "ImageProcessor.h"

void drawHUD(cv::Mat& frame,
             double fps,
             ProcessingMode currentMode);
