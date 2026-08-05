#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace vision
{
    // Estimate the 3D position of the matched visual features
    class TriangulationProcessor
    {
    private:
        cv::Mat points3D; 

    public:
        std::vector<cv::Point3f> triangulate(
            const cv::Mat& R,
            const cv::Mat& t,
            const std::vector<cv::Point2f>& previousPoints,
            const std::vector<cv::Point2f>& currentPoints
        );

        const cv::Mat& getPoints3D() const;
    };
}
