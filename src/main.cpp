#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Open the default camera (camera index 0)
    cv::VideoCapture cam(0);

    if (!cam.isOpened()) {
        std::cerr << "Error: Could not open the camera." << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        // Capture a frame
        cam >> frame;

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }

        // Display the frame
        cv::imshow("video", frame);

        // Break the loop if the 'ESC' key is pressed
        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    // Release the camera and destroy all OpenCV windows
    cam.release();
    cv::destroyAllWindows();

    return 0;
}
