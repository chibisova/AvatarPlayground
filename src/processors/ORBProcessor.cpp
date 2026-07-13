#include "processors/ORBProcessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

namespace vision
{
    cv::Mat ORBProcessor::process(const cv::Mat& frame)
    {
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        cv::Mat processed;
                
        cv::cvtColor(frame, processed, cv::COLOR_BGR2GRAY);

        cv::Ptr<cv::ORB> orb = cv::ORB::create();
        // shorter option:
        // auto orb = cv::ORB::create();
        orb->detectAndCompute(processed, cv::noArray(), keypoints, descriptors);

        cv::drawKeypoints(frame, keypoints, processed, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

        return processed;
    }
}
