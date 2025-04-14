# Solution

Solution to the assignment.

`Q1` The submodule PWM frequency is calculated as follows:

```C
submodule_pwm_frequency = (main_clk / submodule_prescaler) / (submodule_modulo + 1) Hz
```

Rewritten:

```C
(submodule_modulo + 1) = (main_clk / submodule_prescaler) / submodule_pwm_frequency
```

Substituting known values:

```C
(submodule_modulo + 1) = (48 MHz / 2) / 50 Hz = 480000
```

Problem: this value of 480000 is too high to fit in a 16-bit register!
Solution: to increase the submodule_prescaler value so the submodule_modulo value can be decreased.

Rewriting the equation above gives:

```C
submodule_prescaler = (main_clk) / ((submodule_modulo + 1) * submodule_pwm_frequency)
```

With a maximum submodule_modulo value the minimum submodule_prescaler can be calculated.
Substituting known values:

```C
submodule_prescaler = (48 MHz) / (65536 * 50) = 14.65
```

The closest next available submodule_prescaler is 16. This would give an actual submodule_modulo value of:

```C
(submodule_modulo + 1) = (48 MHz / 16) / 50 Hz = 60000
```

In other words: the submodule_modulo value of 60000 is equal to a period of 1/(50 Hz) = 20 ms.

`Q2` A duty cycle of 1 ms is therefore equal to a match value of:

```C
60000 / 20 = 3000
```

# flexpwm0_edge_aligned.c

```C
// Servo PWM configuration

// Configure submodule 1
//
// PRSC: [100] = Prescaler 16. PWM clock frequency = main_clk / 16
// FULL: [1] = Full-cycle reloads enabled
// Rest: default
FLEXPWM0->SM[1].CTRL = PWM_CTRL_PRSC(0b100) | PWM_CTRL_FULL(1);

// DBGEN: [1] = When set to one, the PWM continues to run while the chip is
//              in Debug mode. If the device enters Debug mode and this bit
//              is zero, then the PWM outputs are disabled until Debug mode
//              is exited. At that point, the PWM pins resume operation as
//              programmed in the PWM registers
// INDEP: [1] = PWM_A and PWM_B outputs are independent PWMs
// Rest: default
FLEXPWM0->SM[1].CTRL2 = PWM_CTRL2_INDEP(1) | PWM_CTRL2_DBGEN(1);

// POLX: [1] = PWM_X output inverted. A low level on the PWM_X pin
//             represents the "on" or "active" state
// Rest: default
FLEXPWM0->SM[1].OCTRL = PWM_OCTRL_POLX(1);

// DISX: [0000] = Disable fault handling
FLEXPWM0->SM[1].DISMAP[0] &= ~(PWM_DISMAP_DIS0X_MASK);

// Edge aligned PWM. See reference manual paragraph 31.3.2.2 (NXP, 2024).
FLEXPWM0->SM[1].INIT = 0;
FLEXPWM0->SM[1].VAL0 = 3000;
FLEXPWM0->SM[1].VAL1 = 60000-1;
FLEXPWM0->SM[1].VAL2 = 0;
FLEXPWM0->SM[1].VAL3 = 0;
FLEXPWM0->SM[1].VAL4 = 0;
FLEXPWM0->SM[1].VAL5 = 0;
```
