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

## Classical Image Processing completed

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


