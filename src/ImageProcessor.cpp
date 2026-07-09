#include "ImageProcessor.h"
#include "Config.h"

cv::Mat processFrame(const cv::Mat& frame, ProcessingMode mode)
{
    switch (mode)
    {
        case ProcessingMode::Original:
            return frame.clone();
        
        case ProcessingMode::Gray:
        {
            cv::Mat grayFrame;
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            return grayFrame;
        }

        case ProcessingMode::Blur:
        {
            cv::Mat blurFrame;
            cv::GaussianBlur(frame, blurFrame, cv::Size(5, 5), 0);
            return blurFrame;
        }

        case ProcessingMode::Canny:
        {
            cv::Mat cannyFrame; 
            cv::cvtColor(frame, cannyFrame, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(cannyFrame, cannyFrame, cv::Size(5, 5), 0);
            cv::Canny(cannyFrame, cannyFrame, 100, 150);
            return cannyFrame;

        }

        case ProcessingMode::Threshold:
        {
            cv::Mat threshFrame;
            cv::cvtColor(frame, threshFrame, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(threshFrame, threshFrame, cv::Size(5, 5), 0);
            cv::adaptiveThreshold(threshFrame, threshFrame, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 2);
            return threshFrame;
        }

        case ProcessingMode::ShiTomasiCorners:
        {
            std::vector<cv::Point2f> cornersFrame;
            cv::Mat grayFrame;
            cv::Mat output = frame.clone();
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);
            cv::goodFeaturesToTrack(grayFrame, cornersFrame, 200, 0.01, 10);
            for (const cv::Point2f& corner : cornersFrame)
            {
                cv::circle(output, corner, 5, cv::Scalar(0, 255, 0), -1);
            }
            return output;
        }

        case ProcessingMode::HarrisCorners:
        {
            cv::Mat grayFrame;
            cv::Mat harrisResponse;
            cv::Mat normalizedResponse;
            cv::Mat outputFrame = frame.clone();
            
            cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
            cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);
            cv::cornerHarris(grayFrame, harrisResponse, 5, 3, 0.04);
            
            // Normalize the Harris response for visualization
            cv::normalize(
                harrisResponse,
                normalizedResponse,
                0,
                255,
                cv::NORM_MINMAX,
                CV_8U);

            // Draw circles on the corners detected by Harris
            for (int y = 0; y < normalizedResponse.rows; ++y)
            {
                for (int x = 0; x < normalizedResponse.cols; ++x)
                {
                    if (normalizedResponse.at<uchar>(y, x) > Config::HARRIS_THRESHOLD)
                    {
                        cv::circle(outputFrame, cv::Point(x, y), 5, cv::Scalar(0,255,0), -1);
                    }
                }
            }
            
            return outputFrame;
        }

        default:
            return frame.clone();
    }
    
}

std::string processingModeToString(ProcessingMode mode)
{
    switch(mode)
    {
        case ProcessingMode::Original:
            return "Original";

        case ProcessingMode::Gray:
            return "Grayscale";

        case ProcessingMode::Blur:
            return "Blur";

        case ProcessingMode::Canny:
            return "Canny";

        case ProcessingMode::Threshold:
            return "Threshold";

        case ProcessingMode::ShiTomasiCorners:
            return "Shi-Tomasi Corners";
        
        case ProcessingMode::HarrisCorners:
            return "Harris Corners";
    }

    return "Unknown";
}
