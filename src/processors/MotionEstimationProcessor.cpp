#include "processors/MotionEstimationProcessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Config.h"

namespace vision
{
    const cv::Mat& MotionEstimationProcessor::getRotation() const
    {
        return relativeRotation;
    }

    const cv::Mat& MotionEstimationProcessor::getTranslation() const
    {
        return relativeTranslation;
    }

    bool MotionEstimationProcessor::hasValidPose() const
    {
        return poseValid;
    }

    cv::Mat MotionEstimationProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::KeyPoint> currentKeypoints;
        cv::Mat currentDescriptors;
        cv::Mat gray;
        cv::Mat output;
        std::vector<cv::DMatch> goodMatches;        
        std::vector<std::vector<cv::DMatch>> knnMatches;
        std::vector<cv::Point2f> previousPoints;
        std::vector<cv::Point2f> currentPoints;
        std::vector<cv::DMatch> inlierMatches; // For visualization of RANSAC
        const cv::Mat& K = Config::CAMERA_INTRINSICS; // Temperal Camera calibration approximation
        cv::Mat E;
        cv::Mat essentialMask;
        

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Create matching pairs of keypoints based on the descriptors
        auto orb = cv::ORB::create();
        orb->detectAndCompute(gray, cv::noArray(), currentKeypoints, currentDescriptors);

        
        if (!previousDescriptors.empty() && !currentDescriptors.empty())
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

                poseValid = false;
                
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

            // Compute Essential Matrix
            if (previousPoints.size() < 5) 
            {
                output = frame.clone();

                previousFrame = frame.clone();
                previousDescriptors = currentDescriptors;
                previousKeypoints = currentKeypoints;

                poseValid = false;

                return output;
            }

            E = cv::findEssentialMat(previousPoints, currentPoints, K, cv::RANSAC, 0.999, 1.0, essentialMask);
            std::cout
            << "E size: "
            << E.rows
            << " x "
            << E.cols
            << std::endl;

            std::cout << "Good matches: "
            << goodMatches.size()
            << '\n';

            if (E.empty()){
                // Estimation failed 
                std::cout << "No E";
                
                output = frame.clone();

                previousFrame = frame.clone();
                previousDescriptors = currentDescriptors;
                previousKeypoints = currentKeypoints;

                poseValid = false;

                return output;
            } 

            if (E.cols > 3)
            {
                E = E.colRange(0,3).clone();
            }

            // Get Camera Rotation and Translation matrices
            // inliers - how many matches were geometrically consistent
            int inliers = cv::recoverPose(E, previousPoints, currentPoints, K, relativeRotation, relativeTranslation); 
            
            // Check whether the pose recovery was successful
            double ratio = static_cast<double>(inliers) / goodMatches.size();

            if (ratio < 0.25)
            {
                output = frame.clone();

                previousFrame = frame.clone();
                previousDescriptors = currentDescriptors;
                previousKeypoints = currentKeypoints;
                poseValid = false;
                return output;
            }

            poseValid = true;
            
            std::cout << "Inliers: " << inliers << '\n';
            std::cout << "Relative rotation:\n" << relativeRotation << std::endl;
            std::cout << "Relative Translation:\n" << relativeTranslation << std::endl;            

            for (int i = 0; i < goodMatches.size(); ++i)
            {
                if (essentialMask.at<uchar>(i))
                    inlierMatches.push_back(goodMatches[i]);
            }
            cv::drawMatches(previousFrame, previousKeypoints, frame, currentKeypoints, inlierMatches, output);
        } else {
            
            output = frame.clone();

            poseValid = false;
        }
        previousFrame = frame.clone();
        previousDescriptors = currentDescriptors;
        previousKeypoints = currentKeypoints;

        return output;
        
    }
}
