Overview
========
Demonstrates how to add an assembly source file (*.s) to a project.

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
- Single step through the application.

Result
======
- Start the debugger and single step through the code. Make sure the disassembly window is selected and that the core registers are visible.
- At every step, one or more core registers and/or the stack is updated. Make sure to understand why these registers are updated and what information is stored on the stack.
