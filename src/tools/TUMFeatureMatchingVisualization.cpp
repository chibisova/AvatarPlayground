#include <opencv2/opencv.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#include "processors/KNNMatcherProcessor.h"

namespace
{
    constexpr const char* DATASET_PATH =
        "datasets/tum/rgbd_dataset_freiburg1_xyz/";
    constexpr const char* FIRST_FRAME = "rgb/1305031102.175304.png";
    constexpr const char* SECOND_FRAME = "rgb/1305031102.343233.png";
    constexpr size_t TOP_MATCH_COUNT = 50;
    constexpr int OUTPUT_WIDTH = 2560;
}

int main()
{
    const cv::Mat firstFrame = cv::imread(
        std::string(DATASET_PATH) + FIRST_FRAME,
        cv::IMREAD_COLOR
    );
    const cv::Mat secondFrame = cv::imread(
        std::string(DATASET_PATH) + SECOND_FRAME,
        cv::IMREAD_COLOR
    );

    if (firstFrame.empty() || secondFrame.empty())
    {
        std::cerr << "Could not load the selected TUM frames.\n";
        return 1;
    }

    // Reuse the project's ORB + KNN BFMatcher + Lowe-ratio pipeline.
    vision::KNNMatcherProcessor matcher;
    matcher.process(firstFrame);
    matcher.process(secondFrame);

    std::vector<cv::DMatch> topMatches = matcher.getGoodMatches();

    std::sort(
        topMatches.begin(),
        topMatches.end(),
        [](const cv::DMatch& left, const cv::DMatch& right)
        {
            return left.distance < right.distance;
        }
    );

    if (topMatches.empty())
    {
        std::cerr << "No Lowe-ratio matches were found.\n";
        return 1;
    }

    topMatches.resize(std::min(TOP_MATCH_COUNT, topMatches.size()));

    cv::Mat visualization;
    cv::drawMatches(
        firstFrame,
        matcher.getMatchedPreviousKeypoints(),
        secondFrame,
        matcher.getMatchedCurrentKeypoints(),
        topMatches,
        visualization,
        cv::Scalar::all(-1),
        cv::Scalar::all(-1),
        std::vector<char>(),
        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
    );

    const double scale =
        static_cast<double>(OUTPUT_WIDTH) / visualization.cols;

    cv::resize(
        visualization,
        visualization,
        cv::Size(
            OUTPUT_WIDTH,
            cvRound(visualization.rows * scale)
        ),
        0.0,
        0.0,
        cv::INTER_CUBIC
    );

    std::filesystem::create_directories("output");

    if (!cv::imwrite("output/feature_matching_tum.png", visualization))
    {
        std::cerr << "Could not write output/feature_matching_tum.png\n";
        return 1;
    }

    std::cout << "Saved output/feature_matching_tum.png\n";
    return 0;
}
