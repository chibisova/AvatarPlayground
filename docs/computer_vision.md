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
