#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// Processor for feature detector & descriptor
namespace vision
{
    class MotionEstimationProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::KeyPoint> previousKeypoints;
            cv::Mat previousDescriptors;
            cv::Mat relativeRotation;
            cv::Mat relativeTranslation;
            bool poseValid = false;
        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "Motion Estimation"; 
            }
            const cv::Mat& getRotation() const;
            const cv::Mat& getTranslation() const;
            bool hasValidPose() const;
    }; 
}
