#include "CameraCalibration.h"

namespace vision
{
    cv::Mat CameraCalibration::createIntrinsicMatrix(int width, int height)
    {
        double fx = width;
        double fy = width;

        double cx = width / 2.0;
        double cy = height / 2.0;

        return (cv::Mat_<double>(3,3) <<
            fx, 0, cx,
            0, fy, cy,
            0, 0, 1);
    }
}
