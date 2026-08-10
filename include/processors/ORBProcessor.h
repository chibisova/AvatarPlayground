#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor for detecting and describing keypoints using the ORB algorithm
namespace vision
{
    class ORBProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "ORB"; 
            }
    }; 
}
