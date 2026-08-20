#include "visualization/SpatialVisualizer.h"

#include <opencv2/viz/widgets.hpp>
#include <algorithm>

namespace vision
{
    SpatialVisualizer::SpatialVisualizer()
        : window("Spatial Avatar Playground"),
        overviewWindow("Spatial Avatar Playground - Overview"),
        sceneCenter(0.0f, 0.0f, 0.0f),
        sceneRadius(1.0f),
        lastFittedRadius(0.0f),
        initialized(false)
    {
        window.showWidget(
            "CoordinateSystem",
            cv::viz::WCoordinateSystem(0.1)
        );

        overviewWindow.showWidget(
            "CoordinateSystem",
            cv::viz::WCoordinateSystem(0.1)
        );

        window.setBackgroundColor(
            cv::viz::Color::black()
        );

        overviewWindow.setBackgroundColor(
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
        
        // -------------------------------------------------
        // Compute scene bounds for automatic framing
        // -------------------------------------------------

        cv::Point3f minPoint = points[0];
        cv::Point3f maxPoint = points[0];

        // -------------------------------------------------
        // Robust scene bounds for visualization
        // Ignore extreme outlier landmarks when framing
        // -------------------------------------------------

        std::vector<float> xs;
        std::vector<float> ys;
        std::vector<float> zs;

        xs.reserve(points.size());
        ys.reserve(points.size());
        zs.reserve(points.size());

        for (const auto& point : points)
        {
            xs.push_back(point.x);
            ys.push_back(point.y);
            zs.push_back(point.z);
        }

        std::sort(xs.begin(), xs.end());
        std::sort(ys.begin(), ys.end());
        std::sort(zs.begin(), zs.end());

        // Use the central 90% of the points for framing.
        const size_t lowerIndex =
            static_cast<size_t>(xs.size() * 0.05);

        const size_t upperIndex =
            static_cast<size_t>(xs.size() * 0.95);

        sceneCenter = cv::Point3f(
            (xs[lowerIndex] + xs[upperIndex]) * 0.5f,
            (ys[lowerIndex] + ys[upperIndex]) * 0.5f,
            (zs[lowerIndex] + zs[upperIndex]) * 0.5f
        );

        sceneRadius = 0.5f * std::max({
            xs[upperIndex] - xs[lowerIndex],
            ys[upperIndex] - ys[lowerIndex],
            zs[upperIndex] - zs[lowerIndex]
        });

        if (sceneRadius < 0.1f)
            sceneRadius = 0.1f;

        cv::Mat pointCloud(points);

        cv::viz::WCloud cloudWidget(
            pointCloud,
            cv::viz::Color::white()
        );
        
        cloudWidget.setRenderingProperty(
            cv::viz::POINT_SIZE,
            1
        );
        
        window.showWidget(
            "MapPoints",
            cloudWidget
        );

        overviewWindow.showWidget(
            "MapPoints",
            cloudWidget
        );

        if (points.size() > 500)
        {
            if (!initialized ||
                lastFittedRadius <= 0.0f ||
                sceneRadius > lastFittedRadius * 1.5f)
            {
                fitView();
                lastFittedRadius = sceneRadius;
            }
        }

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
                4.0
            );
            
            window.showWidget(
                "CameraTrajectory",
                trajectoryWidget
            );

            overviewWindow.showWidget(
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

        overviewWindow.showWidget(
            "CurrentCamera",
            cv::viz::WCameraPosition(
                0.5
            ),
            pose
        );

        initialized = true;
    }


    void SpatialVisualizer::fitView()
    {
        if (sceneRadius <= 0.0f)
            sceneRadius = 1.0f;

        const double distance =
            static_cast<double>(sceneRadius) * 1.5;

        cv::Vec3d center(
            sceneCenter.x,
            sceneCenter.y,
            sceneCenter.z
        );

        // -------------------------------------------------
        // View 1: current front view
        // -------------------------------------------------

        cv::Vec3d cameraPosition(
            center[0],
            center[1],
            center[2] + distance
        );

        cv::Vec3d up(
            0.0,
            1.0,
            0.0
        );

        cv::Affine3d viewerPose =
            cv::viz::makeCameraPose(
                cameraPosition,
                center,
                up
            );

        window.setViewerPose(viewerPose);


        // -------------------------------------------------
        // View 2: elevated oblique overview
        // -------------------------------------------------

        cv::Vec3d overviewCameraPosition(
            center[0] + distance * 1.5,
            center[1] + distance * 1.3,
            center[2] + distance * 1.5
        );

        cv::Affine3d overviewPose =
            cv::viz::makeCameraPose(
                overviewCameraPosition,
                center,
                up
            );

        overviewWindow.setViewerPose(
            overviewPose
        );
    }

    void SpatialVisualizer::show()
    {
        window.spinOnce(
            10,
            true
        );

        overviewWindow.spinOnce(
            10,
            true
        );
    }

    void SpatialVisualizer::saveScreenshot(
        const std::string& filename
    )
    {
        window.saveScreenshot(
            filename
        );
    }
    
    void SpatialVisualizer::saveOverviewScreenshot(
        const std::string& filename
    )
    {
        overviewWindow.saveScreenshot(
            filename
        );
    }

    bool SpatialVisualizer::wasStopped() const
    {
        return
            window.wasStopped() ||
            overviewWindow.wasStopped();
    }
}
