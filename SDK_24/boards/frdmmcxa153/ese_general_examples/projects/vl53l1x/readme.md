Overview
========
Read the VL53L1X time-of-flight sensor using LPI2C0 and the ultra lite driver (ULD) provided by STMicroelectronics.

Hardware requirements
=====================
- FRDM-MCXA153 board
- Type-C USB cable
- VL53L1X time-of-flight sensor

Board settings
==============
- Default

Preparation
===========
- Use jumper wires to make the following connections:

  |  FRDM-MCXA153   | VL53L1X |
  |:---------------:|:-------:|
  |             3V3 | VIN     |
  |             GND | GND     |
  |P3_27/LPI2C0_SCL | SCL     |
  |P3_28/LPI2C0_SDA | SDA     |
  |      P3_15/GPIO | GPIOI   |
  |              nc | XSHUT   |

- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target board.
- Build the application.
- Run or debug the application.

Result
======
- Open the VCOM (115200-8n1) in a terminal application of your choice. The following output is visiable:

    ```TXT
    VL53L1X ToF project - Debug
    Build Jan 26 2025 13:49:20
    VL53L1X Model ID        0xEA
    VL53L1X Module type     0xCC
    VL53L1X 0xEACC
    VL53L1X booted
    VL53L1X ULD version 3.5.2
    VL53L1X ULD running
    [00000033] 0, 606, 19376, 280, 148
    [00000054] 0, 594, 19920, 240, 151
    [00000074] 0, 604, 19928, 264, 151
    [00000094] 0, 606, 20000, 280, 151
    [00000114] 0, 613, 20000, 256, 151
    ```

    At every VL53L1X interrupt, the following information is printed:
    
    ```TXT
    [timestamp] range status, distance, signal_rate, ambient_rate, spad_num
    ```

- When an object is within 50cm, the green LED is on.
