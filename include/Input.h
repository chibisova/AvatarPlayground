#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include "ImageProcessor.h"

// Function to handle keyboard input
bool handleKeyboard(
    int key,
    const cv::Mat& frame,
    const std::string& screenshotsDir,
    int& numOfScr,
    ProcessingMode& mode);

// Function to calculate FPS
void saveScreenshot(
    const cv::Mat& frame,
    const std::string& screenshotsDir,
    int& numOfScr);
