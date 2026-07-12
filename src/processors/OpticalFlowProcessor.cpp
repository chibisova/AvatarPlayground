#include "processors/OpticalFlowProcessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "Config.h"


namespace vision
{
    cv::Mat OpticalFlowProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::Point2f> currentCorners;
        std::vector<uchar> status;
        std::vector<float> err;
        cv::Mat currentFrame;
        cv::Mat output = frame.clone();
        std::vector<cv::Point2f> trackedPreviousCorners;
        std::vector<cv::Point2f> newCorners;

        
        cv::cvtColor(frame, currentFrame, cv::COLOR_BGR2GRAY);
        
        cv::GaussianBlur(currentFrame, currentFrame, cv::Size(5, 5), 0);

        if (previousFrame.empty()){
            cv::goodFeaturesToTrack(currentFrame, currentCorners, 200, 0.01, 10);
            previousFrame = currentFrame;
            previousCorners = currentCorners;
            return output;
        }
        else {
            cv::calcOpticalFlowPyrLK(previousFrame, currentFrame, previousCorners, currentCorners, status, err, cv::Size(21, 21), 3);
            
            // Draw motion vectors
            for (size_t i = 0; i < previousCorners.size(); i++)
            {
                if (status[i])
                {
                    cv::line(output, previousCorners[i], currentCorners[i], cv::Scalar(0, 255, 0), 1);
                    cv::circle(output, currentCorners[i], 5, cv::Scalar(0, 255, 0), -1);

                    trackedPreviousCorners.push_back(currentCorners[i]);

                }
            }

            // Replace previousCorners with tracked points
            previousCorners = trackedPreviousCorners;

            // Does it have enough tracked points? -> Detect aditional points
            if (previousCorners.size() < Config::MIN_TRACKED_POINTS)
            {
                cv::goodFeaturesToTrack(currentFrame, newCorners, Config::MAX_TRACKED_POINTS - previousCorners.size(), 0.01, 10);
                previousCorners.insert(
                    previousCorners.end(),
                    newCorners.begin(),
                    newCorners.end());
            }

            previousFrame = currentFrame;
            return output;
        }
    }
}
