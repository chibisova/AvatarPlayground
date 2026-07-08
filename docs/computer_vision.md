# Computer Vision

## Webcam Foundation

### Measuring FPS

Camera-reported FPS:

```cpp
cam.get(cv::CAP_PROP_FPS);
```

Measured FPS:

```cpp
double calculateFPS(...);
```

These values may differ because application processing also consumes time.

---

## Image Processing Pipeline

```text
Capture
    ↓
Process
    ↓
Draw HUD
    ↓
Display
    ↓
Handle Input
    ↓
Repeat
```

This pipeline forms the basis of future stages such as feature detection, tracking, pose estimation and SLAM.

## Color Spaces

OpenGL usually stores images as RGB.

OpenCV stores images as BGR by default.

Many classical computer vision algorithms operate on grayscale images because they analyze changes in intensity rather than color.

### Why Grayscale?

Advantages:

- reduces memory usage
- reduces computation
- removes unnecessary color information
- highlights intensity changes used by many vision algorithms

Examples:

- Canny Edge Detection
- Harris Corners
- SIFT
- ORB

## Processor Responsibilities

GrayProcessor
    Color → Gray

BlurProcessor
    Color → Blurred Color

CannyProcessor
    Color → Gray → Blur → Canny

FaceDetector
    Color → Face Bounding Boxes

HandTracker
    Color → 21 Hand Landmarks

Stylization
    Color → Stylized Color


