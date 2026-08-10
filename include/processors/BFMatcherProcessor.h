#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor for feature detector & descriptor
/* 
    This processor detects keypoints and computes descriptors using the ORB algorithm.
    It can be used to extract features from an image for tasks such as matching and tracking.
*/
namespace vision
{
    class BFMatcherProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::KeyPoint> previousKeypoints;
            cv::Mat previousDescriptors;

        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "BFMatcher"; 
            }
    }; 
}
