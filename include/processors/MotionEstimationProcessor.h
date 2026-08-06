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
            std::vector<cv::KeyPoint> currentKeypoints;
            
            cv::Mat previousDescriptors;
            cv::Mat currentDescriptors;

            // Frozen copy of previousKeypoints/previousDescriptors as they were
            // when goodMatches was computed, taken before advanceFrame() overwrites them.
            std::vector<cv::KeyPoint> frozenPreviousKeypoints;
            cv::Mat frozenPreviousDescriptors;

            cv::Mat relativeRotation;
            cv::Mat relativeTranslation;

            std::vector<cv::DMatch> goodMatches; // Exposing good matches for Map3D to get descriptors of matched points

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

            const std::vector<cv::DMatch>& getGoodMatches() const;

            const std::vector<cv::Point2f>& getPreviousPoints() const;
            const std::vector<cv::Point2f>& getCurrentPoints() const;

            const std::vector<cv::KeyPoint>& getPreviousKeypoints() const;
            const std::vector<cv::KeyPoint>& getCurrentKeypoints() const;

            const cv::Mat& getPreviousDescriptors() const;
            const cv::Mat& getCurrentDescriptors() const;

            void advanceFrame(const cv::Mat& frame);

            bool hasValidPose() const;
    }; 
}
