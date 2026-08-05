#pragma once

#include <opencv2/core.hpp>
#include <vector>

namespace vision
{
    class Map3D
    {
    private:
        std::vector<cv::Point3f> mapPoints;

    public:
        void addPoints(const std::vector<cv::Point3f>& points);

        const std::vector<cv::Point3f>& getPoints() const;

        void clear();
    };
}
