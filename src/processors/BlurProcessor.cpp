#include "processors/BlurProcessor.h"
#include <opencv2/imgproc.hpp>


namespace vision
{
    cv::Mat BlurProcessor::process(const cv::Mat& frame)
    {
        cv::Mat blurFrame;
        
        cv::GaussianBlur(frame, blurFrame, cv::Size(5, 5), 0);
            
        return blurFrame;
    }
}
