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
            break;
        case '2':
            currentMode = ProcessingMode::Gray;
            break;
        case '3':
            currentMode = ProcessingMode::Blur;
            break;
        case '4':
            currentMode = ProcessingMode::Canny;
            break;
        case '5':
            currentMode = ProcessingMode::Threshold;
            break;
        case '6':
            currentMode = ProcessingMode::ShiTomasiCorners;
            break;
        case '7':
            currentMode = ProcessingMode::HarrisCorners;
            break;
        case '8':
            currentMode = ProcessingMode::OpticalFlow;
            break;
        case '9':
            currentMode = ProcessingMode::ORB;
            break;
        case '0':
            currentMode = ProcessingMode::BFMatcher;
            break;
        case 'k':
        case 'K':
            currentMode = ProcessingMode::KNNMatcher;
            break;
        case 'r':
        case 'R':
            currentMode = ProcessingMode::RANSAC;
        case 'm':
        case 'M':
            currentMode = ProcessingMode::MotionEstimation;
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
