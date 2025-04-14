/*! ***************************************************************************
 *
 * \brief     Low level driver for the encoder
 * \file      encoder.c
 * \author    Hugo Arends
 * \date      April 2024
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
#include "encoder.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile int32_t pulse_cnt = 0;
static volatile bool sw_pressed = false;
static volatile bool sw_released = false;

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------

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
    // TBD
}

void GPIO1_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO1_IRQn);

    // TBD
}

void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    // TBD
}

/*!
 * \brief Resets the counted pulses.
 *
 * Resets the internal pulses counter to 0.
 */
void encoder_reset(void)
{
    // TBD
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
    // TBD

    return 0;
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
    // TBD

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
    // TBD

    return false;
}
