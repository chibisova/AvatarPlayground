#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main()
{
    // 1. Define known 3D points
    std::vector<cv::Point3f> objectPoints =
    {
        {0.0f, 0.0f, 5.0f},
        {1.0f, 0.0f, 5.0f},
        {1.0f, 1.0f, 5.0f},
        {0.0f, 1.0f, 5.0f},
        {0.0f, 0.0f, 6.0f},
        {1.0f, 0.0f, 6.0f},
        {1.0f, 1.0f, 6.0f},
        {0.0f, 1.0f, 6.0f}
    };

    // 2. Camera intrinsics
    cv::Mat K = (cv::Mat_<double>(3, 3) <<
        1280, 0, 640,
        0, 1280, 360,
        0, 0, 1
    );

    // 3. Known ground-truth camera pose
    cv::Mat rvecGroundTruth = cv::Mat::zeros(3, 1, CV_64F);

    cv::Mat tvecGroundTruth = (cv::Mat_<double>(3, 1) <<
        0.0,
        0.0,
        0.0
    );

    // 4. Project 3D points into the image
    std::vector<cv::Point2f> imagePoints;

    cv::projectPoints(
        objectPoints,
        rvecGroundTruth,
        tvecGroundTruth,
        K,
        cv::noArray(),
        imagePoints
    );

    // 5. Recover pose using PnP
    cv::Mat rvecEstimated;
    cv::Mat tvecEstimated;

    bool success = cv::solvePnP(
        objectPoints,
        imagePoints,
        K,
        cv::noArray(),
        rvecEstimated,
        tvecEstimated
    );

    if (!success)
    {
        std::cout << "PnP failed\n";
        return -1;
    }

    std::cout << "Ground-truth translation:\n"
              << tvecGroundTruth << "\n\n";

    std::cout << "Estimated translation:\n"
              << tvecEstimated << "\n\n";

    std::cout << "Ground-truth rotation vector:\n"
              << rvecGroundTruth << "\n\n";

    std::cout << "Estimated rotation vector:\n"
              << rvecEstimated << "\n";

    return 0;
}
