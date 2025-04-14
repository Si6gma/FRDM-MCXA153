Overview
========
Use GPIO inputs to read a rotary encoder.

Hardware requirements
=====================
- FRDM-MCXA153 board
- Type-C USB cable
- Shield V3

Board settings
==============
- Default

Preparation
===========
- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target board.
- Build the application.
- Run or debug the application.

Result
======
- The RGB LED blinks as follows:

    - Clockwise pulses > 0: green LED on
    - Clockwise pulses < 0: red LED on
    - Clockwise pulses = 0: LEDs off
    
- Pressing the encoder resets the clockwise pulses to 0.
