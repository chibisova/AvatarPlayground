#include "Input.h"
#include "Config.h"

#include <filesystem>
#include <iostream>

bool handleKeyboard(int key, const cv::Mat& frame, const std::string& screenshotsDir, int& numOfScr) 
{
    if (key == Config::ESC_KEY) { // ESC key
        return false; 
    }
    switch(key){
        case 's':
        case 'S':
            saveScreenshot(frame, screenshotsDir, numOfScr);
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
