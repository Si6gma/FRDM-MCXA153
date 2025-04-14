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
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    // Release modules from reset and leave others unchanged
    // GPIO3: [1] = Peripheral is released from reset
    // PORT3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);    

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
    // Configure P3_13
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
    PORT3->PCR[13] = PORT_PCR_LK(1);
    PORT3->PCR[12] = PORT_PCR_LK(1);
    PORT3->PCR[0] = PORT_PCR_LK(1);

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
    // PDO13, PDO12 and PDO0: [1] = Logic level 1 – LEDs off
    GPIO3->PDOR |= (1<<13) | (1<<12) | (1<<0);

    // 1. b.
    //
    // PDD13, PDO12 and PDO0: [1] = Output
    GPIO3->PDDR |= (1<<13) | (1<<12) | (1<<0);
}

```

---

# encoder.c

```C
/*!
 * \brief Initializes the encoder pins
 *
 * Resources:
 * - ENC_A  | P3_31 | GPIO input with interrupts enabled on rising edges
 * - ENC_B  | P2_7  | GPIO input
 * - ENC_SW | P1_6  | GPIO input with interrupts enabled on both edges
 */
void encoder_init(void)
{
    // Enable modules and leave others unchanged
    // PORT1: [1] = Peripheral clock is enabled
    // GPIO1: [1] = Peripheral clock is enabled
    // PORT2: [1] = Peripheral clock is enabled
    // GPIO2: [1] = Peripheral clock is enabled
    // PORT3: [1] = Peripheral clock is enabled
    // GPIO3: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT1(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO1(1);
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    // Release modules from reset and leave others unchanged
    // PORT1: [1] = Peripheral is released from reset
    // GPIO1: [1] = Peripheral is released from reset
    // PORT2: [1] = Peripheral is released from reset
    // GPIO2: [1] = Peripheral is released from reset
    // PORT3: [1] = Peripheral is released from reset
    // GPIO3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT1(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO1(1);
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    // Configure pins P1_6
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0000] = Alternative 0 (GPIO)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [1] = Enables
    // PS:  [1] = Enables internal pullup resistor
    PORT1->PCR[6] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);

    // 1. n.a.
    // 2. Initialize the interrupt function by writing to Interrupt Control a
    //    (ICR0 - ICR31) for the corresponding pins and desired configuration.
    //    If the pin is previously used for a different function, first write
    //    0100_0000h to Interrupt Control a (ICR0 - ICR31) to disable the
    //    previous function and clear the flag.

    // 2.
    //
    // ISF: [1] = Clear the flag
    // IRQC : [1010] = ISF and interrupt on both edges
    GPIO1->ICR[6] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1011);

    // Enable GPIO1 interrupts
    NVIC_SetPriority(GPIO1_IRQn, 7);
    NVIC_ClearPendingIRQ(GPIO1_IRQn);
    NVIC_EnableIRQ(GPIO1_IRQn);


    // Configure pins P2_7 and P3_31
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0000] = Alternative 0 (GPIO)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [1] = Enables
    // PS:  [1] = Enables internal pullup resistor
    PORT2->PCR[7]  = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);
    PORT3->PCR[31] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);

    // 1. n.a.
    // 2. Initialize the interrupt function by writing to Interrupt Control a
    //    (ICR0 - ICR31) for the corresponding pins and desired configuration.
    //    If the pin is previously used for a different function, first write
    //    0100_0000h to Interrupt Control a (ICR0 - ICR31) to disable the
    //    previous function and clear the flag.

    // 2.
    //
    // ISF: [1] = Clear the flag
    // IRQC : [1010] = ISF and interrupt on rising edges
    GPIO3->ICR[31] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1001);

    // Enable GPIO3 interrupts
    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);
}

void GPIO1_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO1_IRQn);

    // Interrupt handler triggered by ENC_SW?
    if((GPIO1->ISFR[0] & GPIO_ISFR_ISF6(1)) != 0)
    {
        // Clear the flag
        GPIO1->ISFR[0] = GPIO_ISFR_ISF6(1);

        // Falling edge?
        if((GPIO1->PDIR & (1<<6)) == 0)
        {
            sw_released = true;
        }
        else
        {
            sw_pressed = true;
        }
    }
}

void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    // Interrupt handler triggered by ENC_A?
    if((GPIO3->ISFR[0] & GPIO_ISFR_ISF31(1)) != 0)
    {
        // Clear the flag
        GPIO3->ISFR[0] = GPIO_ISFR_ISF31(1);

        // Sample ENC_B
        if((GPIO2->PDIR & (1<<7)) == 0)
        {
            pulse_cnt--;
        }
        else
        {
            pulse_cnt++;
        }
    }
}

/*!
 * \brief Resets the counted pulses.
 *
 * Resets the internal pulses counter to 0.
 */
void encoder_reset(void)
{
    pulse_cnt = 0;
}

/*!
 * \brief Returns the number of pulses in CW or CCW direction since last reset
 *
 * The function keeps track of the CW and CCW pulses. For every CW pulse, an
 * internal counter is incremented. For every CCW pulse, that same counter is
 * decremented.
 *
 * Meaning:
 *   - If the function returns 0, no pulses were counted or as much CW as CCW
 *     pulses
 *   - If the function returns a value < 0, that much more number of CCW pulse
 *     were detected.
 *   - If the function returns a value > 0, that much more number of CW pulse
 *     were detected.
 *
 * \return The number of pulses counted since last reset
 */
int32_t encoder_pulses(void)
{
    return pulse_cnt;
}

/*!
 * \brief Detects if the switch was pressed.
 *
 * This firmware driver remembers if the switch was pressed with an internal
 * flag. When this function is called, it resets the internal flag if it was
 * true.
 *
 * \return True if the switch was pressed.
 */
bool encoder_sw_pressed(void)
{
    if(sw_pressed == true)
    {
        sw_pressed = false;
        return true;
    }
    
    return false;    
}

/*!
 * \brief Detects if the switch was released.
 *
 * This firmware driver remembers if the switch was released with an internal
 * flag. When this function is called, it resets the internal flag if it was
 * true.
 *
 * \return True if the switch was released.
 */
bool encoder_sw_released(void)
{
    if(sw_released == true)
    {
        sw_released = false;
        return true;
    }
    
    return false;  
}

```

---

# main.c

```C
int main(void)
{
    encoder_init();
    gpio_output_init();

    while(1)
    {
        pulses = encoder_pulses();

        if(pulses != previous_pulses)
        {
            previous_pulses = pulses;

            if(pulses > 0)
            {
                // Green LED on
                GPIO3->PCOR = (1<<13);
                // Red LED off
                GPIO3->PSOR = (1<<12);
            }
            else if( pulses < 0)
            {
                // Green LED off
                GPIO3->PSOR = (1<<13);
                // Red LED on
                GPIO3->PCOR = (1<<12);
            }
            else
            {
                // Green LED off
                GPIO3->PSOR = (1<<13);
                // Red LED off
                GPIO3->PSOR = (1<<12);
            }
        }
        
        // --------------------------------------------------------------------
        if(encoder_sw_pressed())
        {
            encoder_reset();
        }
        
        // --------------------------------------------------------------------
        if(encoder_sw_released())
        {
            // Do nothing
        }
    }
}

```
