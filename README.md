

# **rrst-ros2-workspace**
## 1. Overview
専門講習で使うリポジトリです。メインリポジトリのフォークになっています。

## 3. Getting Started

### 3.1 Create a Workspace
Skip this step if the workspace has already been created.

    mkdir -p ~/ros2_ws/src

### 3.2 Clone the Repository

**main (stable)**

    cd ~/ros2_ws/src
    git clone git@github.com:RRST-NHK-Project/rrst-ros2-ws-practice.git .
    git submodule update --init --recursive

**develop (latest, in progress)**

    cd ~/ros2_ws/src
    git clone git@github.com:RRST-NHK-Project/rrst-ros2-ws-practice.git -b develop .
    git submodule update --init --recursive

### 3.3 Build

    cd ~/ros2_ws
    colcon build

To specify the number of build threads:

    cd ~/ros2_ws
    colcon build --parallel-workers 4

---

## 4. Credits
Developed by NHK Project, RRST, Ritsumeikan University, Japan (2024)
- Official Website: https://www.rrst.jp
- X (Twitter): https://x.com/RRST_BKC

![Logo](https://www.rrst.jp/img/logo.png)

---

