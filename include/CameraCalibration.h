#pragma once

#include <opencv2/core/mat.hpp>

namespace vision
{
    class CameraCalibration
    {
    public:
        static cv::Mat createIntrinsicMatrix(int width, int height);
    };
}
