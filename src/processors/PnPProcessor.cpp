#include "processors/PnPProcessor.h"

#include <opencv2/calib3d.hpp>

#include "Config.h"

namespace vision
{
    void PnPProcessor::estimatePose(
        const std::vector<cv::Point3f>& objectPoints,
        const std::vector<cv::Point2f>& imagePoints
    ) 
    {
        poseValid = false; 

        if (objectPoints.size() < 4 || imagePoints.size() < 4 || objectPoints.size() != imagePoints.size())
        {
            return;        
        }

        bool success = cv::solvePnP(objectPoints, imagePoints, Config::CAMERA_INTRINSICS, cv::noArray(), rvec, tvec);

        poseValid = success;
    }

    bool PnPProcessor::hasValidPose() const
    {
        return poseValid;
    }

    const cv::Mat& PnPProcessor::getRotationVector() const
    {
        return rvec;
    }

    const cv::Mat& PnPProcessor::getTranslationVector() const
    {
        return tvec;
    }

}
