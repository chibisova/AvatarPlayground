#include <opencv2/opencv.hpp>

#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "Config.h"
#include "ProcessorManager.h"
#include "CameraCalibration.h"
#include "VisualOdometry.h"
#include "processors/MotionEstimationProcessor.h"
#include "processors/TriangulationProcessor.h"
#include "processors/PnPProcessor.h"
#include "Map3D.h"
#include "visualization/SpatialVisualizer.h"


int main()
{
    std::cout << "===== Spatial Mapping Demo =====\n";

    const std::string videoPath =
        "assets/demo/spatial_mapping_demo.mp4";

    cv::VideoCapture cam(videoPath);

    if (!cam.isOpened())
    {
        std::cerr
            << "Error: Could not open video: "
            << videoPath
            << '\n';

        return -1;
    }

    std::cout
        << "Video opened successfully.\n";

    std::cout
        << "Resolution: "
        << cam.get(cv::CAP_PROP_FRAME_WIDTH)
        << " x "
        << cam.get(cv::CAP_PROP_FRAME_HEIGHT)
        << '\n';

    std::cout
        << "FPS: "
        << cam.get(cv::CAP_PROP_FPS)
        << '\n';


    // ------------------------------------------------------------
    // Existing processing pipeline
    // ------------------------------------------------------------

    vision::ProcessorManager processorManager;

    vision::VisualOdometry visualOdometry;

    vision::TriangulationProcessor triangulationProcessor;

    vision::PnPProcessor pnpProcessor;

    vision::Map3D map;

    vision::SpatialVisualizer spatialVisualizer;

    pnpProcessor.setMap(&map);


    // ------------------------------------------------------------
    // Unlike the interactive application, the demo starts directly
    // in MotionEstimation mode.
    // ------------------------------------------------------------

    ProcessingMode currentMode =
        ProcessingMode::MotionEstimation;


    cv::Mat frame;

    int frameNumber = 0;

    constexpr int VISUALIZATION_STEP = 3;

    const std::string screenshotDirectory =
        "assets/demo/trajectory_frames";

    std::filesystem::create_directories(
        screenshotDirectory
    );

    int screenshotIndex = 0;

    // ------------------------------------------------------------
    // Frame-to-video synchronization mapping
    // ------------------------------------------------------------

    std::ofstream frameMapping(
        screenshotDirectory + "/frame_mapping.txt"
    );

    if (!frameMapping.is_open())
    {
        std::cerr
            << "Error: Could not create frame mapping file.\n";

        return -1;
    }
    
    // ------------------------------------------------------------
    // Main video-processing loop
    // ------------------------------------------------------------

    while (true)
    {
        cam >> frame;

        if (frame.empty())
        {
            break;
        }


        // IMPORTANT:
        // Do NOT mirror the prerecorded video.
        //
        // The live application uses cv::flip() because it is a
        // webcam preview. For an offline trajectory demo we want
        // the actual camera motion preserved.

        Config::CAMERA_INTRINSICS =
            vision::CameraCalibration::createIntrinsicMatrix(
                frame.cols,
                frame.rows
            );


        // --------------------------------------------------------
        // Get MotionEstimation processor
        // --------------------------------------------------------

        auto* processor =
            processorManager.getProcessor(currentMode);

        if (processor == nullptr)
        {
            std::cerr
                << "Error: Could not obtain processor.\n";

            return -1;
        }


        // --------------------------------------------------------
        // Process frame
        // --------------------------------------------------------

        cv::Mat processedFrame =
            processor->process(frame);


        // --------------------------------------------------------
        // Motion Estimation
        // --------------------------------------------------------

        auto* motion =
            dynamic_cast<
                vision::MotionEstimationProcessor*
            >(processor);


        if (motion &&
            motion->hasValidPose())
        {
            const auto& R =
                motion->getRotation();

            const auto& t =
                motion->getTranslation();


            const auto& previousKeypoints =
                motion->getPreviousKeypoints();

            const auto& currentKeypoints =
                motion->getCurrentKeypoints();

            const auto& previousDescriptors =
                motion->getPreviousDescriptors();

            const auto& goodMatches =
                motion->getGoodMatches();


            // ----------------------------------------------------
            // Visual Odometry
            // ----------------------------------------------------

            visualOdometry.update(
                R,
                t
            );


            // ----------------------------------------------------
            // Triangulation
            // ----------------------------------------------------

            std::vector<vision::MapPoint> newPoints =
                triangulationProcessor.triangulate(
                    R,
                    t,
                    previousKeypoints,
                    currentKeypoints,
                    previousDescriptors,
                    goodMatches
                );


            map.updateLandmarks(newPoints);


            // ----------------------------------------------------
            // PnP camera localization
            // ----------------------------------------------------

            pnpProcessor.process(frame);


            if (pnpProcessor.hasValidPose())
            {
                const cv::Mat& rvec =
                    pnpProcessor.getRotationVector();

                const cv::Mat& tvec =
                    pnpProcessor.getTranslationVector();


                cv::Mat R_pnp;

                cv::Rodrigues(
                    rvec,
                    R_pnp
                );


                cv::Mat cameraPosition =
                    -R_pnp.t() * tvec;


                map.addCameraPosition(
                    cv::Point3f(
                        static_cast<float>(
                            cameraPosition.at<double>(0)
                        ),
                        static_cast<float>(
                            cameraPosition.at<double>(1)
                        ),
                        static_cast<float>(
                            cameraPosition.at<double>(2)
                        )
                    )
                );
            }


            // ----------------------------------------------------
            // Spatial visualization
            // ----------------------------------------------------

            if (map.getPoints().size() > 500 &&
                frameNumber % VISUALIZATION_STEP == 0)
            {
                spatialVisualizer.updateMap(map);

                spatialVisualizer.updateCameraPose(
                    visualOdometry.getGlobalPose()
                );

                spatialVisualizer.show();

                if (spatialVisualizer.wasStopped())
                {
                    break;
                }

                std::ostringstream filename;
                filename
                    << screenshotDirectory
                    << "/frame_"
                    << std::setw(5)
                    << std::setfill('0')
                    << screenshotIndex
                    << ".png";

                std::ostringstream overviewFilename;
                overviewFilename
                    << screenshotDirectory
                    << "/overview_"
                    << std::setw(5)
                    << std::setfill('0')
                    << screenshotIndex
                    << ".png";

                spatialVisualizer.saveScreenshot(
                    filename.str()
                );

                spatialVisualizer.saveOverviewScreenshot(
                    overviewFilename.str()
                );

                frameMapping
                    << screenshotIndex
                    << " "
                    << frameNumber
                    << "\n";

                screenshotIndex++;
            }

            // ----------------------------------------------------
            // Console output
            // ----------------------------------------------------

            std::cout
                << "Frame "
                << frameNumber
                << " | Map size: "
                << map.getPoints().size()
                << '\n';
        }


        frameNumber++;
    }

    frameMapping.close();

    // ------------------------------------------------------------
    // Save final reconstruction
    // ------------------------------------------------------------

    map.savePLY(
        "map_all_demo.ply",
        1
    );

    map.savePLY(
        "map_persistent_demo.ply",
        2
    );

    map.saveCameraTrajectoryPLY(
        "camera_trajectory_demo.ply"
    );


    std::cout
        << "\n===== Demo Finished =====\n";

    std::cout
        << "Total frames processed: "
        << frameNumber
        << '\n';

    std::cout
        << "Final map size: "
        << map.getPoints().size()
        << '\n';


    cam.release();

    cv::destroyAllWindows();

    return 0;
}
