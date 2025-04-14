# Assignment - Shift register

Resources: *ese_shieldv3_examples\shiftregister\lpspi*

## Goal

To practice with the modules discussed so far.

## Hardware requirements

- FRDM-MCXA153 board
- HC595 shift register (and 8 LEDs for visualisation of the outputs), available on Shield V3
- Type-C USB cable
- Logic analyzer (optional)

## Functional requirements

The application controls the 8 outputs of a shift register. The shift register is controlled with an SPI interface.

## Architecture

The [Shield V3](./../../docs/FRDM-MCXx_Shield_V3.0_schematic.pdf#page=2){:target="_blank"} shows an example of how to connect the hardware parts. Notice that the MISO pin and QH' pin are not connected.

## API

In order to create the functional requirements, the following API functions are prepared.

```C
/*!
 * \brief Initializes the HC595 pins
 *
 * Resources:
 * - GPIO output | P3_1
 * - LPSPI1
 *
 * Pin P3_1 is configured as an GPIO output pin. The pin is toggled from low to
 * high in order to trigger the SRCLR pin of the HC595. It furthermore 
 * configures LPSPI1 as a master by calling the lpspi1_master_init() function.
 */
void hc595_init(void);

/*!
 * \brief Writes a single byte to the shift register
 *
 * Uses the lpspi1_master_tx() function for transmitting the data.
 *
 * \param[in] data Data item to transmit
 */
void hc595_write(uint8_t data);
```

Implement these functions. In main, use these functions to create an LED chaser. The behaviour is depicted in the following sequence diagram:

```txt
                                               lpspi1           shift
     main              hc595                   txfifo  gpio3     reg.
     ---                ---                     ---     ---      ---
      |                  |                       |       |        |
      | hc595_init       |                       |       |        |
      +----------------->|                       |       |        |
      |                  | init P3_1             |       |        |
      |                  +-----------------------|------>|        |
      |                  | P3_1 = SRCLR = 0      |       |        |
      |                  +-----------------------|------>|------->|
      |                  | P3_1 = SRCLR = 1      |       |        |
      |                  +-----------------------|------>|------->|
      |                  | lpspi1_master_init    |       |        |
      |                  +---------------------->|       |        |
      | hc595_write 0x01 |                       |       |        |
      +----------------->|                       |       |        |
      |                  | lpspi1_master_tx 0x01 |       |        |
   +->|                  +---------------------->|       |  0x01  |
   |  |                  |                       +-------|------->|
   +--+                  |                       |       |        |
 125ms|                  |                       |       |        |
      | hc595_write 0x02 |                       |       |        |
      +----------------->|                       |       |        |
      |                  | lpspi1_master_tx 0x02 |       |        |
   +->|                  +---------------------->|       |  0x02  |
   |  |                  |                       +-------|------->|
   +--+                  |                       |       |        |
 125ms|                  |                       |       |        |
      | hc595_write 0x04 |                       |       |        |
      +----------------->|                       |       |        |
      |                  | lpspi1_master_tx 0x04 |       |        |
   +->|                  +---------------------->|       |  0x04  |
   |  |                  |                       +-------|------->|
   +--+                  |                       |       |        |
 125ms|                  |                       |       |        |  
```

TIP: Use the SysTick timer to create a [millisecond timer](../../../ese_driver_examples/systick/match_interrupt/readme_details.md){:target="_blank"} for the 125 ms delay in main.

[Solution](./solution.md){:target="_blank"}
