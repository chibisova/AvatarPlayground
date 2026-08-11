#pragma once

#include <opencv2/core.hpp>

// Structure to represent a 3D point in the map
/* 
    This structure holds the 3D position of a point in space and its associated descriptor.
    The position is represented as a 3D point (cv::Point3f), and the descriptor is a matrix (cv::Mat)
    that contains feature information for matching and tracking purposes.
*/
namespace vision
{
    struct MapPoint
    {
        cv::Point3f position;
        cv::Mat descriptor;
        int observations = 1;
        int lastSeenFrame = 0;
        bool isBad = false;
    };
}
