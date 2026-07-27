#pragma once

#include <opencv2/core.hpp>
#include <vector>

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
