#pragma once

#include <opencv2/core/mat.hpp>

// Interface for image processing classes
namespace vision
{
    class IImageProcessor
    {
    public:
        virtual ~IImageProcessor() = default; // Virtual destructor for proper cleanup of derived classes
    
        virtual cv::Mat process(const cv::Mat& frame) = 0; // Pure virtual function to process an image frame
    
        virtual const char* name() const = 0; // Pure virtual function to get the name of the processor
    };
    
}
