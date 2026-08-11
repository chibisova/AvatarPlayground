#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "MapPoint.h"

// Class to manage a 3D map of points
/* 
    This class maintains a collection of 3D points (MapPoint) that represent the environment.
    It provides methods to add new points, retrieve existing points, clear the map, and match 
    descriptors of the current frame with the map's descriptors for feature matching.
*/
namespace vision
{
    class Map3D
    {
    private:
        // List of 3D points in the map
        std::vector<MapPoint> mapPoints;

    public:
        // Add new 3D points to the map
        void addPoints(const std::vector<MapPoint>& points);

        const std::vector<MapPoint>& getPoints() const;

        void clear();

        // Function to match descriptors of current frame with the map's descriptors
        std::vector<cv::DMatch> matchDescriptors(const cv::Mat& currentDescriptors) const;

    };
}
