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
