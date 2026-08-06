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

    const std::vector<cv::Point2f>& MotionEstimationProcessor::getPreviousPoints() const
    {
        return previousPoints;
    }
    
    const std::vector<cv::Point2f>& MotionEstimationProcessor::getCurrentPoints() const
    {
        return currentPoints;
    }

    bool MotionEstimationProcessor::hasValidPose() const
    {
        return poseValid;
    }

    const std::vector<cv::DMatch>& MotionEstimationProcessor::getGoodMatches() const
    {
        std::cout << "getter this = " << this << '\n';
        return goodMatches;
    }

    const std::vector<cv::KeyPoint>& MotionEstimationProcessor::getPreviousKeypoints() const
    {
        return frozenPreviousKeypoints;
    }

    const std::vector<cv::KeyPoint>& MotionEstimationProcessor::getCurrentKeypoints() const
    {
        return currentKeypoints;
    }

    const cv::Mat& MotionEstimationProcessor::getPreviousDescriptors() const
    {
        std::cout << "getter this = " << this << '\n';

        return frozenPreviousDescriptors;
    }

    const cv::Mat& MotionEstimationProcessor::getCurrentDescriptors() const
    {
        return currentDescriptors;
    }

    void MotionEstimationProcessor::advanceFrame(const cv::Mat& frame)
    {
        previousFrame = frame.clone();
        previousKeypoints = currentKeypoints;
        previousDescriptors = currentDescriptors.clone();
    }

    cv::Mat MotionEstimationProcessor::process(const cv::Mat& frame)
    {
        std::cout << "this = " << this << '\n';
        cv::Mat gray;
        cv::Mat output;
        std::vector<std::vector<cv::DMatch>> knnMatches;
        std::vector<cv::DMatch> inlierMatches; // For visualization of RANSAC
        const cv::Mat& K = Config::CAMERA_INTRINSICS; // Temperal Camera calibration approximation
        cv::Mat E;
        cv::Mat essentialMask;
        
        poseValid = false;

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Clear previous data for the current frame
        currentKeypoints.clear();
        currentDescriptors.release();
        goodMatches.clear();
        goodMatches.clear();
        knnMatches.clear();
        previousPoints.clear();
        currentPoints.clear();
        inlierMatches.clear();

        // Create matching pairs of keypoints based on the descriptors
        auto orb = cv::ORB::create();
        orb->detectAndCompute(gray, cv::noArray(), currentKeypoints, currentDescriptors);

        // Debugging: Ensure the number of keypoints matches the number of descriptors
        assert(currentKeypoints.size() == static_cast<size_t>(currentDescriptors.rows));
        
        if (!previousDescriptors.empty() && !currentDescriptors.empty())
        {
            // Hamming distance suits ORB's binary descriptors; crossCheck=true keeps only mutual best-matches
            cv::BFMatcher matcher(
                cv::NORM_HAMMING,
                false // Disable crossCheck so KNN can return the two nearest neighbors
            );
            // Debugging: Ensure the number of keypoints matches the number of descriptors
            //assert(previousKeypoints.size() == static_cast<size_t>(previousDescriptors.rows));
            std::cout << "previousKeypoints.size(): "
                    << previousKeypoints.size() << '\n';

            std::cout << "previousDescriptors.rows: "
                    << previousDescriptors.rows << '\n';

            std::cout << "currentKeypoints.size(): "
                    << currentKeypoints.size() << '\n';

            std::cout << "currentDescriptors.rows: "
                    << currentDescriptors.rows << '\n';
            // Use KNN matcher to get 2 best matches per descriptor
            matcher.knnMatch(previousDescriptors, currentDescriptors, knnMatches, 2); 

            int maxQuery = -1;

            for (const auto& pair : knnMatches)
            {
                for (const auto& m : pair)
                    maxQuery = std::max(maxQuery, m.queryIdx);
            }

            std::cout << "After knnMatch\n";
            std::cout << "rows = " << previousDescriptors.rows << '\n';
            std::cout << "max query = " << maxQuery << '\n';


            if (knnMatches.empty())
            {
                output = frame.clone();

                advanceFrame(frame);

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
            maxQuery = -1;

            for (const auto& m : goodMatches)
                maxQuery = std::max(maxQuery, m.queryIdx);

            std::cout << "After Lowe\n";
            std::cout << "rows = " << previousDescriptors.rows << '\n';
            std::cout << "max query = " << maxQuery << '\n';

            previousPoints.clear();
            currentPoints.clear();

            for (const auto& match : goodMatches){
                if (match.queryIdx >= previousKeypoints.size())
                {
                    std::cout << "BAD queryIdx "
                            << match.queryIdx
                            << " / "
                            << previousKeypoints.size()
                            << '\n';
                    continue;
                }

                if (match.trainIdx >= currentKeypoints.size())
                {
                    std::cout << "BAD trainIdx "
                            << match.trainIdx
                            << " / "
                            << currentKeypoints.size()
                            << '\n';
                    continue;
                }
                previousPoints.push_back(previousKeypoints[match.queryIdx].pt);
                currentPoints.push_back(currentKeypoints[match.trainIdx].pt);
            }

            // Compute Essential Matrix
            if (previousPoints.size() < 5) 
            {
                output = frame.clone();

                advanceFrame(frame);

                poseValid = false;

                return output;
            }

            E = cv::findEssentialMat(previousPoints, currentPoints, K, cv::RANSAC, 0.999, 1.0, essentialMask);
            std::cout << "E size: " << E.rows << " x " << E.cols << '\n';


            if (E.empty()){
                // Estimation failed 
                std::cout << "No E";
                
                output = frame.clone();

                advanceFrame(frame);

                poseValid = false;

                return output;
            } 

            if (E.cols > 3)
            {
                std::cout << "Multiple Essential Matrices returned.\n";

                E = E.rowRange(0,3).clone();
            }

            if (E.rows != 3 || E.cols != 3)
            {
                std::cerr << "Invalid Essential Matrix size: "
                        << E.rows
                        << " x "
                        << E.cols
                        << '\n';

                poseValid = false;

                advanceFrame(frame);

                return frame.clone();
            }

            // Get Camera Rotation and Translation matrices
            // inliers - how many matches were geometrically consistent
            int inliers = cv::recoverPose(E, previousPoints, currentPoints, K, relativeRotation, relativeTranslation); 
            
            
            // Check whether the pose recovery was successful
            double ratio = static_cast<double>(inliers) / goodMatches.size();

            if (ratio < 0.25)
            {
                output = frame.clone();

                advanceFrame(frame);

                poseValid = false;
                return output;
            }

            poseValid = true;
            inlierMatches.clear();

            for (int i = 0; i < goodMatches.size(); ++i)
            {
                if (essentialMask.at<uchar>(i))
                    inlierMatches.push_back(goodMatches[i]);
            }
            cv::drawMatches(previousFrame, previousKeypoints, frame, currentKeypoints, inlierMatches, output);

            // Snapshot the previous keypoints/descriptors goodMatches was computed
            // against, before advanceFrame() overwrites them below.
            frozenPreviousKeypoints = previousKeypoints;
            frozenPreviousDescriptors = previousDescriptors.clone();
        } else {
            
            output = frame.clone();

            poseValid = false;
        }
        
        advanceFrame(frame);

        std::cout << "Leaving MotionEstimation\n";
        std::cout << previousDescriptors.rows << '\n';

        if (!goodMatches.empty())
        {
            std::cout
                << "largest queryIdx = "
                << std::max_element(
                    goodMatches.begin(),
                    goodMatches.end(),
                    [](const auto& a, const auto& b)
                    {
                        return a.queryIdx < b.queryIdx;
                    })->queryIdx
                << '\n';
        }

        return output;
        
    }
}
