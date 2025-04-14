Overview
========
Write to a 1602 LCD with an I2C piggy-back.

Hardware requirements
=====================
- FRDM-MCXA153 board
- Type-C USB cable
- 1602 LCD with I2C piggy-back

Board settings
==============
- Default

Preparation
===========
- Connect the FRDM-MCXA153 board and the LCD I2C piggy-back board as follows

    |   FRDM-MCXA153   | LCD |
    |:----------------:|:---:|
    | P3_27/LPI2C0_SCL | SCL |
    | P3_28/LPI2C0_SDA | SDA |
    | 5V0              | VCC |
    | GND              | GND |

- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target board.
- Build the application.
- Run or debug the application.

Result
======
- Open the VCOM (115200-8n1) in a terminal application of your choice. A timeout message is printed every second.
- On the LCD:
    - the first line shows a seconds counter with several leading zeros.
    - the second line shows the message "made by ESE".
- Press SW2. The LCD backlight will switch on.
- Press SW3. The LCD backlight will switch off.
