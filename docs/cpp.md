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
