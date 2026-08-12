#include "processors/TriangulationProcessor.h"

#include <opencv2/calib3d.hpp>
#include <cmath>

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
        
        // Explicitly convert triangulation output to CV_64F
        cv::Mat points4D64;
        points4D.convertTo(points4D64, CV_64F);

        std::cout
            << "points4D type = "
            << points4D.type()
            << '\n';

        std::cout
            << "points4D64 type = "
            << points4D64.type()
            << '\n';

        std::cout
            << "points4D64.cols = "
            << points4D64.cols
            << '\n';

        // Convert homogeneous coordinates to 3D by devision by W
        points3D = cv::Mat::zeros(
            points4D64.cols,
            3,
            CV_64F
        );
        
        for (int i = 0; i < points4D64.cols; ++i)
        {
            double X = points4D64.at<double>(0, i);
            double Y = points4D64.at<double>(1, i);
            double Z = points4D64.at<double>(2, i);
            double W = points4D64.at<double>(3, i);
        
            if (std::abs(W) < 1e-6)
                continue;
        
            points3D.at<double>(i, 0) = X / W;
            points3D.at<double>(i, 1) = Y / W;
            points3D.at<double>(i, 2) = Z / W;
        }
        // Debug
        std::cout << "Triangulated points: " << points3D.rows << std::endl;

        for (int i = 0; i < std::min(5, points3D.rows); ++i)
        {
            std::cout << points3D.row(i) << std::endl;
        }

        // Map3DaddPoints();

        // Depth test
        // Validate triangulated points
        int positiveDepth = 0;
        int validReprojection = 0;

        constexpr double REPROJECTION_ERROR_THRESHOLD = 5.0;

        for (int i = 0; i < points3D.rows; ++i)
        {
            assert(i < goodMatches.size());

            int queryIdx = goodMatches[i].queryIdx;

            if (queryIdx < 0 || queryIdx >= previousDescriptors.rows)
            {
                std::cout << "queryIdx = " << queryIdx << '\n';
                std::cout << "rows = " << previousDescriptors.rows << '\n';
                std::cout << "i = " << i << '\n';
                continue;
            }

            double X = points3D.at<double>(i, 0);
            double Y = points3D.at<double>(i, 1);
            double Z = points3D.at<double>(i, 2);

            // Reject points behind the first camera
            if (Z <= 0.0)
                continue;

            positiveDepth++;

            // 3D point in homogeneous coordinates
            cv::Mat point4D = (cv::Mat_<double>(4, 1) <<
                X, Y, Z, 1.0
            );

            // Project into previous frame
            cv::Mat projectedPrevious = P1 * point4D;

            double previousProjectedX =
                projectedPrevious.at<double>(0) /
                projectedPrevious.at<double>(2);

            double previousProjectedY =
                projectedPrevious.at<double>(1) /
                projectedPrevious.at<double>(2);

            // Project into current frame
            cv::Mat projectedCurrent = P2 * point4D;

            double currentProjectedX =
                projectedCurrent.at<double>(0) /
                projectedCurrent.at<double>(2);

            double currentProjectedY =
                projectedCurrent.at<double>(1) /
                projectedCurrent.at<double>(2);

            // Calculate reprojection errors
            double dx = previousProjectedX - previousPoints[i].x;
            double dy = previousProjectedY - previousPoints[i].y;

            double previousError = std::sqrt(dx * dx + dy * dy);

            dx = currentProjectedX - currentPoints[i].x;
            dy = currentProjectedY - currentPoints[i].y;

            double currentError = std::sqrt(dx * dx + dy * dy);

            // Reject geometrically inconsistent points
            if (previousError > REPROJECTION_ERROR_THRESHOLD ||
                currentError > REPROJECTION_ERROR_THRESHOLD)
            {
                continue;
            }

            validReprojection++;

            // Create MapPoint
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

        std::cout << "Points in front of camera: "
          << positiveDepth
          << " / "
          << points3D.rows
          << std::endl;

        std::cout << "Valid reprojection points: "
                << validReprojection
                << " / "
                << positiveDepth
                << std::endl;
        return validPoints;
    }

}
