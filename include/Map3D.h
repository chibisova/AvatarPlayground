#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "MapPoint.h"

namespace vision
{
    class Map3D
    {
    private:
        std::vector<MapPoint> mapPoints;

    public:
        void addPoints(const std::vector<MapPoint>& points);

        const std::vector<MapPoint>& getPoints() const;

        void clear();
    };
}
