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


int main() {

    // Defaults
    int numOfScr = 1; // Counter for screenshots
    ProcessingMode currentMode = ProcessingMode::Original; // Default processing mode
    vision::ProcessorManager processorManager;

    vision::VisualOdometry visualOdometry;
    vision::TriangulationProcessor triangulationProcessor;

    vision::Map3D map;

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

                const auto& previousPoints =
                    motion->getPreviousPoints();

                const auto& currentPoints =
                    motion->getCurrentPoints();

                // Visual Odometry
                visualOdometry.update(R, t);

                // Triangulation
                std::vector<cv::Point3f> newPoints = triangulationProcessor.triangulate(
                    R,
                    t,
                    previousPoints,
                    currentPoints
                );

                map.addPoints(newPoints);

                std::cout << "New points: "
                        << newPoints.size() << std::endl;

                std::cout << "Map size: "
                        << map.getPoints().size() << std::endl;
                
                std::cout << "Triangulated points: "
                        << triangulationProcessor
                                .getPoints3D()
                                .rows
                        << std::endl;
            
                cv::imshow("Trajectory", visualOdometry.getTrajectory());
            }
        }

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
