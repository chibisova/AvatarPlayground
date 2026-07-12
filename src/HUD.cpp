#include "HUD.h"
#include "ProcessorManager.h"


void drawHUD(cv::Mat& frame, double fps, const char* currentMode)
{
    // Get text using custom FPS calculation and camera properties
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
    std::string resText = "Resolution: " + std::to_string(frame.cols) + " x " + std::to_string(frame.rows);

    // Add Current mode to the HUD
    std::string modeText = "Mode: ";
    modeText += currentMode;

    // Draw the FPS and resolution on the frame
    cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
    cv::putText(frame, resText, cv::Point(10,60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
    cv::putText(frame, modeText, cv::Point(10,90), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);

}
