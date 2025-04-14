/*! ***************************************************************************
 *
 * \brief     HC-SR04
 * \file      hcsr04.h
 * \author    Hugo Arends
 * \date      December 2024
 *
 * \see       NXP. (2024). MCX A153, A152, A143, A142 Reference Manual. Rev. 4,
 *            01/2024. From:
 *            https://www.nxp.com/docs/en/reference-manual/MCXAP64M96FS3RM.pdf
 * \see       Elec Freaks. Ultrasonic Ranging Module HC - SR04.
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
#ifndef HCSR04_H
#define HCSR04_H

#include <MCXA153.h>

// Select one of the implementation solutions with the following define
// 1: TRIG: polling
//    ECHO: polling
// 2: TRIG: 10 Hz PWM signal
//    ECHO: GPIO interrupt on both rising and falling edge
// 3: TRIG: 10 Hz PWM signal
//    ECHO: Input capture on both rising and falling edge and single interrupt
//          on falling edge
#define SOLUTION (3)

void hcsr04_init(void);
int32_t hcsr04_getdistance(void);

#endif // HCSR04_H
