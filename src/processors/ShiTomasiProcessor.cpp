#include "processors/ShiTomasiProcessor.h"
#include <opencv2/imgproc.hpp>


namespace vision
{
    cv::Mat ShiTomasiProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::Point2f> cornersFrame;
        cv::Mat grayFrame;
        cv::Mat output = frame.clone();
        
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        
        cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);

        cv::goodFeaturesToTrack(grayFrame, cornersFrame, 200, 0.01, 10);

        for (const cv::Point2f& corner : cornersFrame)
        {
            cv::circle(output, corner, 5, cv::Scalar(0, 255, 0), -1);
        }
        return output;
    }
}
