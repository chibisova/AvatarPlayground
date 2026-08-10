#pragma once

#include <opencv2/core.hpp>
#include <vector>

// Processor for Perspective-n-Point (PnP) pose estimation
/* 
    This processor estimates the camera pose (rotation and translation) 
    given a set of 3D points in the world and their corresponding 2D projections in the image.
    Input: objectPoints (3D points), imagePoints (2D points)
    Output: rvec (rotation vector), tvec (translation vector), poseValid (boolean indicating if the pose estimation was successful)
*/

namespace vision
{
    class PnPProcessor
    {
    private:
        cv::Mat rvec;
        cv::Mat tvec;
        bool poseValid = false;

    public:
        void estimatePose(
            const std::vector<cv::Point3f>& objectPoints,
            const std::vector<cv::Point2f>& imagePoints
        );

        bool hasValidPose() const;

        const cv::Mat& getRotationVector() const;
        const cv::Mat& getTranslationVector() const;
    };
}
