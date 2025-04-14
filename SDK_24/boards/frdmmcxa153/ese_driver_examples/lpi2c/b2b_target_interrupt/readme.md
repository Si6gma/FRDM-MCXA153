Overview
========
Target mode by using interrupts for transmitting and receiving data.

- The target address is 0x7E.
- The controller can write a single byte to the target. The byte controls the target RGB LED as follows: XXXXXBGR
    - bit 0 controls the red LED: 1 is on, 0 is off
    - bit 1 controls the green LED: 1 is on, 0 is off
    - bit 2 controls the blue LED: 1 is on, 0 is off
    - X means don't care
- The controller can read from the target. The target always returns the following bytes after a write command:
    - 0x01
    - 0x02
    - 0x03

Hardware requirements
=====================
- 2 FRDM-MCXA153 boards
- 2 Type-C USB cables
- Jumper wires
- 4k7 Ohm pullup resistors, available on the Shield V3

Board settings
==============
- Connect two boards/shields as follows:

|Controller|Target|
|---|---|
|SDA|SDA|
|SCL|SCL|
|GND|GND|

Preparation
===========
- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target boards.
- Build the applications:
    - b2b_controller_interrupt
    - b2b_target_interrupt
- Run or debug the applications.

Result
======
- The controller is able to control the RGB state of the target.

