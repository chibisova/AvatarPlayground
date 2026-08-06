#pragma once

#include <opencv2/core.hpp>

namespace vision
{
    struct MapPoint
    {
        cv::Point3f position;
        cv::Mat descriptor;
    };
}
