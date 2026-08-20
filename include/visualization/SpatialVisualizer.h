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

        void saveScreenshot(const std::string& filename);
        
        void saveOverviewScreenshot(const std::string& filename);
        
        bool wasStopped() const;

    private:
        void fitView();
    
        cv::viz::Viz3d window;
        cv::viz::Viz3d overviewWindow;
    
        cv::Point3f sceneCenter;
        float sceneRadius;
        float lastFittedRadius;
    
        bool initialized;
    
        std::vector<cv::Point3f> cameraTrajectory;
    };
}
