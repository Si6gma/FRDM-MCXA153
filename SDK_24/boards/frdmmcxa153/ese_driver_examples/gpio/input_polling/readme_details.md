# GPIO Input - polling

Resources: *ese_driver_examples\gpio\input_polling*

## Goal

Know how to initialize and read a digital input pin by polling a bit in the GPIO 
module.

## Switches - Which pin to use?

The FRDM-MCXA153 board contains three switches. One of these switches is SW3, 
which is used in this example. However, before being able to read the logic 
state of SW3, the physical connection on the FRDM-MCXA153 board must be verified. 
Referring to the [board schematic page 7](./../../../../datasheets/SPF-90829_A1.pdf#page=7){:target="_blank"},
it shows that **PORT1 pin 7 (P1_7)** is connected to switch SW3 (a.k.a. WAKEUP).

The schematic also shows that an external pullup resistor is connected to SW3.
So, pressing SW3 connects it to GND, reading logic 0.

## Initialization

Similar to GPIO output pins, the GPIO modules must be used to read the logic 
state of a microcontroller pin. However, a pin must first be configured for the 
GPIO function. Using P1_7 as an example, it takes the following steps to 
configure the pin for GPIO:

1. Enable the PORT1 and GPIO1 modules in the MRCC module.
2. Initialize P1_7 for GPIO function in the PORT1 module.

These steps are explained in more detail in the following sections.
No additional action is needed, because the default direction of a pin is input.

### 1. Enable the PORT1 and GPIO1 modules

The MRCC module must be used to enable other modules, such as PORT1 and GPIO1. 
How this is done is described in the 
[reference manual paragraph 14.3](./../../../../datasheets/MCXAP64M96FS3RM.pdf#14.3%20Functional%20description){:target="_blank"}.

```C
// Before a module can be used, its clocks must be enabled (CC != 00) and it
// must be released from reset (MRCC_GLB_RST [peripherals name] = 1). If a
// module is not released from reset (MRCC_GLB_RST [peripherals name] = 0),
// an attempt to access a register within that module is terminated with a
// bus error.
```

This description translates to the following C instructions:

```C
// Enable modules and leave others unchanged
// PORT1: [1] = Peripheral clock is enabled
// GPIO1: [1] = Peripheral clock is enabled
MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT1(1);
MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO1(1);

// Release modules from reset and leave others unchanged
// PORT1: [1] = Peripheral is released from reset
// GPIO1: [1] = Peripheral is released from reset
MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT1(1);
MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO1(1);
```

### 2. Initialize P1_7 for GPIO function

The PORT module must be used to configure the function for a pin. How this is 
done is described in the 
[reference manual paragraph 11.4](./../../../../datasheets/MCXAP64M96FS3RM.pdf#11.4%20Initialization){:target="_blank"}.

```C
// 1. Initialize the pin functions:
//    -  Initialize single pin functions by writing appropriate values to
//       PCRn
//    -  Initialize multiple pins (up to 16) with the same configuration by
//       writing appropriate values to Global Pin Control Low (GPCLR) or
//       Global Pin Control High (GPCHR).
// 2. Lock the configuration for a given pin, by writing 1 to PCRn [LK], so
//    that it cannot be changed until the next reset.
```

There is a PCR register for each and every pin. The reference manual describes 
the PCR registers in detail. The PCR register for P1_7 is addressed as follows:

```C
PORT1->PCR[7]
```

This register is used as follows for pin P1_7 to:

- Set the LK bit field to [1]: Lock this PCR.
- Set the IBE bit field to [1]: Input Buffer Enable.
- Set the MUX bit field to [0000]: alternative 0 (GPIO).
- Set all other bit fields to their reset value logic 0.

```C
// 1. & 2.
//
// Configure pins
// LK : [1] = Locks this PCR
// INV: [0] = Does not invert
// IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
//            functions
// MUX: [0000] = Alternative 0 (GPIO)
// DSE: [0] = low drive strength is configured on the corresponding pin,
//            if the pin is configured as a digital output
// ODE: [0] = Disables
// SRE: [0] = Fast
// PE:  [0] = Disables
// PS:  [0] = n.a.
PORT1->PCR[7] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);
```

## Reading the pin logic state

After initialization the pin logic state can be read from 
[PDIR](./../../../../datasheets/MCXAP64M96FS3RM.pdf#12.7%20Memory%20map%20and%20register%20definition){:target="_blank"}
register in the GPIO peripheral. For example:

```C
while(1)
{
    // SW3 pressed?
    if((GPIO1->PDIR & (1<<7)) == 0)
    {
        // Green LED on
        GPIO3->PCOR = (1<<13);
    }
    else
    {
        // Green LED off
        GPIO3->PSOR = (1<<13);
    }
}
```

Suppose the value of the GPIO1->PDIR register is 0x034045A6 and the if-statement
is executed. The question is: *Will the green LED be on or off?*

The answer to this questions depends on the value of the register (which in turn 
depends on the connected hardware) and the mask.

![Bitwise and](./../../../../imgs/gpio_bitwise_and.png)

The result of the bitwise-and operator is not equal to zero, so the expression 
in the if-statement is false. The green LED will be off.

## Assignment

- Also initialize the pin connected to SW2 as GPIO input pin.
- Update main as follows:
    - If SW3 is pressed the green LED switches on
    - If SW2 is pressed the green LED switches off

[Solution](./solution.md){:target="_blank"}
