Overview
========
Use a HCSR04 ultrasonic range sensor to measure the distance to an object.

Three different solutions are demonstrated. Each solution is selected by the define SOLUTION in the file hcsr04.h.

Hardware requirements
=====================
- FRDM-MCXA153 board
- Type-C USB cable
- HCSR04 ultrasonic range sensor

Board settings
==============
- Default

Preparation
===========
- Connect the FRDM-MCXA153 board and the HCSR04 as follows

    |   FRDM-MCXA153   | HCSR04 |
    |:----------------:|:------:|
    | P3_11            | TRIG   |
    | P3_1             | ECHO   |
    | 5V0              | VCC    |
    | GND              | GND    |

- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target board.
- Build the application.
- Run or debug the application.

Result
======
- Open the VCOM (115200-8n1) in a terminal application of your choice.
- The measured distance is printed 10 times per second.
