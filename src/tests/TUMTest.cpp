#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>

#include "Config.h"
#include "processors/MotionEstimationProcessor.h"

constexpr int FRAME_STEP = 5;
constexpr double VISUALIZATION_SCALE = 0.05;

struct TUMFrame
{
    double timestamp;
    std::string filename;
};

struct TUMGroundTruth
{
    double timestamp;

    cv::Vec3d position;
    cv::Vec4d orientation; // qx, qy, qz, qw
};

struct TrajectorySample
{
    double timestamp;

    cv::Vec3d estimatedPosition;
    cv::Vec3d groundTruthPosition;
};


double rotationErrorDegrees(
    const cv::Mat& estimatedRotation,
    const cv::Mat& groundTruthRotation
)
{
    const cv::Mat rotationError =
        estimatedRotation * groundTruthRotation.t();

    const double cosine = std::clamp(
        (cv::trace(rotationError)[0] - 1.0) / 2.0,
        -1.0,
        1.0
    );

    return std::acos(cosine) * 180.0 / CV_PI;
}


bool loadRGBList(
    const std::string& filename,
    std::vector<TUMFrame>& frames
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Could not open: "
                  << filename << '\n';

        return false;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);

        TUMFrame frame;

        ss >> frame.timestamp
           >> frame.filename;

        if (!ss.fail())
            frames.push_back(frame);
    }

    return !frames.empty();
}


bool loadGroundTruth(
    const std::string& filename,
    std::vector<TUMGroundTruth>& groundTruth
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Could not open: "
                  << filename
                  << '\n';

        return false;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);

        TUMGroundTruth gt;

        ss >> gt.timestamp
           >> gt.position[0]
           >> gt.position[1]
           >> gt.position[2]
           >> gt.orientation[0]
           >> gt.orientation[1]
           >> gt.orientation[2]
           >> gt.orientation[3];

        if (!ss.fail())
            groundTruth.push_back(gt);
    }

    return !groundTruth.empty();
}


const TUMGroundTruth* findClosestGroundTruth(
    double timestamp,
    const std::vector<TUMGroundTruth>& groundTruth
)
{
    if (groundTruth.empty())
        return nullptr;

    const TUMGroundTruth* closest =
        &groundTruth[0];

    double smallestDifference =
        std::abs(
            timestamp - closest->timestamp
        );

    for (const auto& gt : groundTruth)
    {
        double difference =
            std::abs(
                timestamp - gt.timestamp
            );

        if (difference < smallestDifference)
        {
            smallestDifference = difference;
            closest = &gt;
        }
    }

    return closest;
}


cv::Mat quaternionToRotation(const cv::Vec4d& q)
{
    double x = q[0];
    double y = q[1];
    double z = q[2];
    double w = q[3];

    cv::Mat R =
        (cv::Mat_<double>(3, 3) <<
            1 - 2*y*y - 2*z*z,
            2*x*y - 2*z*w,
            2*x*z + 2*y*w,

            2*x*y + 2*z*w,
            1 - 2*x*x - 2*z*z,
            2*y*z - 2*x*w,

            2*x*z - 2*y*w,
            2*y*z + 2*x*w,
            1 - 2*x*x - 2*y*y
        );

    return R;
}


int main()
{
    std::cout
        << "===== TUM RGB-D VO Test =====\n";

    std::cout
        << "Frame step: "
        << FRAME_STEP
        << "\n";


    const std::string datasetPath =
        "datasets/tum/rgbd_dataset_freiburg1_xyz/";


    const std::string rgbListPath =
        datasetPath + "rgb.txt";

    const std::string groundTruthPath =
        datasetPath + "groundtruth.txt";


    // ---------------------------------------------------------
    // Load RGB frame list
    // ---------------------------------------------------------

    std::vector<TUMFrame> frames;

    if (!loadRGBList(
            rgbListPath,
            frames))
    {
        return -1;
    }

    std::cout
        << "Loaded RGB frames: "
        << frames.size()
        << '\n';


    // ---------------------------------------------------------
    // Load ground truth
    // ---------------------------------------------------------

    std::vector<TUMGroundTruth> groundTruth;

    if (!loadGroundTruth(
            groundTruthPath,
            groundTruth))
    {
        return -1;
    }

    std::cout
        << "Loaded ground-truth poses: "
        << groundTruth.size()
        << '\n';


    // ---------------------------------------------------------
    // TUM Freiburg 1 RGB intrinsics
    // ---------------------------------------------------------

    Config::CAMERA_INTRINSICS =
        (cv::Mat_<double>(3, 3) <<
            517.3, 0.0, 318.6,
            0.0, 516.5, 255.3,
            0.0, 0.0, 1.0
        );


    // ---------------------------------------------------------
    // TUM Freiburg 1 RGB distortion
    // ---------------------------------------------------------

    cv::Mat distortionCoefficients =
        (cv::Mat_<double>(1, 5) <<
            0.2624,
            -0.9531,
            -0.0054,
            0.0026,
            1.1633
        );


    // ---------------------------------------------------------
    // Create existing VO processor
    // ---------------------------------------------------------

    vision::MotionEstimationProcessor motionEstimation;


    // Current global camera pose.
    //
    // We are intentionally keeping the existing
    // accumulation implementation unchanged for this test.

    cv::Mat worldPose =
        cv::Mat::eye(
            4,
            4,
            CV_64F
        );


    // ---------------------------------------------------------
    // Trajectory storage
    // ---------------------------------------------------------

    std::vector<TrajectorySample>
        trajectorySamples;


    int successfulPoses = 0;
    int evaluatedPairs = 0;
    std::vector<double> rotationErrorsDegrees;
    const TUMGroundTruth* previousGroundTruth = nullptr;
    const TUMGroundTruth* anchorGroundTruth = nullptr;

    std::ofstream trajectoryLog("logs/tum_trajectory.csv");

    if (!trajectoryLog.is_open())
    {
        std::cerr << "Could not write: logs/tum_trajectory.csv\n";
        return -1;
    }

    trajectoryLog
        << "frame,timestamp,status,estimated_x,estimated_y,estimated_z,"
        << "ground_truth_x,ground_truth_y,ground_truth_z,"
        << "pose_inliers,median_parallax_px,rotation_error_deg\n";




    // ---------------------------------------------------------
    // Process TUM frames
    // ---------------------------------------------------------

    for (size_t i = 0; i < frames.size(); i += FRAME_STEP)
    {
        std::string imagePath =
            datasetPath +
            frames[i].filename;


        cv::Mat frame =
            cv::imread(
                imagePath,
                cv::IMREAD_COLOR
            );


        if (frame.empty())
        {
            std::cerr
                << "Could not load image: "
                << imagePath
                << '\n';

            return -1;
        }


        // -----------------------------------------------------
        // Undistort TUM RGB image
        // -----------------------------------------------------

        cv::Mat undistortedFrame;

        cv::undistort(
            frame,
            undistortedFrame,
            Config::CAMERA_INTRINSICS,
            distortionCoefficients
        );


        std::cout
            << "Processing frame "
            << i
            << ": "
            << frames[i].filename
            << '\n';


        // -----------------------------------------------------
        // Find closest ground-truth pose
        // -----------------------------------------------------

        const TUMGroundTruth* gt =
            findClosestGroundTruth(
                frames[i].timestamp,
                groundTruth
            );


        // -----------------------------------------------------
        // Run VO
        // -----------------------------------------------------

        motionEstimation.process(
            undistortedFrame
        );

        const bool poseValid = motionEstimation.hasValidPose();
        cv::Vec3d estimatedPosition(
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN()
        );
        double rotationError = std::numeric_limits<double>::quiet_NaN();

        if (previousGroundTruth != nullptr)
        {
            ++evaluatedPairs;

            std::cout
                << "Pose diagnostics | "
                << (poseValid ? "accepted" : "rejected")
                << " | candidates: "
                << motionEstimation.getCandidatePointCount()
                << " | essential inliers: "
                << motionEstimation.getEssentialInlierCount()
                << " | pose inliers: "
                << motionEstimation.getPoseInlierCount()
                << " | median parallax (px): "
                << motionEstimation.getMedianParallaxPixels()
                << '\n';
        }


        // -----------------------------------------------------
        // Check VO pose
        // -----------------------------------------------------

        if (
            poseValid
        )
        {
            successfulPoses++;

            std::cout
                << "Pose succeeded\n";


            const cv::Mat& R =
                motionEstimation.getRotation();


            const cv::Mat& t =
                motionEstimation.getTranslation();

            if (anchorGroundTruth != nullptr && gt != nullptr)
            {
                const cv::Mat groundTruthRelativeRotation =
                    quaternionToRotation(gt->orientation).t() *
                    quaternionToRotation(anchorGroundTruth->orientation);

                rotationError = rotationErrorDegrees(
                    R,
                    groundTruthRelativeRotation
                );

                rotationErrorsDegrees.push_back(rotationError);

                std::cout
                    << "Ground-truth rotation error (deg): "
                    << rotationError
                    << '\n';
            }



            // -------------------------------------------------
            // Existing pose accumulation
            //
            // DO NOT CHANGE THIS YET.
            // -------------------------------------------------

            cv::Mat RInverse =
                R.t();


            cv::Mat scaledT =
                t * VISUALIZATION_SCALE;
            
            cv::Mat tInverse =
                -RInverse * scaledT;


            cv::Mat relativeCameraPose =
                cv::Mat::eye(
                    4,
                    4,
                    CV_64F
                );


            RInverse.copyTo(
                relativeCameraPose(
                    cv::Rect(
                        0,
                        0,
                        3,
                        3
                    )
                )
            );


            tInverse.copyTo(
                relativeCameraPose(
                    cv::Rect(
                        3,
                        0,
                        1,
                        3
                    )
                )
            );


            worldPose =
                worldPose *
                relativeCameraPose;


            std::cout
                << "Camera position: "
                << worldPose.at<double>(0, 3)
                << ", "
                << worldPose.at<double>(1, 3)
                << ", "
                << worldPose.at<double>(2, 3)
                << '\n';


            // -------------------------------------------------
            // Store trajectory sample
            //
            // Only successful VO poses are stored.
            // -------------------------------------------------

            if (gt != nullptr)
            {
                TrajectorySample sample;

                sample.timestamp =
                    frames[i].timestamp;


                sample.estimatedPosition =
                    cv::Vec3d(
                        worldPose.at<double>(0, 3),
                        worldPose.at<double>(1, 3),
                        worldPose.at<double>(2, 3)
                    );

                estimatedPosition = sample.estimatedPosition;


                sample.groundTruthPosition =
                    gt->position;


                trajectorySamples.push_back(
                    sample
                );
            }


            if (i % 50 == 0)
            {
                std::cout
                    << "Processed "
                    << i
                    << " / "
                    << frames.size()
                    << '\n';
            }
        }
        else
        {
            std::cout
                << "Pose not available\n";
        }

        if (previousGroundTruth != nullptr && gt != nullptr)
        {
            trajectoryLog
                << i << ','
                << frames[i].timestamp << ','
                << (poseValid ? "accepted" : "rejected") << ','
                << estimatedPosition[0] << ','
                << estimatedPosition[1] << ','
                << estimatedPosition[2] << ','
                << gt->position[0] << ','
                << gt->position[1] << ','
                << gt->position[2] << ','
                << motionEstimation.getPoseInlierCount() << ','
                << motionEstimation.getMedianParallaxPixels() << ','
                << rotationError
                << '\n';
        }

        if (previousGroundTruth == nullptr || poseValid)
        {
            anchorGroundTruth = gt;
        }

        previousGroundTruth = gt;
    }


    // ---------------------------------------------------------
    // Print trajectory summary
    // ---------------------------------------------------------

    std::cout
        << "\nTrajectory samples: "
        << trajectorySamples.size()
        << '\n';


    const size_t samplesToPrint =
        std::min<size_t>(
            5,
            trajectorySamples.size()
        );


    for (
        size_t i = 0;
        i < samplesToPrint;
        ++i
    )
    {
        const auto& sample =
            trajectorySamples[i];


        std::cout
            << "Sample "
            << i
            << " | timestamp: "
            << sample.timestamp
            << "\n  Estimated: "
            << sample.estimatedPosition[0]
            << ", "
            << sample.estimatedPosition[1]
            << ", "
            << sample.estimatedPosition[2]

            << "\n  Ground truth: "
            << sample.groundTruthPosition[0]
            << ", "
            << sample.groundTruthPosition[1]
            << ", "
            << sample.groundTruthPosition[2]
            << '\n';
    }


    std::cout
        << "\nSuccessful poses: "
        << successfulPoses
        << " / "
        << evaluatedPairs
        << " ("
        << (evaluatedPairs == 0
                ? 0.0
                : 100.0 * successfulPoses / evaluatedPairs)
        << "%)\n";

    if (!rotationErrorsDegrees.empty())
    {
        const double totalRotationError = std::accumulate(
            rotationErrorsDegrees.begin(),
            rotationErrorsDegrees.end(),
            0.0
        );

        std::cout
            << "Mean accepted-pair rotation error (deg): "
            << totalRotationError / rotationErrorsDegrees.size()
            << '\n';
    }

    std::cout
        << "Total RGB frames: "
        << frames.size()
        << '\n';

    std::cout
        << "Trajectory diagnostics: logs/tum_trajectory.csv\n";


    return 0;
}
