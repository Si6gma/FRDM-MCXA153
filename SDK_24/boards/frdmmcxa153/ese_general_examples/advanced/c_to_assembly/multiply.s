/*! ***************************************************************************
 *
 * \brief     Example for creating a function in assembly
 * \file      multiply.s
 * \author    Hugo Arends
 * \date      June 2024
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

    .syntax unified
    .arch armv8-m.main

    .thumb_func
    .align 2
    .global  multiply
    .type    multiply, %function

multiply:              // 'multiply' is called a label. It is translated into a
                       // memory address by the linker. This memory address,
                       // holds the first instruction of this function (entry
                       // point).
                       //
                       // By convention:
                       // - R0 holds the first argument a and R1 holds the second
                       //   argument b.
                       // - When the function finishes, R0 must contain the
                       //   return value of the function.

      PUSH  {R7}       // R7 is used in this function, but it's value should be
                       // restored after this function is done. Therefore, save
                       // it on the stack.

      MUL   R7,R0,R1   // Multiply R0 and R1 and store the result in R7.
      MOV   R0,R7      // R0 should contain the multiplied result, so move it.

      POP   {R7}       // Restore the value of R7

      BX    LR         // Return from function
