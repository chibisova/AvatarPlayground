#include "visualization/SpatialVisualizer.h"

#include <opencv2/viz/widgets.hpp>

namespace vision
{
    SpatialVisualizer::SpatialVisualizer()
        : window("Spatial Avatar Playground"),
          initialized(false)
    {
        window.showWidget(
            "CoordinateSystem",
            cv::viz::WCoordinateSystem(0.1)
        );

        window.setBackgroundColor(
            cv::viz::Color::black()
        );
    }


    void SpatialVisualizer::updateMap(
        const Map3D& map
    )
    {
        std::vector<cv::Point3f> points;

        const auto& mapPoints =
            map.getPoints();

        points.reserve(mapPoints.size());

        for (const auto& point : mapPoints)
        {
            if (point.isBad)
                continue;

            points.push_back(point.position);
        }

        if (points.empty())
            return;

        cv::Mat pointCloud(points);

        cv::viz::WCloud cloudWidget(
            pointCloud,
            cv::viz::Color::white()
        );
        
        cloudWidget.setRenderingProperty(
            cv::viz::POINT_SIZE,
            3
        );
        
        window.showWidget(
            "MapPoints",
            cloudWidget
        );

        initialized = true;
    }


    void SpatialVisualizer::updateCameraPose(
        const cv::Mat& worldPose
    )
    {
        if (
            worldPose.empty() ||
            worldPose.rows != 4 ||
            worldPose.cols != 4
        )
        {
            return;
        }

        cv::Point3f cameraPosition(
            static_cast<float>(
                worldPose.at<double>(0, 3)
            ),
            static_cast<float>(
                worldPose.at<double>(1, 3)
            ),
            static_cast<float>(
                worldPose.at<double>(2, 3)
            )
        );

        cameraTrajectory.push_back(
            cameraPosition
        );

        // -------------------------------------------------
        // Camera trajectory
        // -------------------------------------------------

        if (cameraTrajectory.size() >= 2)
        {
            cv::Mat trajectoryMat(
                cameraTrajectory
            );

            cv::viz::WPolyLine trajectoryWidget(
                trajectoryMat,
                cv::viz::Color::green()
            );
            
            trajectoryWidget.setRenderingProperty(
                cv::viz::LINE_WIDTH,
                3
            );
            
            window.showWidget(
                "CameraTrajectory",
                trajectoryWidget
            );
        }


        // -------------------------------------------------
        // Current camera coordinate frame
        // -------------------------------------------------

        cv::Affine3d pose(
            worldPose
        );

        window.showWidget(
            "CurrentCamera",
            cv::viz::WCameraPosition(
                0.5
            ),
            pose
        );

        initialized = true;
    }


    void SpatialVisualizer::show()
    {
        window.spinOnce(
            1,
            true
        );
    }


    bool SpatialVisualizer::wasStopped() const
    {
        return window.wasStopped();
    }
}
