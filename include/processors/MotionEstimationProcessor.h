#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct
#include <vector>

namespace vision
{
    // Estimate Camera motion between frames
    class MotionEstimationProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::KeyPoint> previousKeypoints;
            cv::Mat previousDescriptors;

            cv::Mat relativeRotation;
            cv::Mat relativeTranslation;

            std::vector<cv::Point2f> previousPoints;
            std::vector<cv::Point2f> currentPoints;

            bool poseValid = false;
        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "Motion Estimation"; 
            }

            const cv::Mat& getRotation() const;
            const cv::Mat& getTranslation() const;

            const std::vector<cv::Point2f>& getPreviousPoints() const;
            const std::vector<cv::Point2f>& getCurrentPoints() const;

            bool hasValidPose() const;
    }; 
}
