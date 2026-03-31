# RubiksCubeRobot
Rubik’s Cube solving robot built with 8 servo motors and an ESP32, using a mobile phone application to scan and solve the cube.

This project is an improved version of a previous Rubik’s Cube solving robot. I upgraded the hardware by replacing the Arduino Nano with an ESP32, eliminating the need for a separate Bluetooth module thanks to the ESP32’s built-in connectivity.

I also modified parts of the code to ensure full compatibility with the ESP32 and to improve overall performance.

Additionally, I redesigned the GitHub documentation to include clear, step-by-step instructions, making it easy for anyone to build this project using only this repository, even without prior experience.

## 1. Hardware

The hardware files can be found in the **RubiksCubeRobot-main** repository, inside the **Hardware** folder.

As a base support, I used a 28 cm x 28 cm wooden board. All the components are mounted on it using screws and washers.
<div align="left">
  <img width="475" height="475" alt="Pic1" src="https://github.com/user-attachments/assets/a7e02409-c7f3-41cf-a8e5-c2c7d38b13b7" />
</div>

In this folder, you will find two Fusion 360 files if you want to explore the full assembly. However, for building the robot, you will mainly need the STL files, which contain all the parts required for 3D printing.

<div align="left">
  <img width="475" height="475" alt="structure" src="https://github.com/user-attachments/assets/3dc8e093-9273-48aa-ab30-6aff71bbc0e4" />
</div>

### Required 3D Printed Parts

* 2x ChannelTopMirror.stl

* 2x ChannelTopNormal.stl

* 4x Grabber-With cutout.stl

* 2x SliderMirror.stl

* 2x SliderNormal.stl

* 4x ChannelBot.stl

* 4x Drill_Jig.stl

* 4x gear.stl


<img width="475" height="162" alt="3D_print" src="https://github.com/user-attachments/assets/9d5489df-4284-4341-8e49-20f1daf5939e" />

###  Assembly

Follow the layout shown in the image below to correctly position all the 3D printed parts on the wooden base.

Make sure to:

* Place the sliders symmetrically
* Align the grabbers with the cube position
* Secure all parts tightly to ensure stability during operation

<img width="272" height="353" alt="act" src="https://github.com/user-attachments/assets/5facaf70-6079-4241-9b81-beec97b4d7d7" />


Power Supply:
For the servo motors, I used an external power supply rated at 5V and 5A to ensure stable operation and enough current for all components to work properly.
<img width="272" height="353" alt="act" src="https://github.com/user-attachments/assets/5c829dee-9262-445c-9d4a-9a6d238b3dba" />




## 2. Software

The software has been modified to work with the ESP32, making the system much more efficient compared to using an Arduino Nano with a separate Bluetooth module. Thanks to the ESP32’s built-in capabilities, the setup is simpler and more reliable.

The mobile application remains unchanged and works well with this setup.

To install the application on your phone, you need to enable **Android Developer Mode** and install the app manually using a PC (sideloading).

<details>
<summary> Pin Configuration</summary>

```cpp
#define RIGHT_SLIDER_PIN 13
#define RIGHT_SPINNER_PIN 14
#define LEFT_SLIDER_PIN 27
#define LEFT_SPINNER_PIN 26
#define FRONT_SLIDER_PIN 25
#define FRONT_SPINNER_PIN 33
#define BACK_SLIDER_PIN 32
#define BACK_SPINNER_PIN 19 // GPIO19
```

</details>

##  Workflow: Solving a Cube

1. **Connection**

   * User starts the app
   * Connects to the robot via Bluetooth (HC-06 / ESP32)

2. **Scan Phase**

   * Open the **Scan** tab
   * The robot rotates to expose each face
   * User taps **"Scan Face"** for all 6 faces
   * The app captures and processes the images

3. **Processing**

   * The app converts color detection into **Kociemba format**
   * The data is sent to a two-phase solver (embedded `.jar`)
   * An optimal move sequence is generated

4. **Solving**

   * User sets the desired speed
   * Taps **"Solve"**
   * The app sends the MOVE command with the solution sequence
   * The ESP32 executes the servo movements
   * The phone listens for **IDLE/BUSY** status updates

5. **Done**

   * The cube is solved
   * The robot returns to the rest position







