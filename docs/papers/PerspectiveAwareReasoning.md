# Perspective-Aware Reasoning in Vision-Language Models via Mental Imagery Simulation

- **GitHub:** https://github.com/KAIST-Visual-AI-Group/APC-VLM
- **Paper:** https://apc-vlm.github.io/static/Perspective_Aware_Reasoning_Preprint.pdf

## Problem

VLMs lack perspective-aware reasoning — i.e., they fail to understand a situation from an alternative viewpoint (that of an object in the scene, rather than the viewer/camera).

## Proposed Solution

> Similar to human **mental imagery**, make the model perceive the world through *abstracted representations*. The authors propose **Abstract Perspective Change (APC)**, a framework for perspective-aware reasoning that effectively leverages vision foundation models — object detection (GroundingDINO), segmentation (SAM + DepthPro), orientation estimation (OrientAnything) — to construct scene abstractions and enable perspective changes.

## Pipeline

### Stage 1: Scene Abstraction

Use pretrained models to reconstruct a 3D scene abstraction from a 2D image by selecting and extracting objects of interest.

**1. Extracting Objects of Interest**

- **Model:** Qwen2.5-VL-7B-Instruct
- **Input:** Image *I* and question *Q*
- **Task:** Identify the list of objects necessary for answering the question
- **Output:** List of objects of interest, specified by name

```
# Situation Description
Given an image and a spatial-reasoning
question, identify all entities mentioned
in the question.

# Example
[Question] You are standing at the
airplane's position, facing where it
is facing. Is the person on your left or
right?
[Detect] [airplane, person]

# Your Task
Now, given the question below, list the
entities that appear in the question.

[Question] {Question}
[Detect]
```

**2. Building Object Abstractions**

- **Models:** GroundingDINO, SAM, DepthPro, OrientAnything
- **Input:** List of objects of interest

| Model | Input | Output |
|---|---|---|
| GroundingDINO | List of objects of interest | 2D bounding box |
| SAM | Image cropped by 2D bounding box | Segmentation mask |
| DepthPro | Image | Metric depth map |
| OrientAnything | Cropped image | Object's frontal orientation in camera coordinate system |

*3D coordinates:* unproject 2D coordinates to 3D using the depth map from DepthPro.

*Output:* 3D position and orientation coordinates — *S*

### Stage 2: Perspective Change

Transform the scene abstraction from the camera's perspective to the reference viewer's perspective.

**1. Setting a Reference Perspective**

- **Input:** Spatial reasoning question
- **Task:** Pick one reference-viewer object from the list of objects of interest
- **Output:** Extracted reference perspective *A*

```
Given a question about spatial reasoning,
we want to extract the perspective of the
question. If the question is from the
camera's perspective, return ++camera++.

# Example
[Question] From the woman's perspective,
is the tree on the left or right?
[Perspective] ++woman++

# Your Task
Given the question below, please specify
the perspective from which the question
is asked.

You must return in the format:
[Perspective] ++object name++
[Question] {Question}
[Options] obj1, obj2, ..., camera
[Perspective]
```

**2. Transforming Scene Abstraction**

- **Task:** Apply a coordinate transformation from the camera's frame to that of the reference viewer *A*. Reference viewer *A* is placed at the origin, with its orientation aligned to the z-axis.
- **Output:** *S_a* — the resulting abstraction

### Stage 3: Perspective Prompting

Generate a prompt (the **perspective prompt**) from the transformed scene abstraction *S_a* to feed back to the VLM.

**1. Numerical (Textual) Prompt**

- **Input:** Each object's textual description, 3D position, and orientation

```
This is an image of a 3D scene.
- The viewer is facing towards the object
that is closest to the center.
- A larger object is closer to the viewer
compared to a smaller object.

# Task
Based on the image, please answer the
following question.

{Question}
Please only return the answer.
```

- **Output:** Answer to the question

**2. Visual Prompt**

- **Input:** *S_a*
- **Process:** Assign each object a cube and render from reference viewer *A*'s vantage point → get an image from the new viewpoint. Each cube gets its own color; object names in the prompt are replaced with their corresponding colors.

```
Imagine that you are at the {src obj}'s
position and facing where it is facing.
We have the coordinates of different
objects in {src obj}'s coordinate system.

# Coordinate System
- The origin is at the {src obj}'s
position.
- The {src obj}'s facing direction is [0,
0, 1], which is aligned with the z-axis.
- The x-axis is to the right, the y-axis
is up, and the z-axis is forward.

# Object Coordinates
[...]

# Task
Given the above {src obj}'s coordinate
system and the object coordinates, please
answer the following question:
[Question] {Question}
```

- **Output:** Answer to the original question

## Q&A / Notes

> **Q: Why not use SVD/PCA for orientation?**
>
> A: PCA gives the axis of *greatest variance* in a point cloud. That works nicely for elongated, asymmetric shapes (a pen, a car, a fish) where "long axis = facing direction" is a decent bet. But it breaks for complex shapes, such as a human:
> 1. A standing human's greatest-variance axis is *vertical* (height dominates width/depth) — that tells you nothing about whether they're facing left, right, toward camera, or away.
> 2. PCA can't resolve a 180° ambiguity even when it does find a sensible axis — it'll tell you "the object is oriented along this line," not which *end* is the front.
