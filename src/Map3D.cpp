#include "Map3D.h"

namespace vision 
{
    void Map3D::addPoints(const std::vector<cv::Point3f>& points)
    {
        mapPoints.insert(
            mapPoints.end(),
            points.begin(),
            points.end()
        );
    }

    const std::vector<cv::Point3f>& Map3D::getPoints() const
    {
        return mapPoints;
    }

    void Map3D::clear() 
    {
        mapPoints.clear();
    }
}
