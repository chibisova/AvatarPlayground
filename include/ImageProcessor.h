#pragma once

#include <opencv2/opencv.hpp>
#include <string>

// Enum to define different processing modes
enum class ProcessingMode
{
    Original,
    Gray,
    Blur,
    Canny,
    Threshold,
    ShiTomasiCorners,
    HarrisCorners
};

// Function to process the frame based on the selected mode
cv::Mat processFrame(
    const cv::Mat& frame,
    ProcessingMode mode);

// Function to get the string representation of the current mode
std::string processingModeToString(ProcessingMode mode);
