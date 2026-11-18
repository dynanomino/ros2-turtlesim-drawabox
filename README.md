# ROS 2 Turtlesim Draw-A-Box Demo

This repo is a three-package ROS 2 (C++) workspace demonstrating basic competency in using topics, services, the publisher-subscriber node architecture, parameters and XML launch files.
Turtlesim is the target system, instead of a physical hardware setup.

The goal is for each spawned turtle robot in Turtlesim to move in a square path, despawned by a service, then a new robot is spawned via another service.

<img width="1280" height="720" alt="ROS2 Turtlesim Box Compress" src="https://github.com/user-attachments/assets/d4a13464-fb6d-4905-b061-b2dc67241fbd" />

## Architecture

<img width="1176" height="699" alt="ROS2 Turtlesim Drawbox Nodes" src="https://github.com/user-attachments/assets/8d6874f7-ba17-46fc-aeff-6ef789a7ecc4" />

## Prerequisites

- ROS 2 (developed and tested on **Humble**, OS: Ubuntu 22.04)
- Turtlesim

```bash
sudo apt install ros-$ROS_DISTRO-turtlesim
```

## Build

```bash
git clone https://github.com/dynanomino/ros2-turtlesim-drawabox.git
cd ros2-turtlesim-drawabox
source /opt/ros/$ROS_DISTRO/setup.bash
rosdep install --from-paths src --ignore-src -r -y
colcon build
source install/setup.bash
```

## Run

```bash
ros2 launch my_robot_bringup turtle1_controller.launch.xml
```

## What you should see

The turtlesim window should pop open, and the controller node will drive the turtle.
The terminal will log when the spawn and despawn services are processed

## Compatibility

This was tested on ROS 2 Humble, it is recommended to build from source if any issues arise in importing.

## Thank You For Reading!
