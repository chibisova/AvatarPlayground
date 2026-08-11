#include "processors/PnPProcessor.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

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

        bool success = cv::solvePnPRansac(objectPoints, imagePoints, Config::CAMERA_INTRINSICS, cv::noArray(), rvec, tvec);

        poseValid = success;
    }

    /*
        process(frame, map)
        │
        ├── ORB detectAndCompute()
        │
        ├── map.matchDescriptors(currentDescriptors)
        │
        ├── Build objectPoints
        │
        ├── Build imagePoints
        │
        ├── estimatePose(objectPoints, imagePoints)
        │
        └── Return visualization
    */

    cv::Mat PnPProcessor::process(const cv::Mat &frame)
    {
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat currentDescriptors;
        cv::Mat processed;
        
        cv::cvtColor(frame, processed, cv::COLOR_BGR2GRAY);

        cv::Ptr<cv::ORB> orb = cv::ORB::create();

        orb->detectAndCompute(processed, cv::noArray(), keypoints, currentDescriptors);

        if (!map)
        {
            return frame.clone();
        }
        auto matches = map->matchDescriptors(currentDescriptors);

        std::vector<cv::Point3f> objectPoints;
        std::vector<cv::Point2f> imagePoints;

        for (const auto& match : matches)
        {
            objectPoints.push_back(
                map->getPoints()[match.queryIdx].position
            );

            imagePoints.push_back(
                keypoints[match.trainIdx].pt
            );
        }

        if (matches.size() < 4)
        {
            poseValid = false;
            return frame.clone();
        }

        estimatePose(objectPoints, imagePoints);

        std::cout << "Map points: "
                << map->getPoints().size()
                << '\n';

        std::cout << "Descriptor matches: "
                << matches.size()
                << '\n';

        std::cout << "PnP correspondences: "
                << objectPoints.size()
                << '\n';

        return frame.clone();
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

    void PnPProcessor::setMap(const Map3D* map)
    {
        this->map = map;
    }

}
