#include "ImageProcessor.h"
#include "Config.h"
#include "processors/GrayProcessor.h"
#include "processors/BlurProcessor.h"
#include "processors/HarrisProcessor.h"
#include "processors/CannyProcessor.h"
#include "processors/ThresholdProcessor.h"
#include "processors/ShiTomasiProcessor.h"


cv::Mat processFrame(const cv::Mat& frame, ProcessingMode mode)
{
    switch (mode)
    {
        case ProcessingMode::Original:
            return frame.clone();
        
        case ProcessingMode::Gray:
        {
            vision::GrayProcessor grayProcessor;
            return grayProcessor.process(frame);
        }

        case ProcessingMode::Blur:
        {
            vision::BlurProcessor blurProcessor;
            return blurProcessor.process(frame);
        }

        case ProcessingMode::Canny:
        {
            vision::CannyProcessor cannyProcessor;
            return cannyProcessor.process(frame);

        }

        case ProcessingMode::Threshold:
        {
            vision::ThresholdProcessor thresholdProcessor;
            return thresholdProcessor.process(frame);
        }

        case ProcessingMode::ShiTomasiCorners:
        {
            vision::ShiTomasiProcessor shiTomasiProcessor;
            return shiTomasiProcessor.process(frame);
        }

        case ProcessingMode::HarrisCorners:
        {
            vision::HarrisProcessor harrisProcessor;
            return harrisProcessor.process(frame);
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

        case ProcessingMode::ShiTomasiCorners:
            return "Shi-Tomasi Corners";
        
        case ProcessingMode::HarrisCorners:
            return "Harris Corners";
    }

    return "Unknown";
}
