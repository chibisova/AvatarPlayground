#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor that matches keypoints between two frames using KNN matching
/*  
    Input: current frame, previous frame, previous keypoints, previous descriptors
    Output: good matches between previous and current keypoints
*/
namespace vision
{
    class KNNMatcherProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::KeyPoint> previousKeypoints;
            cv::Mat previousDescriptors;

        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "KNN Matcher"; 
            }
    }; 
}
