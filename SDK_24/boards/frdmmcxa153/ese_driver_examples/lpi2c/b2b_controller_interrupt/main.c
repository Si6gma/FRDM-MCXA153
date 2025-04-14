/*! ***************************************************************************
 *
 * \brief     Main application
 * \file      main.c
 * \author    Hugo Arends
 * \date      February 2024
 *
 * \see       NXP. (2024). MCX A153, A152, A143, A142 Reference Manual. Rev. 4,
 *            01/2024. From:
 *            https://www.nxp.com/docs/en/reference-manual/MCXAP64M96FS3RM.pdf
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
#include <MCXA153.h>
#include "gpio_output.h"
#include "lpi2c0_controller_interrupt.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile uint32_t ms = 0;
const uint32_t interval_ms = 500;
uint32_t previous_ms = 0;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    gpio_output_init();
    lpi2c0_controller_init();

    // Generate an interrupt every 1 ms
    SysTick_Config(48000);

    uint8_t rgb = 0x01;

    while(1)
    {
        uint32_t current_ms = ms;

        // --------------------------------------------------------------------
        if((current_ms - previous_ms) >= interval_ms)
        {
            previous_ms = current_ms;

            // TBD

            // Red
            if((rgb & 0x01) == 0)
            {
                GPIO3->PSOR = (1<<12);
            }
            else
            {
                GPIO3->PCOR = (1<<12);
            }

            // Green
            if((rgb & 0x02) == 0)
            {
                GPIO3->PSOR = (1<<13);
            }
            else
            {
                GPIO3->PCOR = (1<<13);
            }

            // Blue
            if((rgb & 0x04) == 0)
            {
                GPIO3->PSOR = (1<<0);
            }
            else
            {
                GPIO3->PCOR = (1<<0);
            }

            rgb = (rgb == 4) ? 1 : rgb << 1;
        }
    }
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    ms++;
}
