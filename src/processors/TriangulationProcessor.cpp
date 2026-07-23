#include "processors/TriangulationProcessor.h"

#include <opencv2/calib3d.hpp>

#include "Config.h"

namespace vision
{

    const cv::Mat& TriangulationProcessor::getPoints3D() const
    {
        return points3D;
    }
    
    void TriangulationProcessor::triangulate(
        const cv::Mat& R,
        const cv::Mat& t,
        const std::vector<cv::Point2f>& previousPoints,
        const std::vector<cv::Point2f>& currentPoints
    )
    {
        if (previousPoints.size() < 2 || currentPoints.size() < 2)
        {
            points3D.release();
            return;
        }

        if (previousPoints.size() != currentPoints.size())
        {
            points3D.release();
            return;
        }
        // Get camera's data
        const cv::Mat& K = Config::CAMERA_INTRINSICS;

        // First camera: K [I | 0]
        cv::Mat P1 = cv::Mat::zeros(3, 4, CV_64F);

        K.copyTo(P1(cv::Rect(0, 0, 3, 3)));

        // Second camera: K [R | t]
        cv::Mat P2 = cv::Mat::zeros(3, 4, CV_64F);

        R.copyTo(P2(cv::Rect(0, 0, 3, 3)));
        t.copyTo(P2(cv::Rect(3, 0, 1, 3)));

        P2 = K * P2;

        // Triangulate
        cv::Mat points4D;

        cv::triangulatePoints(P1, P2, previousPoints, currentPoints, points4D);

        // Convert homogeneous coordinates to 3D
        points3D = cv::Mat::zeros(
            points4D.cols,
            3,
            CV_64F
        );

        for (int i = 0; i < points4D.cols; ++i){
            float X = points4D.at<float>(0, i);
            float Y = points4D.at<float>(1, i);
            float Z = points4D.at<float>(2, i);
            float W = points4D.at<float>(3, i);

            if (std::abs(W) < 1e-6)
                continue;

            points3D.at<double>(i, 0) = static_cast<double>(X / W);
            points3D.at<double>(i, 1) = static_cast<double>(Y / W);
            points3D.at<double>(i, 2) = static_cast<double>(Z / W);
        }

        // Debug
        std::cout << "Triangulated points: " << points3D.rows << std::endl;

        for (int i = 0; i < std::min(5, points3D.rows); ++i)
        {
            std::cout << points3D.row(i) << std::endl;
        }

        // Depth test
        int positiveDepth = 0;

        for (int i = 0; i < points3D.rows; ++i)
        {
            double Z = points3D.at<double>(i, 2);

            if (Z > 0)
                positiveDepth++;
        }

        std::cout << "Points in front of camera: "
                << positiveDepth
                << " / "
                << points3D.rows
                << std::endl;
    }

}
