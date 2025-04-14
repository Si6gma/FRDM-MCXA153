Overview
========
Demonstrates how to implement a cyclic executive with interrupt scheduler.

Details
====================
Do you want to know more details about this example? Check the [details](./readme_details.md){:target="_blank"}.

Hardware requirements
=====================
- FRDM-MCXA153 board
- Type-C USB cable

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
- Open the VCOM (115200-8n1) in a terminal application of your choice.
- Press SW3. The green LED will be on for 3 seconds. The terminal application will log two events, the switch being pressed and the timeout.
- Press the spacebar in the terminal application to change the LED colour.
