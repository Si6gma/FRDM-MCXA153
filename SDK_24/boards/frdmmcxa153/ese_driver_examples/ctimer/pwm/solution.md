# Solution

Solution to the assignment.

`Q1` The blue LED is connected to P3_0. Checking the muxing slot alternatives, there is **no** CTIMER match alternative.
The green LED is connected to P3_13. Checking the muxing slot alternatives, there is one CTIMER match alternative, namely CT1_MAT3. However, in PWM mode, MAT3 is dedicated to the top value.

`Q2` The red LED stays off, because there is never a match. The line will always remain high. From the reference manual:

> *All single-edge controlled PWM outputs go low at the beginning of each PWM cycle (timer is set to zero) unless their match value
is zero.*

`Q3` The red LED is on with a 50% duty cycle.

`Q4` The red LED is on with a duty cycle of 99%. As stated in the reference manual:

> *If a match register contains the same value as the timer reset value (the PWM cycle length), the PWM output is reset to low on the next clock tick after the timer reaches the match value. Therefore, the PWM output always consists of one clock tick wide positive pulse with a period that the PWM cycle length determines (that is, the timer reload value).*

`Q5` The match value is greater than the PWM cycle length, so the output will continue to stay low.

`Q6` The effective prescaler is 1, so the generated PWM frequency will be 10 times faster: 10 kHz instead of 1 kHz.

`Q7` MAT3 must be equal to 1000-1.

`Q8` The duty cycle of the generated PWM signal can be configured more precise. Instead of 100 steps, there are now 1000 steps.
