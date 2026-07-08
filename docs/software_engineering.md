# Software Engineering

## API-First Design

Design the function interface before implementing the algorithm.

```cpp
cv::Mat processFrame(const cv::Mat& frame, ProcessingMode mode);
```

A clean API communicates intent and hides implementation details.

---

## Single Responsibility Principle

Each function should perform one clear task.

Examples:
- drawHUD()
- calculateFPS()
- handleKeyboard()
- saveScreenshot()

---

## Separation of Concerns

Split the project into focused modules.

```text
include/
    Config.h
    HUD.h
    Input.h
    Timing.h
    ImageProcessor.h

src/
    main.cpp
    HUD.cpp
    Input.cpp
    Timing.cpp
    ImageProcessor.cpp
```

`main.cpp` coordinates the application while each module owns one responsibility.

---

## Designing Function Interfaces

Ask:

> What is the smallest, clearest interface this function needs?

Functions should:
- accept only necessary parameters,
- hide implementation details,
- return only what the caller needs.
