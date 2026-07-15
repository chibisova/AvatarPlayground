# Avatar Playground

A modular C++ computer vision playground for developing and evaluating real-time perception modules for intelligent avatars, spatial computing, and XR applications.

Avatar Playground is designed as a unified framework for implementing, visualizing, and comparing both classical and learning-based perception algorithms. Starting from low-level image processing, the project progressively expands toward feature tracking, pose estimation, visual odometry, sensor fusion, and neural avatar technologies, with each module designed to integrate into a single real-time perception pipeline.

The long-term objective is to bridge computer vision, computer graphics, machine learning, and XR in an extensible environment for rapid experimentation with next-generation avatar systems.

---

# Current Capabilities

## Image Processing

- Grayscale
- Gaussian Blur
- Thresholding
- Canny Edge Detection

## Feature Detection

- Harris Corner Detector
- Shi-Tomasi Corner Detector
- ORB (Oriented FAST and Rotated BRIEF)

## Motion Tracking

- Lucas–Kanade Optical Flow

## Feature Matching

- Brute Force Matcher (Hamming Distance)
- K-Nearest Neighbors (KNN)
- Lowe's Ratio Test
- RANSAC-based Homography Estimation

---

# Architecture

```
Camera
   │
   ▼
ProcessorManager
   │
   ├── GrayProcessor
   ├── BlurProcessor
   ├── ThresholdProcessor
   ├── CannyProcessor
   ├── HarrisProcessor
   ├── ShiTomasiProcessor
   ├── OpticalFlowProcessor
   ├── ORBProcessor
   ├── BFMatcherProcessor
   ├── KNNMatcherProcessor
   └── RANSACProcessor
```

Each perception module implements a common processor interface, allowing algorithms to be developed, evaluated, and replaced independently.

The architecture separates acquisition, processing, visualization, and configuration, making the framework suitable for iterative experimentation and future integration of learning-based methods.

Stateful modules (such as optical flow and feature matching) maintain persistent state through `ProcessorManager`, enabling temporal algorithms to operate consistently across consecutive frames.

---

# Research Roadmap

Avatar Playground is developed incrementally, with each stage building upon previous components.

### Classical Computer Vision

- [x] Image Processing
- [x] Corner Detection
- [x] Optical Flow
- [x] Feature Detection (ORB)
- [x] Feature Matching
- [x] Homography Estimation (RANSAC)

### Geometric Vision

- [x] Essential Matrix Estimation
- [ ] Camera Pose Estimation (PnP)
- [ ] Epipolar Geometry
- [ ] Visual Odometry
- [ ] Visual-Inertial Odometry
- [ ] Bundle Adjustment
- [ ] SLAM Fundamentals

### Human Perception

- [ ] Face Detection
- [ ] Facial Landmark Detection
- [ ] Head Pose Estimation
- [ ] Hand Tracking
- [ ] Body Pose Estimation

### Neural Perception

- [ ] Learned Feature Matching (SuperPoint / LightGlue)
- [ ] Monocular 3D Face Reconstruction
- [ ] Neural Avatar Reconstruction
- [ ] Generative Avatar Models
- [ ] Expression Transfer
- [ ] Motion Generation

### Real-Time Avatar Systems

- [ ] Unity Integration
- [ ] Real-Time Avatar Animation
- [ ] Sensor Fusion
- [ ] Spatial Avatar Pipeline

---

# Technologies

- C++20
- OpenCV 4.x
- CMake
- Visual Studio Code

Future integrations may include:

- PyTorch
- ONNX Runtime
- MediaPipe
- Unity
- CUDA

---

# Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Run:

```bash
./AvatarPlayground
```

---

# Project Structure

```
include/
│
├── processors/
├── core/
├── utils/
└── visualization/

src/
├── processors/
├── core/
└── visualization/

assets/
docs/

main.cpp
ProcessorManager.cpp
Config.h
```

---

# Design Principles

The framework is designed around several core principles:

- **Modularity** — each algorithm exists as an independent processor.
- **Comparability** — different methods can be evaluated under a common interface.
- **Real-Time Performance** — interactive visualization and low-latency processing.
- **Extensibility** — new perception modules can be integrated without modifying the existing pipeline.
- **Research-Oriented Development** — classical computer vision and modern deep learning methods coexist within the same architecture.

---

# Long-Term Vision

Avatar Playground is intended to evolve into a unified perception framework for intelligent avatars.

Rather than existing as a collection of isolated computer vision demos, the framework serves as an experimental environment where perception, geometry, machine learning, and graphics modules can be combined into complete real-time avatar systems.

Future work will progressively integrate neural reconstruction, generative models, and spatial computing techniques, enabling rapid experimentation with next-generation digital human technologies.
