/*! ***************************************************************************
 *
 * \brief     Low level driver for the encoder
 * \file      servo.c
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
 ******************************************************************************/
#include "servo.h"

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

/*!
 * \brief Initializes the servo pins
 *
 * Resources:
 * - SERVO_PWM | P3_10 | CT1_MAT0
 *
 * Configures CTimer1 to generate a PWM signal on MAT0 with a 50 Hz frequency.
 */
void servo_init(void)
{
    // TBD
}

/*!
 * \brief Set the servo duty cycle
 *
 * Sets the PWM duty cycle as follows:
 * - 1000 =   0% = 1.0 ms pulse width = servo moves left
 * - 1500 =  50% = 1.5 ms pulse width = servo moves centre
 * - 2000 = 100% = 2.0 ms pulse width = servo moves right
 *
 * \param[in]  value Duty cycle of the PWM signal
 */
void servo_set(int32_t value)
{
    // TBD
}
