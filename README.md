# RubiksCubeRobot
Rubik’s Cube solving robot built with 8 servo motors and an ESP32, using a mobile phone application to scan and solve the cube.

This project is an improved version of a previous Rubik’s Cube solving robot. I upgraded the hardware by replacing the Arduino Nano with an ESP32, eliminating the need for a separate Bluetooth module thanks to the ESP32’s built-in connectivity.

I also modified parts of the code to ensure full compatibility with the ESP32 and to improve overall performance.

Additionally, I redesigned the GitHub documentation to include clear, step-by-step instructions, making it easy for anyone to build this project using only this repository, even without prior experience.

## 1. Hardware

The hardware files can be found in the **RubiksCubeRobot-main** repository, inside the **Hardware** folder.

As a base support, I used a 28 cm x 28 cm wooden board. All the components are mounted on it using screws and washers.
![1bfb5039-195c-49a9-8c7a-25b2f29bcd08](https://github.com/user-attachments/assets/a7e02409-c7f3-41cf-a8e5-c2c7d38b13b7)
![b37cb933-e48f-48ad-b5dd-789842bbda0c](https://github.com/user-attachments/assets/266c9808-e7a8-4745-a276-d1b3f144c74f)


In this folder, you will find two Fusion 360 files if you want to explore the full assembly. However, for building the robot, you will mainly need the STL files, which contain all the parts required for 3D printing.

### 🧩 Required 3D Printed Parts

* 2x ChannelTopMirror.stl

* 2x ChannelTopNormal.stl

* 4x Grabber-With cutout.stl

* 2x SliderMirror.stl

* 2x SliderNormal.stl

* 4x ChannelBot.stl

* 4x Drill_Jig.stl

* 4x gear.stl

These quantities represent all the components needed to fully assemble the robot.

<img width="1900" height="647" alt="3D_print" src="https://github.com/user-attachments/assets/9d5489df-4284-4341-8e49-20f1daf5939e" />

