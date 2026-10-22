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

            std::vector<cv::KeyPoint> matchedPreviousKeypoints;
            std::vector<cv::KeyPoint> matchedCurrentKeypoints;
            std::vector<cv::DMatch> lastGoodMatches;

        public:
            cv::Mat process(const cv::Mat& frame) override;

            const std::vector<cv::KeyPoint>& getMatchedPreviousKeypoints() const;
            const std::vector<cv::KeyPoint>& getMatchedCurrentKeypoints() const;
            const std::vector<cv::DMatch>& getGoodMatches() const;

            const char* name() const override
            { 
                return "KNN Matcher"; 
            }
    }; 
}
