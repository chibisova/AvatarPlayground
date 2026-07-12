#include "processors/BFMatcherProcessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

namespace vision
{
    cv::Mat BFMatcherProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::KeyPoint> currentKeypoints;
        cv::Mat currentDescriptors;
        cv::Mat gray;
        cv::Mat output;
        std::vector<cv::DMatch> matches;        

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Create matching pairs of keypoints based on the descriptors
        auto orb = cv::ORB::create();
        orb->detectAndCompute(gray, cv::noArray(), currentKeypoints, currentDescriptors);

        if (!previousDescriptors.empty())
        {
            // Hamming distance suits ORB's binary descriptors; crossCheck=true keeps only mutual best-matches
            cv::BFMatcher matcher(
                cv::NORM_HAMMING,
                true
            );
            
            // Create matching pairs of keypoints based on the descriptors
            matcher.match(previousDescriptors, currentDescriptors, matches, cv::noArray());
            
            // Visualize
            cv::drawMatches(previousFrame, previousKeypoints, frame, currentKeypoints, matches, output, cv::Scalar::all(-1));
        } else {
            output = frame.clone();
        }
        
        previousFrame = frame.clone();
        previousDescriptors = currentDescriptors;
        previousKeypoints = currentKeypoints;

        return output;
    }
}
