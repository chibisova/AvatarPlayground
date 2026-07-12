#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor for feature detector & descriptor
namespace vision
{
    class ORBProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::Point2f> previousCorners;
        
        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "ORB"; 
            }
    }; 
}
