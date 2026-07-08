#include "Input.h"
#include "Config.h"


#include <filesystem>
#include <iostream>

bool handleKeyboard(int key, const cv::Mat& frame, const std::string& screenshotsDir, int& numOfScr, ProcessingMode& currentMode) 
{
    if (key == Config::ESC_KEY) { // ESC key
        return false; 
    }
    switch(key){
        case 's':
        case 'S':
            saveScreenshot(frame, screenshotsDir, numOfScr);
            break;
        case '1':
            currentMode = ProcessingMode::Original;
            std::cout << "Original" << std::endl;
            break;
        case '2':
            currentMode = ProcessingMode::Gray;
            std::cout << "Grayscale" << std::endl;
            break;
        case '3':
            std::cout << "Gaussian Blur" << std::endl;
            break;
        case '4':
            std::cout << "Canny Edge Detection" << std::endl;
            break;
        case '5':
            std::cout << "Adaptive Threshold" << std::endl;
            break;
        case '6':
            std::cout << "Corner Detection (Shi-Tomasi)" << std::endl;
            break;
    }
    return true; // Continue running
}

void saveScreenshot(const cv::Mat& frame, const std::string& screenshotsDir, int& numOfScr) 
{
    std::string filename = screenshotsDir + "/screenshot_" + std::to_string(numOfScr) + ".png";
    if (cv::imwrite(filename, frame)) {
        std::cout << "Image saved successfully to " << filename << std::endl;
        numOfScr++;
    } else {
        std::cerr << "Failed to save the image." << std::endl;
    }
}
