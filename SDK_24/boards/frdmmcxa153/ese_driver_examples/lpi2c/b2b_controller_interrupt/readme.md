Overview
========
Controller mode by using interrupts for transmitting and receiving data.

Hardware requirements
=====================
- 2 FRDM-MCXA153 boards
- 2 Type-C USB cables
- Jumper wires
- 4k7 Ohm pullup resistors, available on the Shield V3

Board settings
==============
- Connect two boards/shields as follows:
    - SDA - SDA
    - SCL - SCL
    - GND - GND

Preparation
===========
- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target boards.
- Build the applications.
- Run or debug the applications.

Result
======
- The controller is able to control the RGB state of the target project *b2b_target_interrupt*.
