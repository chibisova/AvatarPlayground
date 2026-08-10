#pragma once

#include "processors/IImageProcessor.h" // Ensure this file defines the IImageProcessor class or struct

// This processor uses optical flow to track the motion of points between consecutive frames
/* 
    Input: current frame, previous frame, previous corners
    Output: current corners, status of each corner (found or not found)
*/
namespace vision
{
    class OpticalFlowProcessor : public IImageProcessor    // Inherits from IImageProcessor
    {
        private:
            cv::Mat previousFrame;
            std::vector<cv::Point2f> previousCorners;

        public:
            cv::Mat process(const cv::Mat& frame) override;

            const char* name() const override
            { 
                return "MotionTracker"; 
            }
    }; 

}
