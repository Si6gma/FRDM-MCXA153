# Assignment - Neopixel

Resources: *ese_shieldv3_examples\neopixel\flexpwm*

## Goal

To control neopixels with the FlexPWM module.

## Hardware requirements

- FRDM-MCXA153 board
- Three neopixels (SK6812 or compatible)
- Type-C USB cable

## Neopixels

Neopixels are RGB LEDs that are controller by sending 24-bits over is single wire.

```txt
              3V3
               │
         ┌─────┴────┐ 
 24-bits │ neopixel │
────────►│          │────────►
         └─────┬────┘
               │
              GND
```

Each colour is controlled by 8-bits. Sending a single bit takes approximately 1us. A pause greater than 50us is an indication to show the transmitted value on the LEDs.

The width of a pulse determines if a logic 0 or 1 is transmitted. The period of a pulse is always the same. This is depicted in the following timing diagram, with example values for the [SK6812](https://cdn-shop.adafruit.com/product-files/1138/SK6812+LED+datasheet+.pdf){:target="_blank"} neopixel.

```txt        
logic 0 ┌────┐          ┌        
        ┘    └──────────┘
        │◄──►│◄────────►│
          T0H     T0L
         300ns   900ns
        
logic 1 ┌───────┐       ┌        
        ┘       └───────┘
        │◄─────►│◄─────►│
           T1H     T1L
          600ns   600ns
```

For a single neopixel, 24 of these pulses must be transmitted. If one pulse takes 1.2 us, 24 pulses will take 28.8 us.
Furthermore, the 24-bit data structure is as follows:

```txt
        Green            Red            Blue
  |◄─────────────►|◄─────────────►|◄─────────────►|
  ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐  
  │7             0│7             0│7             0│ ─►t
```

Multiple neopixels can be connected in cascade. The first 24-bits that are transmitted will appear on the latest neopixel in the chain.

## Functional requirements

The application uses three neopixels connected to P3_8. An LED chaser is created as follows:

- The LED chaser runs with 10 Hz.
- At startup the first neopixel is red, the others are off.
- After 100 ms, the second neopixel is red, the others are off.
- After another 100 ms, the third neopixel is red, the others are off.
- After another 100 ms, the first neopixel is green, the others are off.
- After another 100 ms, the second neopixel is green, the others are off.
- After another 100 ms, the third neopixel is green, the others are off.
- After another 100 ms, the first neopixel is blue, the others are off.
- After another 100 ms, the second neopixel is blue, the others are off.
- After another 100 ms, the third neopixel is blue, the others are off.
- Pressing SW3 changes the LED chaser direction.

## Architecture

The [Shield V3](./../../docs/FRDM-MCXx_Shield_V3.0_schematic.pdf#page=4){:target="_blank"} shows an example of how to connect the hardware parts.

Notice from the timing diagram of a single neopixel that the period for a logic 0 and 1 is the same. As only the width of the pulse changes, a PWM signal with one of two duty cycles can be used to control the neopixels. This will require the use of a timer with PWM capabilities and updating the match value when there is a reload interrupt. The FlexPWM module will be used to realize this. The basic idea for implementation is depicted in the following timing diagram.


```txt

VAL1 (?) - - - - - - - + - - - - - - + - - - - - - + - - - - - - +
                      /|            /|            /|            /|
                     / |           / |           / |           / |
                    /  |          /  |          /  |          /  |
                   /   |         /   |         /   |         /   |
                  /    |        /    |        /    |        /    |
                 /     |       /     |       /     |       /     |
                /      |      /      | - - -+-VAL3-|-     /      |
               /       |     /       |     /.      |     /       |
              /        |    /        |    / .      |    /        |
             /         | - + - VAL3 -|-  /  .      | - + - VAL3 -|-
            /          |  /.         |  /   .      |  /.         |
           /           | / .         | /    .      | / .         |
VAL3 (0)  /            |/  .         |/     .      |/  .         |
INIT (0) +──── VAL3 ───+─────────────+─────────────+─────────────+─
VAL2 (0)               .   .         .      .      .   .         .
VAL4 (0)               .   .         .      .      .   .         .
                       ┌───┐         ┌──────┐      ┌───┐         ┌
PWM_n    ──────────────┘   └─────────┘      └──────┘   └─────────┘                     
                       │T0H          │ T1H         │T0H
                       ▼             ▼             ▼
                    Reload        Reload        Reload
                   interrupt     interrupt     interrupt
                      ISR           ISR           ISR
│               │      │             │             │
├───────────────┤      │             │             │
│T0H match value│◄─────┘             │             │
│T1H match value│◄───────────────────┘             │
│T0H match value│◄─────────────────────────────────┘
│      etc.     │
│      ...      │
├───────────────┤
│               │
 Array in memory
 storing the T0H
 and T1H values
 for each bit.
```

This interrupt driven approach offloads the CPU. At every FlexPWM reload interrupt, a new VAL3 match value will be read from an array in memory. This means that this array has to be prepared before the initial transmission starts and that it will use quit some RAM memory. In fact, it will use 24 times a 16-bit number (uint16_t) for every neopixel.

It is also important to note that the time between the *reload interrupt* and the fastest match is only 300ns. Running at 48 MHz, this means that only 14.4 instructions can be executed. This is not enough for both context switching en ISR code execution. Running at 96 MHz instead, will allow to execute twice as many instructions which is enough, provided that optimization is enabled.

## API

In order to create the functional requirements, the following API functions are prepared.

```C
void neopixel_init(void);
void neopixel_update(const rgb_t *pixels);
```

## Assignments

`Q1` Although the CPU is clocked at 96 MHz, the FlexPWM module timer is clocked with 48 MHz. Examine the code example. What is the TOP (VAL1) value?

`Q2` What is the time between two reload interrupt events?

`Q3` Given the following compare values, what is the duration for T0H, T1H, T0L, and T1L?

- T0H = 15
- T1H = 30

`Q4` Use a logic analyzer to confirm these durations.

`Q5` Make the chaser change direction by pressing SW3.

[Solution](./solution.md){:target="_blank"}
