#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "processors/IImageProcessor.h"
#include "Map3D.h"

// Processor for Perspective-n-Point (PnP) pose estimation
/* 
    This processor estimates the camera pose (rotation and translation) 
    given a set of 3D points in the world and their corresponding 2D projections in the image.
    Input: objectPoints (3D points), imagePoints (2D points)
    Output: rvec (rotation vector), tvec (translation vector), poseValid (boolean indicating if the pose estimation was successful)
*/

namespace vision
{
    class PnPProcessor : public IImageProcessor
    {
    private:
        cv::Mat rvec;
        cv::Mat tvec;
        bool poseValid = false;

        const Map3D* map = nullptr;

        void estimatePose(
            const std::vector<cv::Point3f>& objectPoints,
            const std::vector<cv::Point2f>& imagePoints
        );

    public:
        cv::Mat process(const cv::Mat& frame) override;
        
        bool hasValidPose() const;
        void setMap(const Map3D* map);

        const cv::Mat& getRotationVector() const;
        const cv::Mat& getTranslationVector() const;

        const char* name() const override
        {
            return "PnP";
        }
    };
}
