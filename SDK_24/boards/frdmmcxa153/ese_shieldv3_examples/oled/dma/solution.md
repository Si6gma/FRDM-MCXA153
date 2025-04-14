# Solution

Solution to the assignment.

`Q1` The I2C address is described in section *8.1.5 MCU I2C Interface*. It depends on the connection of the pin SA0. Assuming SA0 is connected to GND, then the I2C address for writing is 0b01111000 (0x78) and for reading is 0b01111001 (0x79).

`Q2` The array of initialisation commands is located in *ssd1306.c* and is called *lpi2c0_controller_init_commands*.

`Q3` In order to set inverse display, the command 0xA7 must be transmitted.
