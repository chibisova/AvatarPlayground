#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor for homography estimation
// This processor estimates the homography between two frames using RANSAC

/* 
    After BFMatcher or KNNMatcher finds good matches between keypoints in two frames,
    RANSAC is applied to these matches to compute a robust homography matrix that maps 
    points from the previous frame to the current frame.
*/

namespace vision
{
    class RANSACProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::KeyPoint> previousKeypoints;
            cv::Mat previousDescriptors;

        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "RANSAC"; 
            }
    }; 
}
