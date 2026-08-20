#include "VisualOdometry.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

#include "Config.h"

namespace vision
{
    const cv::Mat& VisualOdometry::getTrajectory() const
    {
        return trajectory;
    }

    const cv::Mat& vision::VisualOdometry::getGlobalPose() const
    {
        return globalPose;
    }
    
    void VisualOdometry::update(const cv::Mat& R,
                                const cv::Mat& t)
    {
        cv::Mat relativePose = cv::Mat::eye(4,4,CV_64F);

        R.copyTo(relativePose(cv::Rect(0, 0, 3, 3)));
        t.copyTo(relativePose(cv::Rect(3, 0, 1, 3)));

        globalPose = globalPose * relativePose;

        double tx = globalPose.at<double>(0, 3);
        double tz = globalPose.at<double>(2, 3);

        int drawX = centerX + scale * tx;
        int drawY = centerY - scale * tz;
        cv::Point currentPoint(drawX, drawY);
        
        if (firstPose)
        {
            previousPoint = currentPoint;
            firstPose = false;

            cv::circle(
                trajectory,
                currentPoint,
                2,
                cv::Scalar(0,255,0),
                cv::FILLED
            );
        }
        else
        {
            cv::line(trajectory,
                    previousPoint,
                    currentPoint,
                    cv::Scalar(0,255,0),
                    1);
            
            cv::circle(
                    trajectory,
                    currentPoint,
                    2,
                    cv::Scalar(0,255,0),
                    cv::FILLED);

            previousPoint = currentPoint;
        }
    }
}
