/**
  ******************************************************************************
  * File    delay.s
  * Authors H. Arends
  * Version V1.0
  * Date    December 2024
  * Brief   ARM Cortex-M33 assembler routine to create a milliseond delay
  *
  *         ARM Cortex-M33 instruction set can be found here:
  *         https://developer.arm.com/documentation/100235/0100
  *
  *         Example usage:
  *
  *               delay_ms(100); // Delay 100 milliseconds
  *
  * Copyright (C) 2024 HAN University of Applied Sciences. All Rights Reserved.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a
  * copy of this software and associated documentation files (the "Software"),
  * to deal in the Software without restriction, including without limitation
  * the rights to use, copy, modify, merge, publish, distribute, sublicense,
  * and/or sell copies of the Software, and to permit persons to whom the
  * Software is furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  * IN THE SOFTWARE.
  ******************************************************************************
  */
        .syntax unified
        .cpu cortex-m33
        .thumb

        .section .text.delay_ms
        .weak delay_ms
        .type delay_ms, %function

delay_ms:

        MOV   r1, #5380 // Implements a delay of approximately 1ms
                        // This value is determined by experimentation

loop:
        SUBS  r1, #1
        BNE   loop

        SUBS  r0, #1
        BNE   delay_ms

        BX    lr
