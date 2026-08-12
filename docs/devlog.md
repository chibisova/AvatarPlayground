# Development Log

## 2026-07-07

### Webcam Foundation
- Built first C++ OpenCV application.
- Implemented FPS calculation and HUD.
- Added screenshot support.
- Refactored into multiple modules.
- Learned function declarations vs implementations.

## 2026-07-08

### Image Processing
- Designed `processFrame()` using an API-first approach.
- Learned the difference between shallow and deep copies of `cv::Mat`.
- Reorganized project into scalable modules before adding computer vision algorithms.
- Implemented Grayscale mode.

## 2026-07-09

### Classical Image Processing completed

- Added Gaussian blur, Canny edge detection
- Added adaptive thresholding
- Added Shi-Tomasi and Harris corner detection
- Refactored processing pipeline

## 2026-07-10

### Architecture

- Learned interfaces and polymorphism
- Refactored image processing into an extensible processor architecture

## 2026-07-11

### Motion Analysis

- Implemented Lucas-Kanade Optical Flow
- Learned the difference between stateless and stateful processors

### Architecture

- Introduced `ProcessorManager`
- Replaced per-frame processor creation with persistent processor instances

## 2026-07-12

### ORB and BFMatcher

- Implemented ORB processor to track local features
- Implemented `BFMatcherProcessor` for frame-to-frame ORB descriptor matching (Hamming distance, cross-check enabled)
- Visualized matches side-by-side via `cv::drawMatches`, with per-match random coloring for readability
- Added first-frame guard (skips matching until `previousDescriptors` is populated)

### Architecture 

- Removed obsolete `processFrame()` switch
- Replaced `processingModeToString()` with `IImageProcessor::name()`
- Moved `ProcessingMode` into its own header
- Simplified processor management through `ProcessorManager`


## 2026-07-13

### Feature Matching

- Improved `BFMatcherProcessor` by replacing the fixed distance threshold with an adaptive minimum-distance filter.
- Implemented `KNNMatcherProcessor` using KNN matching (`k = 2`) and Lowe's Ratio Test for more robust descriptor matching.

## 2026-07-14

### RANSAC Homography Estimation

- Implemented `RANSACProcessor` using ORB features, KNN matching, and Lowe's Ratio Test.
- Added homography estimation with `cv::findHomography(..., cv::RANSAC)`.
- Filtered descriptor matches using the returned inlier mask to keep only geometrically consistent correspondences.

## 2026-07-15

### Essential Matrix & Camera Motion Estimation

- Implemented `MotionEstimationProcessor` using ORB feature detection with KNN matching and Lowe's Ratio Test
- Estimated the Essential Matrix using `cv::findEssentialMat()`
- Recovered relative camera rotation (`R`) and translation (`t`) using `cv::recoverPose()`
- Added approximate camera intrinsic matrix generation through `CameraCalibration`
- Introduced temporary camera calibration initialization using image dimensions


## 2026-07-17

### Motion Estimation

- Fixed processing mode switching and keyboard handling
- Refactored motion estimation pipeline

## 2026-07-19

### Research

- Paper summary: ELITE
- Paper summary: LeGO

## 2026-07-20~23

#### Motion Estimation

- Implemented Essential Matrix estimation using `cv::findEssentialMat()`
- Implemented relative camera pose recovery using `cv::recoverPose()`
- Added temporary camera intrinsic matrix approximation
- Exposed relative rotation and translation through public getters
- Added pose validity checks and confidence gating using the number of inliers
- Filtered unreliable pose estimates before propagation to Visual Odometry

#### Visual Odometry

- Implemented `VisualOdometry` module
- Represented camera poses as 4×4 homogeneous transformation matrices
- Implemented incremental pose composition (`globalPose = globalPose * relativePose`)
- Added persistent 2D trajectory visualization
- Drew camera trajectory in the X–Z plane
- Added trajectory window and real-time visualization

#### Debugging

- Fixed incorrect `recoverPose()` parameter order
- Fixed OpenCV camera intrinsic matrix initialization
- Investigated instability caused by low-inlier Essential Matrix estimation
- Added minimum inlier threshold to reject unstable pose estimates
- Identified dynamic scene objects as a major source of Visual Odometry drift

## 2026-07-24

### Triangulation

- Implemented `TriangulationProcessor` for estimating the 3D position of the matched visual features
- Added depth validation to reject points with invalid or non-positive depth
- Successfully reconstructed sparse 3D feature points from matched observations across two frames

## 2026-07-27

- Add initial PnP Processor setup

## 2026-08-05

- Implemented 3D map from triangulation

## 2026-08-06

### 3D Mapping

- Added `MapPoint` structure storing 3D position and ORB descriptor
- Extended `Map3D` to maintain a persistent sparse point cloud

### Architecture

- Fixed descriptor lifetime bug between `MotionEstimationProcessor` and `TriangulationProcessor`
- Introduced frozen snapshots (`matchedPreviousKeypoints`, `matchedPreviousDescriptors`) before `advanceFrame()`
- Prevented frame-state mismatches between feature matches and descriptor storage
- Clarified ownership of frame-dependent data across pipeline stages

## 2026-08-10

- Implemented 3D <-> 2D matcher via `Map3D::matchDescriptors`
- Added detailed comments to each processor
- Repalced `solvePnP` with `solvePnPRansac` to automatically reject incorrect correspondences

## 2026-08-11

- Refactored `PnPProcessor` into a full processing stage following the project architecture
- Added descriptor matching between the current frame and `Map3D`
- Built 3D–2D correspondences from matched map landmarks
- Integrated `cv::solvePnPRansac()` for robust camera localization
- Successfully estimated absolute camera's absolute pose (`rvec`, `tvec`) from the reconstructed sparse map

## 2026-08-12

- Completed triangulation geometry validation
- Fixed homogeneous 3D point conversion by explicitly handling `CV_32F` → `CV_64F`
- Added positive-depth filtering for triangulated points
- Added reprojection-error filtering to reject geometrically inconsistent 3D points
- Verified that filtered triangulated points continue to support successful PnP localization
