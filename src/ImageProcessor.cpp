#include "ImageProcessor.h"

cv::Mat processFrame(const cv::Mat& frame, ProcessingMode mode)
{
    switch (mode)
    {
        case ProcessingMode::Original:
            return frame.clone();
        
        case ProcessingMode::Gray:
        {
            cv::Mat grayFrame;
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            return grayFrame;
        }

        default:
            return frame.clone();
    }
    
}

std::string processingModeToString(ProcessingMode mode)
{
    switch(mode)
    {
        case ProcessingMode::Original:
            return "Original";

        case ProcessingMode::Gray:
            return "Grayscale";

        case ProcessingMode::Blur:
            return "Blur";

        case ProcessingMode::Canny:
            return "Canny";

        case ProcessingMode::Threshold:
            return "Threshold";

        case ProcessingMode::Corners:
            return "Corners";
    }

    return "Unknown";
}
