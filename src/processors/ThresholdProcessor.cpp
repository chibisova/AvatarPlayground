#include "processors/ThresholdProcessor.h"
#include <opencv2/imgproc.hpp>


namespace vision
{
    cv::Mat ThresholdProcessor::process(const cv::Mat& frame)
    {
        cv::Mat threshFrame;
        
        cv::cvtColor(frame, threshFrame, cv::COLOR_BGR2GRAY);
        
        cv::GaussianBlur(threshFrame, threshFrame, cv::Size(5, 5), 0);
        
        cv::adaptiveThreshold(threshFrame, threshFrame, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 2);

        cv::cvtColor(threshFrame, threshFrame, cv::COLOR_GRAY2BGR); // Convert back to BGR for consistent output format (HUD overlay, etc.)
        
        return threshFrame;
    }
}
