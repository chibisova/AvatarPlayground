#include "processors/KNNMatcherProcessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include "Config.h"

namespace vision
{
    const std::vector<cv::KeyPoint>&
    KNNMatcherProcessor::getMatchedPreviousKeypoints() const
    {
        return matchedPreviousKeypoints;
    }

    const std::vector<cv::KeyPoint>&
    KNNMatcherProcessor::getMatchedCurrentKeypoints() const
    {
        return matchedCurrentKeypoints;
    }

    const std::vector<cv::DMatch>&
    KNNMatcherProcessor::getGoodMatches() const
    {
        return lastGoodMatches;
    }

    cv::Mat KNNMatcherProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::KeyPoint> currentKeypoints;
        cv::Mat currentDescriptors;
        cv::Mat gray;
        cv::Mat output;
        std::vector<cv::DMatch> goodMatches;        
        std::vector<std::vector<cv::DMatch>> knnMatches;

        matchedPreviousKeypoints.clear();
        matchedCurrentKeypoints.clear();
        lastGoodMatches.clear();

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Create matching pairs of keypoints based on the descriptors
        auto orb = cv::ORB::create();
        orb->detectAndCompute(gray, cv::noArray(), currentKeypoints, currentDescriptors);

        
        if (!previousDescriptors.empty())
        {
            // Hamming distance suits ORB's binary descriptors; crossCheck=true keeps only mutual best-matches
            cv::BFMatcher matcher(
                cv::NORM_HAMMING,
                false // Disable crossCheck so KNN can return the two nearest neighbors
            );
            
            // Use KNN matcher to get 2 best matches per descriptor
            matcher.knnMatch(previousDescriptors, currentDescriptors, knnMatches, 2); 


            if (knnMatches.empty())
            {
                output = frame.clone();

                previousFrame = frame.clone();
                previousDescriptors = currentDescriptors;
                previousKeypoints = currentKeypoints;
                
                return output;
            } 

            for (const auto& matches : knnMatches)
            {
                if (matches.size() < 2)
                    continue;

                // Get two best matches
                const cv::DMatch& best = matches[0];
                const cv::DMatch& second = matches[1];

                if (best.distance < Config::LOWE_RATIO * second.distance)
                {
                    goodMatches.push_back(best);
                }
            }

            matchedPreviousKeypoints = previousKeypoints;
            matchedCurrentKeypoints = currentKeypoints;
            lastGoodMatches = goodMatches;

            // Visualize
            cv::drawMatches(previousFrame, previousKeypoints, frame, currentKeypoints, goodMatches, output, cv::Scalar::all(-1));  
        } else {
            output = frame.clone();
        }
        previousFrame = frame.clone();
        previousDescriptors = currentDescriptors;
        previousKeypoints = currentKeypoints;

        return output;
    }
}
