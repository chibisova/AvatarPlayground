#include "HUD.h"

void drawHUD(cv::Mat& frame, double fps)
{
    // Get text using custom FPS calculation and camera properties
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
    std::string resText = "Resolution: " + std::to_string(frame.cols) + " x " + std::to_string(frame.rows);
    
    // Draw the FPS and resolution on the frame
    cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
    cv::putText(frame, resText, cv::Point(10,60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
}
