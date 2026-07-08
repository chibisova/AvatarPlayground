#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include "Config.h"
#include "HUD.h"
#include "Input.h"
#include "Timing.h"


int main() {

    // Constants
    int numOfScr = 1; // Counter for screenshots

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

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }

        // Display text on the frame
        drawHUD(frame, fps);

        // Display the frame
        cv::imshow("video", frame);

        // Handle keyboard input
        if (!handleKeyboard(cv::waitKey(1), frame, Config::SCREENSHOT_DIR, numOfScr))
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
