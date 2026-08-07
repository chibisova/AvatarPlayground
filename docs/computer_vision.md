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

# Computer Vision

## Processing Pipelines

Many computer vision algorithms are composed of multiple processing steps rather than a single operation.

Example:

```
Camera (BGR)
    ↓
Grayscale
    ↓
Gaussian Blur
    ↓
Canny Edge Detection
```

Each stage prepares the image for the next one by reducing noise or extracting information required by the following algorithm.

---

# Canny Edge Detection

## Idea

Edges are locations where image intensity changes rapidly.

Instead of looking at colors, Canny analyzes image gradients after converting the image to grayscale.

Pipeline:

```
Color
    ↓
Grayscale
    ↓
Gaussian Blur
    ↓
Gradient Calculation (Sobel)
    ↓
Non-Maximum Suppression
    ↓
Hysteresis Thresholding
```

---

## Hysteresis Thresholding

### Why do we need two thresholds?

If we use only one threshold:

```
5   18   42   85 | 160 220 250
                 ↑
              threshold
```

only values above the threshold are kept.

However, real edges are rarely equally strong across an entire object.

Instead, Canny uses two thresholds:

```
Low  = 100
High = 200
```

Pixels above the high threshold become **strong edges**.

Pixels between the two thresholds become **weak edges**.

Canny then asks:

> "Is this weak edge connected to a strong edge?"

If yes:

```
Strong ─ Weak ─ Weak
```

Keep it.

If not:

```
Weak

(no strong neighbor)
```

Discard it because it is most likely noise.

---

## Non-Maximum Suppression

Gradient magnitude is often spread across several neighboring pixels.

Non-Maximum Suppression keeps only the strongest pixel along the gradient direction, producing thin one-pixel-wide edges.

Without NMS:

```
████
████
████
```

With NMS:

```
 █
 █
 █
```

---

## Why is it called hysteresis?

The word comes from physics.

It describes a system whose current decision depends on previous or neighboring states.

In Canny, a weak edge is accepted only if it is connected to an already accepted strong edge.

---

# Adaptive Threshold

## Problem

The world is not purely black and white.

Images contain many different illumination levels, making one global threshold unreliable.

---

## Solution

Instead of asking

> "Is this pixel brighter than 128?"

Adaptive Threshold asks

> "Is this pixel brighter than its local neighborhood?"

Each small image region computes its own threshold.

---

## Result

Original:

```
████████
██▒▒▒███
██▒▒▒███
████████
```

Adaptive Threshold:

```
████████
██    ██
██    ██
████████
```

---

## Applications

- Document scanning
- OCR
- QR code detection
- Sudoku solvers
- Industrial inspection
- Contour extraction
- Robotics

---

## Relationship to other filters

Gaussian Blur
→ averages neighboring pixels.

Canny
→ detects strong intensity changes between neighboring pixels.

Adaptive Threshold
→ compares a pixel against its local neighborhood.

---

# Corner Detection

## Problem

To track an object across multiple frames, we need image features that are easy to recognize again after the camera or object moves.

Flat regions and edges are ambiguous.

Corners are much more distinctive.

---

## Shi-Tomasi

Pipeline:

```
Color
    ↓
Grayscale
    ↓
Gaussian Blur
    ↓
Shi-Tomasi
    ↓
Vector<Point2f>
```

Shi-Tomasi directly returns the strongest corners.

Because OpenCV already performs feature ranking and Non-Maximum Suppression, the detected corners are usually clean and stable.

---

## Harris Corner Detector

Pipeline:

```
Color
    ↓
Grayscale
    ↓
Gaussian Blur
    ↓
Harris Response
    ↓
Normalize
    ↓
Threshold
    ↓
Draw Corners
```

Unlike Shi-Tomasi, Harris does **not** return corner locations.

Instead, it produces a floating-point response map indicating how "corner-like" every pixel is.

The developer must normalize the response, choose a threshold, and visualize the detected corners.

---

## Harris vs Shi-Tomasi

| Harris | Shi-Tomasi |
|---------|------------|
| Returns a corner response image | Returns corner positions directly |
| Requires manual thresholding | Automatically selects the strongest corners |
| More parameters to tune | Simpler API |
| Good for understanding corner response | Commonly used in practice |

Shi-Tomasi was proposed as an improvement over Harris by eliminating the need for Harris's heuristic response function and returning the strongest features directly.

## Optical Flow (Lucas-Kanade)

### Goal

Track how image features move between consecutive frames.

Pipeline:

Camera
    ↓
Grayscale
    ↓
Gaussian Blur
    ↓
Shi-Tomasi Corner Detection (first frame)
    ↓
Lucas-Kanade Optical Flow
    ↓
Draw motion vectors

### Idea

Instead of detecting new corners every frame, estimate where previously detected corners moved.

For each tracked point:

Previous Frame
      ●
       \
        \
         ●
Current Frame

The line represents the estimated motion vector.

### Why do we need previous frame?

Optical Flow compares two consecutive frames.

Without storing the previous frame and previous feature points, motion cannot be estimated.

### Lost tracking

Some points disappear because of:

- Occlusion
- Leaving the camera view
- Motion blur
- Lighting changes

When too few points remain, detect new Shi-Tomasi corners.

### Limitations

Optical Flow tracks image patches, not objects.

It does not know whether a point belongs to a face, hand, or background.

Therefore tracking may drift when:

- Objects overlap
- Lighting changes
- The appearance of a patch changes significantly

## ORB (Oriented FAST and Rotated BRIEF)

### Purpose:
- Detect repeatable keypoints.
- Compute a descriptor for each keypoint.

### Pipeline:

```
Image
   ↓
FAST
   ↓
KeyPoints
   ↓
BRIEF
   ↓
Binary Descriptors
```

### KeyPoint
- Stores **where** the feature is.
- Position (`x`, `y`), orientation, scale, etc.

### Descriptor
- Stores **what the neighborhood looks like**.
- ORB descriptor is a **256-bit binary fingerprint**.
- Generated by comparing brightness of many pairs of pixels around the keypoint.

```
KeyPoint
---------
x = 350
y = 220

Descriptor
----------
101101001011...
011010110100...
```

One keypoint → One descriptor.

### Advantages
- Fast
- Rotation invariant
- Robust to moderate illumination changes
- Suitable for real-time applications (SLAM, Visual Odometry)

---

## BFMatcher (Brute Force Matcher)

### Purpose:
- Match descriptors between two frames.

### Pipeline

```
Descriptors A
       +
Descriptors B
       ↓
Compare every descriptor
       ↓
Best Match
```

Uses Hamming distance because ORB descriptors are binary.

### Hamming distance
- Counts how many bits differ.

```
10110010
10100011
   ^   ^
Distance = 2
```

Smaller distance = more similar descriptors.

### Cross Check
- Keeps only mutual best matches.
- Improves reliability by rejecting one-sided matches.

---

## KNN Matching + Lowe Ratio Test

### Purpose
- Reduce ambiguous descriptor matches.

Instead of one best match:

```
Descriptor
      ↓
Best Match
```

KNN returns the two nearest neighbors.

```
Descriptor
      ↓
Best
Second Best
```

### Lowe Ratio Test

```
best.distance < 0.75 * second.distance
```

If the best match is significantly better than the second-best,
accept it.

Otherwise reject it.

### Advantages
- Adaptive (doesn't rely on a fixed distance threshold)
- Rejects ambiguous matches
- More robust than BFMatcher alone

### Pipeline

```
ORB
   ↓
Descriptors
   ↓
KNN (k = 2)
   ↓
Lowe Ratio Test
   ↓
Good Matches
```

## Summary of learned concepts

```
Corner Detection
    ↓
Harris
Shi-Tomasi

Tracking
    ↓
Lucas-Kanade Optical Flow

Feature Detection
    ↓
ORB

Feature Matching
    ↓
BFMatcher
KNN + Lowe Ratio

Geometry (Next)
    ↓
RANSAC
Homography
Camera Motion
SLAM
```
## RANSAC (Random Sample Consensus)

### Problem

Even after descriptor matching, some correspondences are incorrect (outliers).

Using all matches directly can produce an incorrect transformation.

### Idea

1. Randomly sample a small subset of correspondences.
2. Estimate a transformation.
3. Count how many correspondences agree with it (inliers).
4. Repeat many times.
5. Keep the transformation supported by the largest number of inliers.

### Inlier Test

A correspondence is an **inlier** if the transformed point is sufficiently close to its observed position.

Otherwise it is an **outlier**.

### Output

RANSAC returns:

- the estimated transformation (e.g. homography)
- an inlier mask indicating which correspondences support the model

## Essential Matrix

### Purpose

Estimate the relative motion between two calibrated camera views.

Unlike homography, the Essential Matrix models general 3D camera motion and is suitable for visual odometry.

### Pipeline

ORB
→ Feature Matching
→ Lowe's Ratio Test
→ Essential Matrix (`findEssentialMat`)
→ Camera Pose (`recoverPose`)

### Inputs

- Previous frame keypoints
- Current frame keypoints
- Camera intrinsic matrix (K)

### Output

Essential Matrix (`E`)

The Essential Matrix encodes the relative geometric relationship between two calibrated camera views.

It is not the camera pose itself.

---

## Camera Intrinsics (K)

The intrinsic matrix describes the internal properties of a camera.

It maps camera-space coordinates onto image pixels.

Typical form:

| fx  0  cx |
| 0  fy  cy |
| 0   0   1 |

where:

- `fx`, `fy` — focal lengths
- `cx`, `cy` — principal point (image center)

For learning purposes, the intrinsic matrix can be approximated from the image resolution.

In production systems, it is obtained through camera calibration.

---

## Camera Pose Recovery

`cv::recoverPose()` decomposes the Essential Matrix into:

- Rotation matrix (`R`)
- Translation vector (`t`)

These describe the camera motion between two consecutive frames.

The translation direction is recovered, but its absolute scale is unknown in monocular visual odometry.

The function also returns the number of geometrically consistent inlier correspondences.

---

## Motion Validation

Although `cv::recoverPose()` always returns a relative camera pose, the estimate is not always reliable.

Sources of failure include:

- Incorrect feature matches
- Dynamic objects in the scene
- Low camera motion
- Degenerate geometric configurations

Therefore, pose estimates should be validated before being used.

Common validation strategies include:

- Minimum number of inliers
- Inlier ratio
- Reprojection error
- Temporal consistency

For this project, a minimum inlier threshold is used:

```cpp
if (inliers < MIN_INLIERS)
    poseValid = false;
```
---

## Visual Odometry

Visual odometry estimates the camera's movement over time by accumulating relative poses between consecutive frames.

For each pair of frames:

``` text
Frame N-1 → Frame N
       │
       ▼
Relative rotation R
Relative translation t
       │
       ▼
Relative camera pose
       │
       ▼
Accumulate over time
       │
       ▼
Global camera trajectory
```

A relative pose can be represented using a homogeneous transformation matrix:

\[ T =
```{=tex}
\begin{bmatrix}
R & t \\
0 & 1
\end{bmatrix}
```
\]

The global camera pose is updated by composing the previous global pose with the newly estimated relative pose.

Because monocular visual odometry cannot directly recover absolute translation scale, the resulting trajectory has an arbitrary scale.

---

## Triangulation

Given corresponding observations of the same feature in two frames and the relative camera poses, triangulation estimates the 3D position of the feature.

``` text
2D point in Frame 1
        +
2D point in Frame 2
        +
Camera poses
        │
        ▼
Triangulation
        │
        ▼
Estimated 3D point
```

OpenCV initially represents the result in homogeneous coordinates:

$$
\mathbf{X} =
\begin{bmatrix}
X \\
Y \\
Z \\
W
\end{bmatrix}
$$

The Euclidean 3D position is obtained by dividing by (W):

\[ x = `\frac{X}{W}`{=tex}, `\qquad`{=tex} y = `\frac{Y}{W}`{=tex},
`\qquad`{=tex} z = `\frac{Z}{W}`{=tex} \]

The resulting point cloud is sparse because it contains only visual features that were successfully detected and matched between frames.

### Depth Validation

Not every triangulated point is geometrically valid. Points with invalid or non-positive depth are rejected before being used as reconstructed scene points.

This produces the following pipeline:

``` text
Feature Detection
        ↓
Feature Matching
        ↓
Geometric Verification
        ↓
Camera Pose Estimation
        ↓
Triangulation
        ↓
Depth Validation
        ↓
Sparse 3D Reconstruction
```
---

### Data Consistency

Triangulation assumes that:

- matched keypoints,
- descriptors,
- camera poses,

all belong to the same pair of frames.

---
