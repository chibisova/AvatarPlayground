#include "processors/RANSACProcessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Config.h"

namespace vision
{
    cv::Mat RANSACProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::KeyPoint> currentKeypoints;
        cv::Mat currentDescriptors;
        cv::Mat gray;
        cv::Mat output;
        std::vector<cv::DMatch> goodMatches;        
        std::vector<std::vector<cv::DMatch>> knnMatches;
        std::vector<cv::Point2f> previousPoints;
        std::vector<cv::Point2f> currentPoints;
        cv::Mat H;
        cv::Mat inlierMask;
        std::vector<cv::DMatch> inlierMatches; // For visualization of RANSAC

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

            for (const auto& match : goodMatches){
                previousPoints.push_back(previousKeypoints[match.queryIdx].pt);
                currentPoints.push_back(currentKeypoints[match.trainIdx].pt);
            }

            H = cv::findHomography(previousPoints, currentPoints, cv::RANSAC, 3, inlierMask); 
            //cv::warpPerspective(previousFrame, output, H, frame.size());

            // Visualize
            for (int i = 0;  i < goodMatches.size(); ++i)
            {
                if (inlierMask.at<uchar>(i) != 0) {
                    inlierMatches.push_back(goodMatches[i]);
                }
            }
            cv::drawMatches(previousFrame, previousKeypoints, frame, currentKeypoints, inlierMatches, output, cv::Scalar::all(-1));  
        } else {
            output = frame.clone();
        }
        previousFrame = frame.clone();
        previousDescriptors = currentDescriptors;
        previousKeypoints = currentKeypoints;

        return output;
    }
}
