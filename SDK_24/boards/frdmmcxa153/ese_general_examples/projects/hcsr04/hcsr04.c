/*! ***************************************************************************
 *
 * \brief     HC-SR04
 * \file      hcsr04.c
 * \author    Hugo Arends
 * \date      December 2024
 *
 * \see       NXP. (2024). MCX A153, A152, A143, A142 Reference Manual. Rev. 4,
 *            01/2024. From:
 *            https://www.nxp.com/docs/en/reference-manual/MCXAP64M96FS3RM.pdf
 * \see       Elec Freaks. Ultrasonic Ranging Module HC - SR04.
 *            From:
 *            https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
 *
 * \copyright 2024 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include "hcsr04.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------

#if (SOLUTION == 1)
void hcsr04_init(void)
{
    // -------------------------------------------------------------------------
    // PORT3 configuration
    // -------------------------------------------------------------------------
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

    // Release modules from reset and leave others unchanged
    // GPIO3: [1] = Peripheral is released from reset
    // PORT3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_PORT3(1);

    // -------------------------------------------------------------------------
    // TRIG pin configuration - P3_11
    // -------------------------------------------------------------------------

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
    // Configure P3_11: TRIG pin
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
    PORT3->PCR[11] = PORT_PCR_LK(1);

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
    // PDO11: [0] = Logic level 0 – TRIG pin low
    GPIO3->PDOR &= ~(1<<11);

    // 1. b.
    //
    // PDO11: [1] = Output
    GPIO3->PDDR |= (1<<11);

    // -------------------------------------------------------------------------
    // ECHO pin configuration - P3_1
    // -------------------------------------------------------------------------

    // 1. & 2.
    //
    // Configure P3_1: ECHO pin
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
    PORT3->PCR[1] = PORT_PCR_LK(1) | PORT_PCR_IBE(1);

    // -------------------------------------------------------------------------
    // CTIMER1 configuration
    // -------------------------------------------------------------------------

    // From section 26.1.5 Initialization (NXP, 2024)
    //
    // 1. Select a clock source for the CTIMER using MRCC_CTIMER0_CLKSEL,
    //    MRCC_CTIMER1_CLKSEL, and MRCC_CTIMER2_CLKSEL registers.
    // 2. Enable the clock to the CTIMER via the
    //    CTIMERGLOBALSTARTEN[CTIMER0_CLK_EN],
    //    CTIMERGLOBALSTARTEN[CTIMER1_CLK_EN], and
    //    CTIMERGLOBALSTARTEN[CTIMER2_CLK_EN] fields. This enables the register
    //    interface and the peripheral function clock.
    // 3. Clear the CTIMER peripheral reset using the MRCC_GLB_RST0 registers.
    // 4. Each CTIMER provides interrupts to the NVIC. See MCR and CCR registers
    //    in the CTIMER register section for match and capture events. For
    //    interrupt connections, see the attached NVIC spreadsheet.
    // 5. Select timer pins and pin modes as needed through the relevant PORT
    //    registers.
    // 6. The CTIMER DMA request lines are connected to the DMA trigger inputs
    //    via the DMAC0_ITRIG_INMUX registers (See Memory map and register
    //    definition). Note that timer DMA request outputs are connected to DMA
    //    trigger inputs.

    // 1.
    //
    // MUX: [101] = CLK_1M
    MRCC0->MRCC_CTIMER1_CLKSEL = MRCC_MRCC_CTIMER1_CLKSEL_MUX(0b101);

    // HALT: [0] = Divider clock is running
    // RESET: [0] = Divider isn't reset
    // DIV: [0000] = divider value = (DIV+1) = 1
    MRCC0->MRCC_CTIMER1_CLKDIV = 0;

    // 2.
    //
    // CTIMER1_CLK_EN: [1] = CTIMER 1 function clock enabled
    SYSCON->CTIMERGLOBALSTARTEN |= SYSCON_CTIMERGLOBALSTARTEN_CTIMER1_CLK_EN(1);

    // 3.
    //
    // Enable modules and leave others unchanged
    // CTIMER1: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_CTIMER1(1);

    // Release modules from reset and leave others unchanged
    // CTIMER1: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_CTIMER1(1);

    // 4.
    //
    // Specifies the prescale value. 1 MHz / 1 = 1 MHz
    CTIMER1->PR = 0;
}

int32_t hcsr04_getdistance(void)
{
    // Reset and start TC
    CTIMER1->TC = 0;
    CTIMER1->TCR |= CTIMER_TCR_CEN(1);

    // TRIG set
    GPIO3->PSOR = (1<<11);

    // Wait 10 microseconds
    while(CTIMER1->TC < 10)
    {}

    // TRIG clear
    GPIO3->PCOR = (1<<11);

    // Note. Measuring P3_11 with a logic analyzer shows a
    //       17.813 microseconds TRIG pulse.

    // Stop TC
    CTIMER1->TCR &= ~CTIMER_TCR_CEN(1);

    // Wait while ECHO low
    while((GPIO3->PDIR & (1<<1)) == 0)
    {}

    // Reset and start TC
    CTIMER1->TC = 0;
    CTIMER1->TCR |= CTIMER_TCR_CEN(1);

    // Wait while ECHO high
    while((GPIO3->PDIR & (1<<1)) != 0)
    {}

    // Stop timer
    CTIMER1->TCR &= ~CTIMER_TCR_CEN(1);

    // Get result and calculate the distance

    int32_t distance = CTIMER1->TC / 58;

    return distance;
}

#endif

#if (SOLUTION == 2)
void hcsr04_init(void)
{
    // -------------------------------------------------------------------------
    // PORT3 configuration
    // -------------------------------------------------------------------------
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

    // Release modules from reset and leave others unchanged
    // GPIO3: [1] = Peripheral is released from reset
    // PORT3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_PORT3(1);

    // -------------------------------------------------------------------------
    // TRIG pin configuration - P3_11
    // -------------------------------------------------------------------------

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
    // Configure P3_11: TRIG pin
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [0] = Disables
    // MUX: [0100] = Alternative 4 (CT1_MAT1)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disables
    // PS:  [0] = n.a.
    PORT3->PCR[11] = PORT_PCR_LK(1) | PORT_PCR_MUX(4);

    // -------------------------------------------------------------------------
    // ECHO pin configuration - P3_1
    // -------------------------------------------------------------------------

    // 1. & 2.
    //
    // Configure P3_1: ECHO pin
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
    PORT3->PCR[1] = PORT_PCR_LK(1) | PORT_PCR_IBE(1);

    // ISF: [1] = Clear the flag
    // IRQC : [1011] = ISF and interrupt on either edge
    GPIO3->ICR[1] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1011);

    // Enable GPIO3 interrupts
    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);

    // -------------------------------------------------------------------------
    // CTIMER1 configuration
    // -------------------------------------------------------------------------

    // 1.
    //
    // MUX: [101] = CLK_1M
    MRCC0->MRCC_CTIMER1_CLKSEL = MRCC_MRCC_CTIMER1_CLKSEL_MUX(0b101);

    // HALT: [0] = Divider clock is running
    // RESET: [0] = Divider isn't reset
    // DIV: [0000] = divider value = (DIV+1) = 1
    MRCC0->MRCC_CTIMER1_CLKDIV = 0;

    // 2.
    //
    // CTIMER1_CLK_EN: [1] = CTIMER 1 function clock enabled
    SYSCON->CTIMERGLOBALSTARTEN |= SYSCON_CTIMERGLOBALSTARTEN_CTIMER1_CLK_EN(1);

    // 3.
    //
    // Enable modules and leave others unchanged
    // CTIMER1: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_CTIMER1(1);

    // Release modules from reset and leave others unchanged
    // CTIMER1: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_CTIMER1(1);

    // 4.
    //
    // Specifies the prescale value. 1 MHz / 1 = 1 MHz
    CTIMER1->PR = 0;

    // Match value: 1 MHz / 100000 = 10 Hz
    //
    // In PWM mode, use match channel 3 to set the PWM cycle length. The other
    // channels can be used for matches
    CTIMER1->MR[1] = 100000-1-10;
    CTIMER1->MR[3] = 100000-1;

    // MR1S: [0] = Does not stop Timer Counter (TC) if MR0 matches Timer Counter
    //             (TC)
    // MR1R: [0] = Resets Timer Counter (TC) if MR0 matches its value.
    // MR1I: [0] = No interrupt when MR0 matches the value in Timer
    //             Counter (TC).
    // MR3S: [0] = Does not stop Timer Counter (TC) if MR3 matches Timer Counter
    //             (TC)
    // MR3R: [1] = Resets Timer Counter (TC) if MR3 matches its value.
    // MR3I: [0] = No interrupt when MR3 matches the value in Timer
    //             Counter (TC).
    CTIMER1->MCR |= CTIMER_MCR_MR3R(1);

    // Configure match outputs as PWM outputs.
    CTIMER1->PWMC |= CTIMER_PWMC_PWMEN3(1) | CTIMER_PWMC_PWMEN1(1);

    // CEN: [1] = Enables the counters.
    CTIMER1->TCR |= CTIMER_TCR_CEN(1);
}

static int32_t distance = 0;
static uint32_t start_us = 0;


int32_t hcsr04_getdistance(void)
{
    return distance;
}

void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    // Interrupt handler triggered by P1_7?
    if((GPIO3->ISFR[0] & GPIO_ISFR_ISF1(1)) != 0)
    {
        // Clear the flag
        GPIO3->ISFR[0] = GPIO_ISFR_ISF1(1);

        // Handle the event

        // Falling edge
        if((GPIO3->PDIR & (1<<1)) == 0)
        {
            distance = (CTIMER1->TC - start_us) / 58;
        }
        else
        {
            start_us = CTIMER1->TC;
        }
    }
}

#endif

#if (SOLUTION == 3)
void hcsr04_init(void)
{
    // -------------------------------------------------------------------------
    // PORT3 configuration
    // -------------------------------------------------------------------------
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

    // Release modules from reset and leave others unchanged
    // GPIO3: [1] = Peripheral is released from reset
    // PORT3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_PORT3(1);

    // -------------------------------------------------------------------------
    // TRIG pin configuration - P3_11
    // -------------------------------------------------------------------------

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
    // Configure P3_11: TRIG pin
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [0] = Disables
    // MUX: [0100] = Alternative 4 (CT1_MAT1)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disables
    // PS:  [0] = n.a.
    PORT3->PCR[11] = PORT_PCR_LK(1) | PORT_PCR_MUX(4);

    // -------------------------------------------------------------------------
    // ECHO pin configuration - P3_1
    // -------------------------------------------------------------------------

    // 1. & 2.
    //
    // Configure P3_1: ECHO pin
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0100] = Alternative 4 (CT_INP17)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disables
    // PS:  [0] = n.a.
    PORT3->PCR[1] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(4);

    // -------------------------------------------------------------------------
    // INPUTMUX0 configuration
    // -------------------------------------------------------------------------

    // Enable INPUTMUX0
    MRCC0->MRCC_GLB_CC0 |= MRCC_MRCC_GLB_CC0_INPUTMUX0(1);
    MRCC0->MRCC_GLB_RST0 |= MRCC_MRCC_GLB_RST0_INPUTMUX0(1);

    // CT_INP17 input (ECHO pin) is selected for both capture channel 0 and 1
    INPUTMUX0->CTIMER1CAP[0] = 0b0010010;
    INPUTMUX0->CTIMER1CAP[1] = 0b0010010;

    // -------------------------------------------------------------------------
    // CTIMER1 configuration
    // -------------------------------------------------------------------------

    // 1.
    //
    // MUX: [101] = CLK_1M
    MRCC0->MRCC_CTIMER1_CLKSEL = MRCC_MRCC_CTIMER1_CLKSEL_MUX(0b101);

    // HALT: [0] = Divider clock is running
    // RESET: [0] = Divider isn't reset
    // DIV: [0000] = divider value = (DIV+1) = 1
    MRCC0->MRCC_CTIMER1_CLKDIV = 0;

    // 2.
    //
    // CTIMER1_CLK_EN: [1] = CTIMER 1 function clock enabled
    SYSCON->CTIMERGLOBALSTARTEN |= SYSCON_CTIMERGLOBALSTARTEN_CTIMER1_CLK_EN(1);

    // 3.
    //
    // Enable modules and leave others unchanged
    // CTIMER1: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_CTIMER1(1);

    // Release modules from reset and leave others unchanged
    // CTIMER1: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_CTIMER1(1);

    // 4.
    //
    // Specifies the prescale value. 1 MHz / 1 = 1 MHz
    CTIMER1->PR = 0;

    // Match value: 1 MHz / 100000 = 10 Hz
    //
    // In PWM mode, use match channel 3 to set the PWM cycle length. The other
    // channels can be used for matches
    CTIMER1->MR[1] = 100000-1-10;
    CTIMER1->MR[3] = 100000-1;

    // MR1S: [0] = Does not stop Timer Counter (TC) if MR1 matches Timer Counter
    //             (TC)
    // MR1R: [0] = Resets Timer Counter (TC) if MR1 matches its value.
    // MR1I: [0] = No interrupt when MR1 matches the value in Timer
    //             Counter (TC).
    // MR3S: [0] = Does not stop Timer Counter (TC) if MR3 matches Timer Counter
    //             (TC)
    // MR3R: [1] = Resets Timer Counter (TC) if MR3 matches its value.
    // MR3I: [0] = No interrupt when MR3 matches the value in Timer
    //             Counter (TC).
    CTIMER1->MCR |= CTIMER_MCR_MR3R(1);

    // Configure match outputs as PWM outputs.
    CTIMER1->PWMC |= CTIMER_PWMC_PWMEN3(1) | CTIMER_PWMC_PWMEN1(1);

    // CAP0: captures the CTIMER->VAL on a rising edge
    // CAP1: captures the CTIMER->VAL on a falling edge and generates an
    //       interrupt
    //
    // CAP0RE: [1] = Rising Edge of Capture Channel 0 loads CAP0
    // CAP0FE: [0] = Falling Edge of Capture Channel 0 does not load CAP0
    // CAP0I : [0] = Does not generate interrupt
    // CAP1RE: [0] = Rising Edge of Capture Channel 1 does not load CAP1
    // CAP1FE: [1] = Falling Edge of Capture Channel 1 loads CAP1
    // CAP1I : [1] = Does generate interrupt
    CTIMER1->CCR |= CTIMER_CCR_CAP1I(1) | CTIMER_CCR_CAP1FE(1) |
        CTIMER_CCR_CAP0RE(1);

    // Enable Interrupts
    NVIC_SetPriority(CTIMER1_IRQn, 7);
    NVIC_ClearPendingIRQ(CTIMER1_IRQn);
    NVIC_EnableIRQ(CTIMER1_IRQn);

    // CEN: [1] = Enables the counters.
    CTIMER1->TCR |= CTIMER_TCR_CEN(1);
}

static int32_t distance = 0;

int32_t hcsr04_getdistance(void)
{
    return distance;
}

void CTIMER1_IRQHandler(void)
{
    // Clear pending IRQ
    NVIC_ClearPendingIRQ(CTIMER1_IRQn);

    // Interrupt generated by CR1?
    if((CTIMER1->IR & CTIMER_IR_CR1INT(1)) != 0)
    {
        // Clear status flag by writing 1
        CTIMER1->IR |= CTIMER_IR_CR1INT(1);

        // Handle the event
        distance = (CTIMER1->CR[1] - CTIMER1->CR[0]) / 58;
    }
}

#endif
