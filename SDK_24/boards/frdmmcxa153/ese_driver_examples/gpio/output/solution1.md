# Solution

Solution to the assignment.

# gpio_output.c

```C
void gpio_output_init(void)
{
    // From section 14.3 Functional description (NXP, 2024)
    //
    // Before a module can be used, its clocks must be enabled (CC != 00) and it
    // must be released from reset (MRCC_GLB_RST [peripherals name] = 1). If a
    // module is not released from reset (MRCC_GLB_RST [peripherals name] = 0),
    // an attempt to access a register within that module is terminated with a
    // bus error.

    // Enable modules and leave others unchanged
    // GPIO3: [1] = Peripheral clock is enabled
    // PORT3: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC1 |= MRCC_MRCC_GLB_CC1_GPIO3(1);
    MRCC0->MRCC_GLB_CC1 |= MRCC_MRCC_GLB_CC1_PORT3(1);

    // Alternative
    // MRCC0->MRCC_GLB_CC1 |= (1<<0);
    // MRCC0->MRCC_GLB_CC1 |= (1<<8);

    // Release modules from reset and leave others unchanged
    // GPIO3: [1] = Peripheral is released from reset
    // PORT3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_PORT3(1);

    // Alternative
    // MRCC0->MRCC_GLB_RST1 |= (1<<0);
    // MRCC0->MRCC_GLB_RST1 |= (1<<8);

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
    // Configure P3_12
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [0] = Disables
    // MUX: [0000] = Alternative 0 (GPIO)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disables
    // PS:  [0] = n.a.
    PORT3->PCR[12] = 0x00008000;

    // From section 12.5 Initialization (NXP, 2024)
    //
    // 1. Initialize the GPIO pins for the output function:
    //    a. Configure the output logic value for each pin by using Port Data
    //       Output (PDOR).
    //    b. Configure the direction for each pin by using Port Data Direction
    //       (PDDR).
    // 2. Interrupt function not used.

    // 1. a.
    //
    // PDO12: [1] = Logic level 1 – LEDs off
    GPIO3->PDOR |= (1<<12);

    // 1. b.
    //
    // PDD12: [1] = Output
    GPIO3->PDDR |= (1<<12);
}
```

---

# main.c

```C
int main(void)
{
    gpio_output_init();

    while(1)
    {
        // LED red off
        // Write logic 1 to bit 12 in the GPIO3 PSOR register so the
        // corresponding bit in PDOR becomes 1
        GPIO3->PSOR = (1<<12);

        // Delay
        for(volatile int i=0; i<1000000; i++)
        {}

        // LED red on
        // Write logic 1 to bit 12 in the GPIO3 PCOR register so the
        // corresponding bit in PDOR becomes 0
        GPIO3->PCOR = (1<<12);

        // Delay
        for(volatile int i=0; i<1000000; i++)
        {}
    }
}
```
