#include "processors/OriginalProcessor.h"
#include <opencv2/imgproc.hpp>


namespace vision
{
    cv::Mat OriginalProcessor::process(const cv::Mat& frame)
    {
        return frame.clone();
    }
}
