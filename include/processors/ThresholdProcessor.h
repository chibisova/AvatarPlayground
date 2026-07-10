#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor that applies adaptive thresholding to an image
namespace vision
{
    class ThresholdProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
    public:
        cv::Mat process(const cv::Mat& frame) override;

        const char* name() const override
        { 
            return "Adaptive Threshold"; 
        }
    }; 
}
