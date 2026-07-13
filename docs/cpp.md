# C++

## References (&)

### What I learned
Passing by reference avoids copying large objects and allows a function to work with the original object.

### Why it matters
Use references when:
- a function needs to modify an object, or
- the object is large and copying it would be inefficient.

Example:
```cpp
void drawHUD(cv::Mat& frame);
```

## const

### What I learned
Use `const` whenever a function should not modify an object.

### Why it matters
- communicates intent
- prevents accidental modification
- allows compiler checks

Example:
```cpp
const cv::Mat& frame
```

## auto vs Explicit Types

Prefer explicit types (`int`, `double`, `bool`) for simple variables.

Use `auto` when the type is long or obvious from the initializer.

## Header vs Source Files

Header (`.h`) files contain declarations.

Source (`.cpp`) files contain implementations.

## Configuration

Store project-wide constants in `Config.h`.

```cpp
namespace Config {
    constexpr int ESC_KEY = 27;
}
```

Access them as:

```cpp
Config::ESC_KEY
```

## Smart Pointers

### What I learned

Smart pointers automatically manage an object's lifetime.

Prefer `std::unique_ptr` (or `cv::Ptr` for OpenCV objects) over raw pointers when ownership is required.

### Why it matters

- prevents memory leaks
- automatically destroys owned objects
- clearly expresses ownership

Example:

```cpp
std::unique_ptr<IImageProcessor> processor =
    std::make_unique<GrayProcessor>();
```

---

## Interfaces & Polymorphism

### What I learned

An interface defines a common API that multiple classes implement.

Polymorphism allows different processor implementations to be accessed through the same interface.

### Why it matters

- modular architecture
- easy to extend
- removes large switch statements

Example:

```cpp
IImageProcessor* processor =
    processorManager.getProcessor(currentMode);

processor->process(frame);
```

---

## std::vector

### What I learned

`std::vector` is a dynamically sized array.

Useful for storing collections whose size changes at runtime.

### Why it matters

Examples:

```cpp
std::vector<cv::KeyPoint>
std::vector<cv::DMatch>
std::vector<std::vector<cv::DMatch>>
```

---

## Range-based for loop

### What I learned

Simplifies iterating over STL containers.

Prefer `const auto&` when only reading elements.

Example:

```cpp
for (const auto& match : matches)
{
    ...
}
```

---

## Namespace

### What I learned

Namespaces prevent name collisions and group related code.

Example:

```cpp
namespace vision
{
    class ORBProcessor { ... };
}
```

---

## constexpr

### What I learned

`constexpr` creates compile-time constants.

Prefer over macros for project constants.

Example:

```cpp
constexpr float LOWE_RATIO = 0.75f;
```
