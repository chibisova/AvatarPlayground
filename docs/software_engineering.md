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

## Dispatch Tables (Future Refactoring)

When the number of processing modes grows, replacing a large `switch` statement with a dispatch table can improve maintainability.

### Example:

ProcessingMode → Function

Instead of

`switch(mode)`

we could use

`processors[currentMode](frame);`

For a small number of modes (5–10), a `switch` is usually the simplest and most readable solution.

## Header Dependencies

A header should include every header required to compile itself.

If a type appears in a function declaration, the corresponding header should usually be included in the `.h` file.

Implementation-only dependencies belong in the `.cpp` file.

Goal:
Every header should be self-contained and compile independently.
