#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include "Config.h"
#include "HUD.h"
#include "Input.h"
#include "Timing.h"
#include "ProcessorManager.h"
#include "CameraCalibration.h"
#include "VisualOdometry.h"
#include "processors/MotionEstimationProcessor.h"
#include "processors/TriangulationProcessor.h"
#include "Map3D.h"
#include "MapPoint.h"
#include "processors/PnPProcessor.h"

int main() {

    // Defaults
    int numOfScr = 1; // Counter for screenshots
    ProcessingMode currentMode = ProcessingMode::Original; // Default processing mode
    vision::ProcessorManager processorManager;

    vision::VisualOdometry visualOdometry;
    vision::TriangulationProcessor triangulationProcessor;

    vision::PnPProcessor pnpProcessor;

    vision::Map3D map;

    pnpProcessor.setMap(&map);

    // Open the default camera
    cv::VideoCapture cam(Config::CAMERA_INDEX);

    if (!cam.isOpened()) {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return -1;
    }

    // Create the screenshots directory if it doesn't exist
    std::filesystem::create_directories(Config::SCREENSHOT_DIR);

    cv::Mat frame;

    // Initialize previous time for FPS calculation
    auto previousTime = std::chrono::steady_clock::now();
    double fps = 0.0;

    while (true) {

        // Capture a frame
        cam >> frame;

        cv::flip(frame, frame, 1);

        Config::CAMERA_INTRINSICS = vision::CameraCalibration::createIntrinsicMatrix(frame.cols, frame.rows);

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }

        // Apply image processing based on the current mode
        auto* processor =
        processorManager.getProcessor(currentMode);
    
        cv::Mat processedFrame =
            processor->process(frame);

        if (currentMode == ProcessingMode::MotionEstimation)
        {
            auto* motion = dynamic_cast<vision::MotionEstimationProcessor*>(processor);
        
            if (motion && motion->hasValidPose())
            {
                const auto& R = motion->getRotation();
                const auto& t = motion->getTranslation();

                const auto& previousKeypoints = motion->getPreviousKeypoints();
                const auto& currentKeypoints = motion->getCurrentKeypoints();
                const auto& previousDescriptors = motion->getPreviousDescriptors();
                const auto& goodMatches = motion->getGoodMatches();


                // Visual Odometry
                visualOdometry.update(R, t);

                // Triangulation
                std::vector<vision::MapPoint> newPoints = triangulationProcessor.triangulate(
                    R,
                    t,
                    previousKeypoints,
                    currentKeypoints,
                    previousDescriptors,
                    goodMatches
                );

                map.updateLandmarks(newPoints);

                pnpProcessor.process(frame);

                if (pnpProcessor.hasValidPose())
                {
                    const cv::Mat& rvec = pnpProcessor.getRotationVector();
                    const cv::Mat& tvec = pnpProcessor.getTranslationVector();

                    cv::Mat R;
                    cv::Rodrigues(rvec, R);

                    cv::Mat cameraPosition = -R.t() * tvec;

                    map.addCameraPosition(
                        cv::Point3f(
                            static_cast<float>(cameraPosition.at<double>(0)),
                            static_cast<float>(cameraPosition.at<double>(1)),
                            static_cast<float>(cameraPosition.at<double>(2))
                        )
                    );
                }

                std::cout << "Map size: "
                << map.getPoints().size()
                << '\n';

                if (!map.getPoints().empty())
                {
                    std::cout << "First landmark observations: "
                            << map.getPoints()[0].observations
                            << '\n';
                }
            
                cv::imshow("Trajectory", visualOdometry.getTrajectory());
            }
        }

        // Save the 3D map to a PLY file
        map.savePLY("map_all.ply", 1);
        map.savePLY("map_persistent.ply", 2);
        map.saveCameraTrajectoryPLY("camera_trajectory.ply");

        // Display text on the frame
        drawHUD(processedFrame, fps, processor->name());

        // Display the frame
        cv::imshow("video", processedFrame);

        // Handle keyboard input
        if (!handleKeyboard(cv::waitKey(1), frame, Config::SCREENSHOT_DIR, numOfScr, currentMode))
        {
            break;
        }

        // Calculate FPS
        fps = calculateFPS(previousTime);
    }

    // Release the camera and destroy all OpenCV windows
    cam.release();
    cv::destroyAllWindows();

    return 0;
}
