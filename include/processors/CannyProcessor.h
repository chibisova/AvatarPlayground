#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor that applies Canny edge detection to an image
namespace vision
{
    class CannyProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
    public:
        cv::Mat process(const cv::Mat& frame) override;

        const char* name() const override
        { 
            return "Canny Edge"; 
        }
    }; 
}
