# Computer Vision Project Portfolio

This repository contains C++ implementations of various computer vision systems using the OpenCV library. The projects range from interactive video processing to ADAS (Advanced Driver Assistance Systems) features and Deep Learning-based object detection.

## Table of Contents

1.  [Interactive Multi-Effect Video Player](https://www.google.com/search?q=%231-interactive-multi-effect-video-player)
2.  [Smart Car Detection System](https://www.google.com/search?q=%232-smart-car-detection-system)
3.  [YOLO Object Detection](https://www.google.com/search?q=%233-yolo-object-detection)
4.  [Dependencies & Setup](https://www.google.com/search?q=%23dependencies--setup)

-----

## 1\. Interactive Multi-Effect Video Player

**File:** `Multi_Effect_Video_Player.c++` (also `22300446-4.cpp`)

A real-time video processing application that allows users to toggle various image processing algorithms on the fly using keyboard shortcuts.

### Features & Controls

| Key | Effect | Description |
| :--- | :--- | :--- |
| **`n`** | **Negative** | Inverts the pixel values of the video frames. |
| **`g`** | **Gamma Correction** | Adjusts brightness using non-linear gamma transformation (power law). |
| **`h`** | **Histogram Equalization** | Enhances contrast by equalizing the histogram of the V channel (HSV). |
| **`s`** | **Skin Color Isolation** | Isolates skin tones (Hue 9-23) by desaturating all other colors. |
| **`c`** | **Color Shift** | Modifies the Hue channel to drastically alter the color palette. |
| **`a`** | **Average Blur** | Applies a 9x9 box blur filter. |
| **`u`** | **Unsharp Masking** | Sharpens the image by subtracting a blurred version from the original. |
| **`w`** | **White Balancing** | Performs automatic white balancing using the Gray World assumption. |
| **`r`** | **Reset** | Returns to the original raw video feed. |
| **`Esc`**| **Exit** | Closes the application. |

**Usage Note:** Ensure a file named `video.mp4` is present in the working directory.

-----

## 2\. Smart Car Detection System

**File:** `smartCarDetectionSystem.cpp`

A safety system designed for automotive applications, combining Lane Departure Warning (LDW) and Forward Vehicle Start Notification (FVSN).

### Key Features

  * **Lane Departure Warning:** Uses Canny Edge Detection and Probabilistic Hough Transform to identify lane lines. If the calculated lane position crosses a defined threshold relative to the frame width, a "Lane Departure\!" alert is triggered.
  * **Start Moving Notification:** Utilizes Background Subtraction (MOG2) to detect motion. It specifically monitors a Region of Interest (ROI) for the car ahead. If the car ahead moves (high motion in ROI) while the background remains static, it displays "Start Moving\!".

**Usage Note:** Requires a video file named `Project2_video.mp4`.

-----

## 3\. YOLO Object Detection

**File:** `object_detection_YOLO.c++`

An implementation of the YOLO (You Only Look Once) object detection algorithm using OpenCV's DNN (Deep Neural Network) module.

### Functionality

  * Loads a pre-trained **YOLOv2-tiny** model (`.cfg` and `.weights`).
  * Performs forward pass detection on video frames.
  * Filters detections based on a confidence threshold (\> 0.24).
  * Draws bounding boxes and labels around detected objects (e.g., cars, pedestrians).

**Required Models & Files:**

  * Video: `deep/downtown_road.mp4`
  * Config: `deep/yolov2-tiny.cfg`
  * Weights: `deep/yolov2-tiny.weights`
  * Classes: `deep/coco.names`

-----

## Dependencies & Setup

### Prerequisites

  * **C++ Compiler** (GCC, Clang, or MSVC)
  * **OpenCV 3.x or 4.x** (Modules required: `core`, `highgui`, `imgproc`, `videoio`, `dnn`, `video`)

### Build Instructions

To compile the files, link against the OpenCV libraries. Example using `g++`:

```bash
g++ Multi_Effect_Video_Player.c++ -o video_player `pkg-config --cflags --libs opencv4`
g++ smartCarDetectionSystem.cpp -o smart_car `pkg-config --cflags --libs opencv4`
g++ object_detection_YOLO.c++ -o yolo_detect `pkg-config --cflags --libs opencv4`
```

*Note: Adjust source filenames and OpenCV flags as per your local environment setup.*
