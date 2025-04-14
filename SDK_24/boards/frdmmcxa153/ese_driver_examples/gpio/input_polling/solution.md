# Solution

Solution to the assignment.

# gpio_input_polling.c

```C
void gpio_input_init(void)
{
    // From section 14.3 Functional description (NXP, 2024)
    //
    // Before a module can be used, its clocks must be enabled (CC != 00) and it
    // must be released from reset (MRCC_GLB_RST [peripherals name] = 1). If a
    // module is not released from reset (MRCC_GLB_RST [peripherals name] = 0),
    // an attempt to access a register within that module is terminated with a
    // bus error.

    // Enable modules and leave others unchanged
    // PORT1: [1] = Peripheral clock is enabled
    // GPIO1: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT1(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO1(1);

    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    // Release modules from reset and leave others unchanged
    // PORT1: [1] = Peripheral is released from reset
    // GPIO1: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT1(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO1(1);

    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    // From section 11.4 Initialization (NXP, 2024)
    //
    // 1. Initialize the pin functions:
    //    -  Initialize single pin functions by writing appropriate values to
    //       PCRn
    //    -  Initialize multiple pins (up to 16) with the same configuration by
    //       writing appropriate values to Global Pin Control Low (GPCLR) or
    //       Global Pin Control High (GPCHR).
    // 2. Lock the configuration for a given pin, by writing 1 to PCRn [LK], so
    //    that it cannot be changed until the next reset.

    // 1. & 2.
    //
    // Configure pin P1_7 and P3_29
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

    PORT3->PCR[29] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);
}

```

---

# main.c

```C
int main(void)
{
    gpio_input_init();
    gpio_output_init();

    while(1)
    {
        // SW3 pressed?
        if((GPIO1->PDIR & (1<<7)) == 0)
        {
            // Green LED on
            GPIO3->PCOR = (1<<13);
        }
        
        // SW2 pressed?
        else if((GPIO3->PDIR & (1<<29)) == 0)
        {
            // Green LED off
            GPIO3->PSOR = (1<<13);
        }
    }
}
```
