#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "MapPoint.h"

namespace vision
{
    // Estimate the 3D position of the matched visual features
    // Input: R, t, previousKeypoints, currentKeypoints, previousDescriptors, goodMatches
    // Output: points3D (cv::Mat of 3D points), vector<MapPoint> (3D points with descriptors)
    class TriangulationProcessor
    {
    private:
        cv::Mat points3D; 

    public:
        std::vector<MapPoint> triangulate(
            const cv::Mat& R,
            const cv::Mat& t,
            const std::vector<cv::KeyPoint>& previousKeypoints,
            const std::vector<cv::KeyPoint>& currentKeypoints,
            const cv::Mat& previousDescriptors,
            const std::vector<cv::DMatch>& goodMatches
            
        );

        const cv::Mat& getPoints3D() const;
    };
}
