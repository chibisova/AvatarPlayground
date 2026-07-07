#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // cv:: is used to access OpenCV functions and classes
    // Open the default camera (camera index 0)
    cv::VideoCapture cam(0);

    if (!cam.isOpened()) {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return -1;
    }

    cv::Mat frame;

    // Initialize previous time for FPS calculation
    auto previousTime = std::chrono::steady_clock::now();
    auto fps = 0.0;

    while (true) {
        
        // Capture a frame
        cam >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }

        // Display text on the frame
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        std::string resText = "Resolution: " + std::to_string(frame.cols) + " x " + std::to_string(frame.rows);
        cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, resText, cv::Point(10,60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);


        // Display the frame
        cv::imshow("video", frame);

        // Break the loop if the 'ESC' key is pressed
        if (cv::waitKey(1) == 27) {
            break;
        }

        // Calculate FPS
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
        fps = 1.0 / deltaTime;
        previousTime = currentTime;
    }

    // Release the camera and destroy all OpenCV windows
    cam.release();
    cv::destroyAllWindows();

    return 0;
}
