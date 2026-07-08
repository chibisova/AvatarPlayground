#pragma once

#include <opencv2/opencv.hpp>
#include <string>

// Function to handle keyboard input
bool handleKeyboard(
    int key,
    const cv::Mat& frame,
    const std::string& screenshotsDir,
    int& numOfScr);
    
// Function to calculate FPS
void saveScreenshot(
    const cv::Mat& frame,
    const std::string& screenshotsDir,
    int& numOfScr);
