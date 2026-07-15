# OpenCV

## Camera Properties

Camera-reported values:

```cpp
cam.get(cv::CAP_PROP_FPS);
cam.get(cv::CAP_PROP_FRAME_WIDTH);
cam.get(cv::CAP_PROP_FRAME_HEIGHT);
```

Actual application FPS can be measured manually.

---

## cv::Mat

`cv::Mat` is a smart container.

### Shallow Copy

```cpp
cv::Mat processed = frame;
```

Creates a new `cv::Mat` object that shares the same pixel buffer.

### Deep Copy

```cpp
cv::Mat processed = frame.clone();
```

Allocates a new pixel buffer.

Use `clone()` only when an independent image is required.

---

## Sharing Images Between Modules

Pass the original image as:

```cpp
const cv::Mat& frame
```

Create a clone only for modules that need to modify or annotate the image.

Rule of thumb:

> Read from the original image. Copy only when you need to modify it.

---

## cv::Mat Copy vs C++ Reference

```cpp
cv::Mat processed = frame;
```

Two objects sharing one pixel buffer.

```cpp
cv::Mat& processed = frame;
```

One object with two names (alias).


## Color Space Conversion

```cpp
cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
```

## Single vs Multi-channel Images

BGR images contain three channels.

Grayscale images contain one channel.

Because grayscale images no longer store color information, drawing colored overlays (HUD, landmarks, boxes) requires either:

- converting back to BGR, or
- visualizing on a separate color image.

## cv::Mat

Represents an image (matrix).

Common operations:

```cpp
frame.clone();
frame.cols;
frame.rows;
```

---

## Color Conversion

Convert between image color spaces.

```cpp
cv::cvtColor(
    frame,
    gray,
    cv::COLOR_BGR2GRAY
);
```

---

## Feature Detection

### ORB

```cpp
auto orb = cv::ORB::create();

orb->detectAndCompute(
    image,
    cv::noArray(),
    keypoints,
    descriptors
);
```

Returns

- `std::vector<cv::KeyPoint>`
- `cv::Mat descriptors`

---

## Descriptor Matching

### BFMatcher

```cpp
cv::BFMatcher matcher(
    cv::NORM_HAMMING,
    false
);
```

Common methods

```cpp
matcher.match(...)
matcher.knnMatch(...)
```

Use `cv::NORM_HAMMING` with ORB descriptors.

---

## Drawing Utilities

Draw detected features.

```cpp
cv::drawKeypoints(...)
```

Draw descriptor matches.

```cpp
cv::drawMatches(...)
```

Draw primitives.

```cpp
cv::circle(...)
cv::line(...)
cv::rectangle(...)
cv::putText(...)
```

---

## noArray()

Represents an empty optional OpenCV input/output array.

Useful when no mask is provided.

```cpp
cv::noArray()
```

Example

```cpp
orb->detectAndCompute(
    gray,
    cv::noArray(),
    keypoints,
    descriptors
);
```

---

## cv::Ptr

OpenCV smart pointer.

Used by many factory functions.

```cpp
cv::Ptr<cv::ORB> orb =
    cv::ORB::create();
```

Equivalent modern C++ syntax:

```cpp
auto orb = cv::ORB::create();
```

## Homography Estimation

Estimate a perspective transformation between two sets of corresponding points.

```cpp
cv::Mat H = cv::findHomography(
    previousPoints,
    currentPoints,
    cv::RANSAC,
    3.0,
    inlierMask
);
```

### Parameters

- `previousPoints` – source image points
- `currentPoints` – destination image points
- `cv::RANSAC` – robust estimation method
- `3.0` – reprojection error threshold (pixels)
- `inlierMask` – output mask identifying inlier correspondences

### Output

Returns a `3×3` homography matrix describing the perspective transformation between the two images.

## Camera Motion Estimation

### `cv::findEssentialMat()`

Computes the Essential Matrix from corresponding feature points in two calibrated images.

Inputs:

- Previous image points
- Current image points
- Camera intrinsic matrix (`K`)

Returns:

- Essential Matrix (`E`)

Optionally returns an inlier mask from RANSAC.

---

### `cv::recoverPose()`

Decomposes the Essential Matrix into camera motion.

Inputs:

- Essential Matrix (`E`)
- Previous image points
- Current image points
- Camera intrinsic matrix (`K`)

Outputs:

- Rotation matrix (`R`)
- Translation vector (`t`)
- Number of inlier correspondences

---

### Function Overloads

Many OpenCV functions provide multiple overloads with different parameter orders.

Example:

```cpp
cv::recoverPose(E, points1, points2, K, R, t);
