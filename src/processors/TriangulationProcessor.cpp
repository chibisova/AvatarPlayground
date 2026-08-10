#include "processors/TriangulationProcessor.h"

#include <opencv2/calib3d.hpp>

#include "Config.h"

namespace vision
{

    const cv::Mat& TriangulationProcessor::getPoints3D() const
    {
        return points3D;
    }


    std::vector<MapPoint> TriangulationProcessor::triangulate(
        const cv::Mat& R,
        const cv::Mat& t,
        const std::vector<cv::KeyPoint>& previousKeypoints,
        const std::vector<cv::KeyPoint>& currentKeypoints,
        const cv::Mat& previousDescriptors,
        const std::vector<cv::DMatch>& goodMatches
    )
    {
        std::cout << "Entering Triangulation\n";
        std::cout << previousDescriptors.rows << '\n';

        int maxIdx = -1;

        for (const auto& m : goodMatches)
            maxIdx = std::max(maxIdx, m.queryIdx);

        std::cout << "largest queryIdx = "
                << maxIdx
                << '\n';

        std::vector<cv::Point2f> previousPoints;
        std::vector<cv::Point2f> currentPoints;

        for (const auto& match : goodMatches)
        {
            previousPoints.push_back(
                previousKeypoints[match.queryIdx].pt);

            currentPoints.push_back(
                currentKeypoints[match.trainIdx].pt);
        }
        
        if (previousPoints.size() < 2 || currentPoints.size() < 2)
        {
            points3D.release();
            return {};
        }

        if (previousPoints.size() != currentPoints.size())
        {
            points3D.release();
            return {};
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
        std::vector<MapPoint> validPoints;
        
        validPoints.clear();

        cv::triangulatePoints(P1, P2, previousPoints, currentPoints, points4D);
        std::cout
            << "points4D.cols = "
            << points4D.cols
            << '\n';

        std::cout
            << "goodMatches.size() = "
            << goodMatches.size()
            << '\n';

        std::cout
            << "previousPoints.size() = "
            << previousPoints.size()
            << '\n';

        // Convert homogeneous coordinates to 3D by devision by W
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

        // Map3DaddPoints();

        // Depth test
        int positiveDepth = 0;

        for (int i = 0; i < points3D.rows; ++i)
        {
            assert(i < goodMatches.size());

            int queryIdx = goodMatches[i].queryIdx;
            if (queryIdx < 0 || queryIdx >= previousDescriptors.rows)
            {
                std::cout << "queryIdx = " << queryIdx << '\n';
                std::cout << "rows     = " << previousDescriptors.rows << '\n';
                std::cout << "goodMatches.size() = " << goodMatches.size() << '\n';
                std::cout << "i = " << i << '\n';

                continue;
            }

            double Z = points3D.at<double>(i, 2);
            double Y = points3D.at<double>(i, 1);
            double X = points3D.at<double>(i, 0);

            if (Z > 0) {
                positiveDepth++;

                MapPoint point;

                point.position = cv::Point3f(
                    static_cast<float>(X),
                    static_cast<float>(Y),
                    static_cast<float>(Z)
                );

                point.descriptor =
                    previousDescriptors
                        .row(goodMatches[i].queryIdx)
                        .clone();

                validPoints.push_back(point);
                
            }
        }

        std::cout << "Points in front of camera: "
                << positiveDepth
                << " / "
                << points3D.rows
                << std::endl;
        
        return validPoints;
    }

}
