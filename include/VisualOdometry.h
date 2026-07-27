#pragma once

#include <opencv2/core.hpp>

// Estimate how camera moves over time
namespace vision
{
    class VisualOdometry
    {
        private:
            cv::Mat globalPose = cv::Mat::eye(4,4,CV_64F);
            cv::Mat trajectory = cv::Mat::zeros(800,800,CV_8UC3);
            const int centerX = 400;
            const int centerY = 400;
            const double scale = 50.0;

            cv::Point previousPoint;
            bool firstPose = true;

        public:
            void update(const cv::Mat& R,
                        const cv::Mat& t);

            const cv::Mat& getTrajectory() const;
    };
}
