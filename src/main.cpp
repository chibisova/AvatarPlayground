#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include "Config.h"
#include "HUD.h"
#include "Input.h"
#include "Timing.h"
#include "ProcessorManager.h"



int main() {

    // Defaults
    int numOfScr = 1; // Counter for screenshots
    ProcessingMode currentMode = ProcessingMode::Original; // Default processing mode
    vision::ProcessorManager processorManager;

    // Open the default camera (camera index 0)
    cv::VideoCapture cam(0);

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

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }

        // Apply image processing based on the current mode
        cv::Mat processedFrame = processorManager
                                    .getProcessor(currentMode)
                                    ->process(frame);

        // Display text on the frame
        drawHUD(processedFrame, fps, currentMode);

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
