#include "processors/GrayProcessor.h"
#include <opencv2/imgproc.hpp>


namespace vision
{
    cv::Mat GrayProcessor::process(const cv::Mat& frame)
    {
        cv::Mat gray;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(gray, gray, cv::COLOR_GRAY2BGR); // Convert back to BGR for consistent output format (HUD overlay, etc.)

        return gray;
    }
}
