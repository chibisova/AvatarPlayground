#include "processors/CannyProcessor.h"
#include <opencv2/imgproc.hpp>


namespace vision
{
    cv::Mat CannyProcessor::process(const cv::Mat& frame)
    {
        cv::Mat cannyFrame; 
        cv::Mat grayFrame;
        
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);
        
        cv::Canny(grayFrame, cannyFrame, 100, 150);

        cv::cvtColor(cannyFrame, cannyFrame, cv::COLOR_GRAY2BGR); // Convert back to BGR for consistent output format (HUD overlay, etc.)
        
        return cannyFrame;
    }
}
