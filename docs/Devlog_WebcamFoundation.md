# Development Log

This document records important engineering concepts, design decisions, and observations made while developing **Spatial Avatar Playground**. The goal is not only to document *what* was implemented, but also *why* specific design choices were made.

Devlog structure: 
## Topic
Passing by Reference (&)

### What I learned
Passing by reference avoids copying large objects.

### Why it matters
OpenCV classes such as cv::Mat and cv::VideoCapture can be expensive to copy. Passing them by reference improves performance and makes the function's intent clearer.

### Example

---

# Topic: Webcam Foundation

## 1. Measuring FPS and Resolution

There are two different ways to obtain FPS and image resolution.

### A. Query camera properties

The camera reports its configured (or requested) properties through OpenCV.

```cpp
double cameraFPS = cam.get(cv::CAP_PROP_FPS);

int width = static_cast<int>(cam.get(cv::CAP_PROP_FRAME_WIDTH));
int height = static_cast<int>(cam.get(cv::CAP_PROP_FRAME_HEIGHT));
```

These values describe the camera itself.

---

### B. Measure application performance

The application's actual FPS can be measured by calculating the time between two processed frames.

```cpp
double calculateFPS(std::chrono::steady_clock::time_point& previousTime)
{
    auto currentTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();

    previousTime = currentTime;
    return 1.0 / deltaTime;
}
```

The image resolution can also be read directly from the captured frame.

```cpp
frame.cols
frame.rows
```

### Observation

The camera FPS and the measured FPS are not necessarily the same.

For example:

- Camera reports **30 FPS**
- Application processes **24 FPS**

The difference comes from the application's processing time.

---

# 2. Passing by Reference (&)

Use references (`&`) when:

- a function needs to modify an object, or
- the object is large and copying it would be inefficient.

Passing by reference avoids unnecessary copies and allows the function to work with the original object.

Example:

```cpp
void drawHUD(cv::Mat& frame);
```

Instead of copying the image, the function works directly with the existing frame.

---

# 3. Using const

Use `const` whenever a function should not modify an object.

Benefits:

- communicates intent to other developers
- prevents accidental modification
- allows the compiler to detect mistakes
- makes APIs easier to understand

Example:

```cpp
void saveScreenshot(
    const cv::Mat& frame,
    const std::string& screenshotsDir,
    int& screenshotCount);
```

From the function signature alone, it is immediately clear that:

- `frame` will only be read
- `screenshotsDir` will only be read
- `screenshotCount` will be modified

---

# 4. auto vs Explicit Types

Prefer explicit types for simple variables.

Examples:

```cpp
int
double
bool
```

Use `auto` when:

- the type is long
- the type is obvious from the right-hand side

Example:

```cpp
auto currentTime = std::chrono::steady_clock::now();
```

Using `auto` here improves readability because the actual type is long and already implied by the expression.

---

# 5. Centralizing Project Constants

Project-wide constants should be stored in a dedicated configuration file instead of hardcoding values throughout the project.

Example:

```cpp
namespace Config
{
    constexpr int ESC_KEY = 27;

    inline const std::string SCREENSHOT_DIR = "./screenshots";
}
```

They can then be accessed anywhere as:

```cpp
Config::ESC_KEY
Config::SCREENSHOT_DIR
```

Benefits:

- easier maintenance
- avoids duplicated values
- changing a constant only requires modifying one file

---

# 6. Single Responsibility Principle

Each function should have one clear responsibility.

Instead of writing one large `main()` function, separate the logic into smaller functions.

Examples:

```cpp
drawHUD()
calculateFPS()
handleKeyboard()
saveScreenshot()
```

Benefits:

- easier to read
- easier to debug
- easier to test
- easier to extend

Small, focused functions are generally easier to maintain than one large function that performs many unrelated tasks.

---

# 7. Separating Interface and Implementation

As the project grows, split it into header (`.h`) and source (`.cpp`) files.

Project structure:

```text
SpatialAvatarPlayground/

include/
    Config.h
    HUD.h
    Input.h
    Timing.h

src/
    main.cpp
    HUD.cpp
    Input.cpp
    Timing.cpp
```

### Header files (`.h`)

Contain declarations only.

Example:

```cpp
void drawHUD(cv::Mat& frame, double fps, int width, int height);
```

---

### Source files (`.cpp`)

Contain the implementation.

Example:

```cpp
void drawHUD(...)
{
    ...
}
```

This separation improves readability, compilation, and scalability as the project grows.

---

# 8. Application Architecture

A real-time vision application follows a continuous processing loop.

```text
Initialize Application
        │
        ▼
Capture Frame
        │
        ▼
Process Frame
        │
        ▼
Draw HUD
        │
        ▼
Display Frame
        │
        ▼
Handle Input
        │
        ▼
Calculate FPS
        │
        └──────────────► Repeat
```

Keeping the loop organized makes the program easier to understand and extend with additional processing stages such as face tracking, hand tracking, or visual odometry.

---

# 9. Designing Function Interfaces

When designing a function, ask:

> **What is the smallest, clearest interface this function needs?**

Good function interfaces expose only the information required to perform their task.

Example:

```cpp
double calculateFPS(std::chrono::steady_clock::time_point& previousTime);
```

Instead of requiring both the previous and current timestamps, the function determines the current time internally. This keeps the interface simple and hides implementation details from the caller.

In general, functions should:
- accept only the parameters they need,
- hide internal implementation details,
- return only the information needed by the caller.
