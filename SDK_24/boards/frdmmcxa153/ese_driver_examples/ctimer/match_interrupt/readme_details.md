# Standard Counter or Timer - match

Resources: *ese_driver_examples\ctimer\match_interrupt*

## Goal

Know how to use the standard counter or timer (CTIMER) module to periodically generate interrupts.

## Standard Counter or Timer

The MCXA153 microcontroller features three Standard Counter or Timer (CTIMER) modules. Features of each module include:

- 32-bit binary counter
- 32-bit prescaler
- Four 32-bit capture registers to take a snapshot of the binary counter when an input signal transitions
- Four 32-bit match registers
    - To control timer operations
    - Generate external outputs
    - Setup for PWM operation, allowing up to three single-edged controlled PWM outputs

## CTIMER clocking

Clocking the CTIMER is depicted in the following image.

![CTIMER clocking](./../../../../imgs/ctimer_clocking.png)

## CTIMER in match interrupt mode

In this example, CTIMER 1 channel 0 is configured to generate an interrupt every second. This is depicted in the following timing diagram.
MAT0 is the compare register and when the counter value matches the MAT0 register, the counter must be reset.

![CTIMER Timing diagram](./../../../../imgs/ctimer_timing_diagram.png)

Generating interrupts at a regular interval with a CTIMER can be done by following the initialization steps as described in the [reference manual paragraph 26.1.5](./../../../../datasheets/MCXAP64M96FS3RM.pdf#26.1%20Chip-specific%20CTIMER%20information){:target="_blank"}.

1. Select a clock source for the CTIMER using MRCC_CTIMER0_CLKSEL,
   MRCC_CTIMER1_CLKSEL, and MRCC_CTIMER2_CLKSEL registers.
2. Enable the clock to the CTIMER via the
   CTIMERGLOBALSTARTEN[CTIMER0_CLK_EN],
   CTIMERGLOBALSTARTEN[CTIMER1_CLK_EN], and
   CTIMERGLOBALSTARTEN[CTIMER2_CLK_EN] fields. This enables the register
   interface and the peripheral function clock.
3. Clear the CTIMER peripheral reset using the MRCC_GLB_RST0 registers.
4. Each CTIMER provides interrupts to the NVIC. See MCR and CCR registers
   in the CTIMER register section for match and capture events. For
   interrupt connections, see the attached NVIC spreadsheet.
5. Select timer pins and pin modes as needed through the relevant PORT
   registers.
6. The CTIMER DMA request lines are connected to the DMA trigger inputs
   via the DMAC0_ITRIG_INMUX registers (See Memory map and register
   definition). Note that timer DMA request outputs are connected to DMA
   trigger inputs.

As an example, CTIMER1 will be configured to generate interrupts with a frequency of 1 Hz. This doesn't require the use of pins or DMA requests, so steps 5. and 6. can be omitted. The other steps are explained in more detail in the following sections.

### 1. Select a clock source

The source clock is selected in the MRCC module. From the available options, 1 MHz is selected. Furthermore, the source clock is configured to run and the divider is set to 1. This divider can be seen as an additional prescaler (available in the MRCC module).

```C
// MUX: [101] = CLK_1M
MRCC0->MRCC_CTIMER1_CLKSEL = MRCC_MRCC_CTIMER1_CLKSEL_MUX(0b101);

// HALT: [0] = Divider clock is running
// RESET: [0] = Divider isn't reset
// DIV: [0000] = divider value = (DIV+1) = 1
MRCC0->MRCC_CTIMER1_CLKDIV = 0;
```

### 2. Enable the clock to the CTIMER

The CTIMER module interface and function clock are disabled by default. These are enabled in the SYSCON module as follows:

```C
// CTIMER1_CLK_EN: [1] = CTIMER 1 function clock enabled
SYSCON->CTIMERGLOBALSTARTEN |= SYSCON_CTIMERGLOBALSTARTEN_CTIMER1_CLK_EN(1);
```

### 3. Clear the CTIMER peripheral reset

Similar to all other modules, the CTIMER needs to be enabled and released from reset in the MRCC module:

```C
// Enable modules and leave others unchanged
// CTIMER1: [1] = Peripheral clock is enabled
MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_CTIMER1(1);

// Release modules from reset and leave others unchanged
// CTIMER1: [1] = Peripheral is released from reset
MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_CTIMER1(1);
```

### 4. Setup NVIC

In order to generate interrupts at the desired interval of 1 Hz, the prescaler and top value must be configured.

Using the equation from [timers introduction](./../../../../docs/timers_introduction.md){:target="_blank"}:

```txt
f_interrupts = f_source_clock / prescaler / (top + 1)
      1Hz    =      1 MHz     / prescaler / (top + 1)
```

This yields a single equation with two unknowns. However, a value can be chosen for one of the unknowns:

```txt
      1Hz    =       1MHz     /   1000    / (top + 1)
      1Hz    =             1 kHz          / (top + 1)
 =>
    (top + 1)= 1 kHz / 1 Hz
    (top + 1)=     1000
```

Finally, check if both the chosen and calculated value fit within 32-bit registers. This is true, because in both cases 1000 < 2^32.

Each timer has four match channels. For generating a single interrupt, the selected channel doesn't matter, so let's select match register channel 0 (MR0).
Three bits need to be configured for MR0:

- MR0S - Stop on MR0
  *Stops Timer Counter (TC) and Prescale Counter (PC), and turns TCR[CEN] to 0 if MR0 matches Timer Counter (TC).* Must be 0, because the timer must keep on running to generate interrupt periodically
- MR0R - Reset on MR0
  *Resets Timer Counter (TC) if MR0 matches its value.* Must be 1, because the timer must restart from zero on a match.
- MR0I - Interrupt on MR0
  *Generates an interrupt when MR0 matches the value in Timer Counter (TC).* Must be 1, because an interrupt must be generated.

```C
// Specifies the prescale value. 1 MHz / 1000 = 1 kHz
CTIMER1->PR = 1000-1;

// Match value for match register 0. 1 kHz / 1000 = 1 Hz
CTIMER1->MR[0] = 1000-1;

// MR0S: [0] = Does not stop Timer Counter (TC) if MR0 matches Timer Counter
//             (TC)
// MR0R: [1] = Resets Timer Counter (TC) if MR0 matches its value.
// MR0I: [1] = Generates an interrupt when MR0 matches the value in Timer
//             Counter (TC).
CTIMER1->MCR |= CTIMER_MCR_MR0R(1) | CTIMER_MCR_MR0I(1);
```

Finally, interrupts are enabled in the NVIC and the CTIMER is enabled.

```C
// Enable Interrupts
NVIC_SetPriority(CTIMER1_IRQn, 7);
NVIC_ClearPendingIRQ(CTIMER1_IRQn);
NVIC_EnableIRQ(CTIMER1_IRQn);

// CEN: [1] = Enables the counters.
CTIMER1->TCR |= CTIMER_TCR_CEN(1);
```

# Interrupt handler

The following interrupt handler clears the flag in the NVIC and CTIMER1 and sets a global flag that can be polled in the main application.

```C
void CTIMER1_IRQHandler(void)
{
    // Clear pending IRQ
    NVIC_ClearPendingIRQ(CTIMER1_IRQn);

    // Interrupt generated by MR0?
    if((CTIMER1->IR & CTIMER_IR_MR0INT(1)) != 0)
    {
        // Clear status flag by writing 1
        CTIMER1->IR |= CTIMER_IR_MR0INT(1);

        // Handle the event
        ctimer1_0_timeout_flag = true;
    }
}
```

In main, this flag is polled and used to toggle the green LED as follows:

![CTIMER Green LED](./../../../../imgs/ctimer_green_led.png)

## Assignment

Use CTIMER1 match register 1 (MR1) to additionally generate the following signal for the red LED:

![CTIMER Green and Red LED](./../../../../imgs/ctimer_green_and_red_led.png)

Tips:

- Both signals have the same frequency, so they can share a CTIMER.
- Only one of both channels should be configured to reset the timer when a match occurs.
- Add another shared variable to let main know an interrupt occurred:

```C
extern volatile bool ctimer1_1_timeout_flag;
```

[Solution](./solution.md){:target="_blank"}
