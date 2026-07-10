#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor that detects Shi-Tomasi corners in an image
namespace vision
{
    class ShiTomasiProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
    public:
        cv::Mat process(const cv::Mat& frame) override;

        const char* name() const override
        { 
            return "Shi-Tomasi Corners"; 
        }
    }; 
}
