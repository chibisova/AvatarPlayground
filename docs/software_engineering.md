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

## Build Systems (CMake)

Creating a new `.cpp` file is not enough.

The build system must also know that the file exists.

For CMake projects, every new implementation file should be added to `add_executable(...)` (or `target_sources(...)`).

Otherwise the compiler will ignore the file, which may lead to confusing IntelliSense or linker errors.

## Stateful vs Stateless Objects

Some algorithms only process the current frame.

Examples:

- Grayscale
- Gaussian Blur
- Canny

These processors are **stateless**.

---

Algorithms such as Optical Flow require memory from previous frames.

Examples:

- Previous frame
- Previously tracked points

These are **stateful**.

Creating a new object every frame destroys that memory.

Instead, create processors once and reuse them during the application's lifetime.

### ProcessorManager

Instead of creating processors inside a `switch` every frame:

```cpp
GrayProcessor processor;
processor.process(frame);
```

store persistent processor instances:

```text
ProcessorManager
    ├── GrayProcessor
    ├── BlurProcessor
    ├── CannyProcessor
    ├── ...
    └── OpticalFlowProcessor
```

Each processor owns its internal state independently.

## Smart Pointers

Smart pointers automatically manage the lifetime of dynamically allocated objects.

Common examples:

- `std::unique_ptr` (C++ Standard Library)
- `cv::Ptr` (OpenCV)

### Why use them?

Instead of manually writing

```cpp
delete ptr;
```

the object is automatically destroyed when the smart pointer goes out of scope.

Benefits:

- Prevents memory leaks
- Prevents forgetting `delete`
- Makes ownership explicit
- Produces safer, cleaner code

## Processor Manager

Instead of creating a new processor every frame:

```text
processFrame()
    ↓
Create Processor
    ↓
process()
```

the application now owns one persistent instance of each processor:

```text
ProcessorManager
        ↓
GrayProcessor
BlurProcessor
...
OpticalFlowProcessor
ORBProcessor
```

### Benefits

- Supports stateful algorithms (e.g. Optical Flow)
- Avoids unnecessary object creation every frame
- Easier to extend with new processors
- Centralizes processor management


