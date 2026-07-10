#include "processors/HarrisProcessor.h"
#include <opencv2/imgproc.hpp>
#include "Config.h"

namespace vision
{
    cv::Mat HarrisProcessor::process(const cv::Mat& frame)
    {
        cv::Mat grayFrame;
        cv::Mat harrisResponse;
        cv::Mat normalizedResponse;
        cv::Mat outputFrame = frame.clone();
        
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);
        
        cv::cornerHarris(grayFrame, harrisResponse, 5, 3, 0.04);
        
        // Normalize the Harris response for visualization
        cv::normalize(
            harrisResponse,
            normalizedResponse,
            0,
            255,
            cv::NORM_MINMAX,
            CV_8U);

        // Draw circles on the corners detected by Harris
        for (int y = 0; y < normalizedResponse.rows; ++y)
        {
            for (int x = 0; x < normalizedResponse.cols; ++x)
            {
                if (normalizedResponse.at<uchar>(y, x) > Config::HARRIS_THRESHOLD)
                {
                    cv::circle(outputFrame, cv::Point(x, y), 5, cv::Scalar(0,255,0), -1);
                }
            }
        }
        
        return outputFrame;
    }
}
