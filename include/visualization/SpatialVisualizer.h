#pragma once

#include <opencv2/viz.hpp>
#include <opencv2/core.hpp>

#include <string>
#include <vector>

#include "Map3D.h"

namespace vision
{
    class SpatialVisualizer
    {
    public:
        SpatialVisualizer();

        void updateMap(const Map3D& map);

        void updateCameraPose(
            const cv::Mat& worldPose
        );

        void show();

        bool wasStopped() const;

    private:
        cv::viz::Viz3d window;

        std::vector<cv::Point3f> cameraTrajectory;

        bool initialized;
    };
}
