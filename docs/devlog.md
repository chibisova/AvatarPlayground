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
