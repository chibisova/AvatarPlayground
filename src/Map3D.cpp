#include "Map3D.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

namespace vision 
{
    void Map3D::addPoints(const std::vector<MapPoint>& points)
    {
        mapPoints.insert(
            mapPoints.end(),
            points.begin(),
            points.end()
        );
    }

    const std::vector<MapPoint>& Map3D::getPoints() const
    {
        return mapPoints;
    }

    void Map3D::clear() 
    {
        mapPoints.clear();
    }

    std::vector<cv::DMatch> Map3D::matchDescriptors(const cv::Mat& currentDescriptors) const
    {
        std::vector<cv::DMatch> matches;

        if (mapPoints.empty() || currentDescriptors.empty())
            return matches;
        
        // Create a matrix of descriptors from the map points
        cv::Mat mapDescriptors(mapPoints.size(), currentDescriptors.cols, currentDescriptors.type());
        for (size_t i = 0; i < mapPoints.size(); ++i) 
        {
            mapPoints[i].descriptor.copyTo(mapDescriptors.row(i));
        }

        // Use BFMatcher to find matches between current descriptors and map descriptors
        cv::BFMatcher matcher(cv::NORM_HAMMING, true);
        matcher.match(mapDescriptors, currentDescriptors, matches);

        return matches;
    }
}
