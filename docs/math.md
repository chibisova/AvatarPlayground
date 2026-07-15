## Rotation Matrix Intuition

A rotation matrix **does not store points**.

Instead, it represents **an operation** that rotates any 3D point.

```cpp
cv::Mat R;
```

Think of it as:

```
Point
  │
  ▼
Rotation Matrix
  │
  ▼
Rotated Point
```

### Common Misconception

The columns **do not** mean:

- Column 1 → rotation around X
- Column 2 → rotation around Y
- Column 3 → rotation around Z

Instead, the **entire matrix together** represents one rotation.

### What do the columns represent?

Each column tells you **where one of the original coordinate axes ends up after the rotation**.

Example:

```
R =
| 0  -1   0 |
| 1   0   0 |
| 0   0   1 |
```

Columns:

```
Column 1 → (0,1,0)
```

The original **X-axis** now points along **+Y**.

```
Column 2 → (-1,0,0)
```

The original **Y-axis** now points along **−X**.

```
Column 3 → (0,0,1)
```

The **Z-axis** remains unchanged.

### Visual intuition

Before rotation:

```
      Z
      ↑
      │
Y ←───●───→ X
```

After rotation:

```
      ↗ Z'
     /
    /
Y' ●
    \
     \
      X'
```

A rotation matrix tells us where the local coordinate axes move after the rotation.

This interpretation is widely used in:
- Computer Vision
- Robotics
- Computer Graphics
- Game Engines (Unity, Unreal)
