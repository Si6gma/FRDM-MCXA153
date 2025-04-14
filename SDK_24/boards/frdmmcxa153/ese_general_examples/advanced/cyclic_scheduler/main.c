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
#include <stdio.h>
#include "gpio_input_interrupt.h"
#include "gpio_output.h"
#include "lpuart0_interrupt.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
#ifdef DEBUG
#define TARGETSTR "Debug"
#else
#define TARGETSTR "Release"
#endif

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile uint32_t ms = 0;
static uint32_t timeout = 0xFFFFFFFF;
static uint8_t color = 0;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    gpio_input_init();
    gpio_output_init();
    lpuart0_init(115200);

    // Generate an interrupt every 1 ms
    SysTick_Config(48000);

    printf("Cyclic executive with interrupts scheduler");
    printf(" - %s\r\n", TARGETSTR);
    printf("Build %s %s\r\n", __DATE__, __TIME__);

    while(1)
    {
        // --------------------------------------------------------------------
        // Switch pressed event?
        if(gpio_input_sw3_pressed())
        {
            printf("[%08lu] event: switch pressed\r\n", ms);

            timeout = ms + 3000;

            const uint32_t lut[3] =  {(1<<13), (1<<12), (1<<0)};

            GPIO3->PCOR = lut[color];
        }

        // --------------------------------------------------------------------
        // Timeout event?
        if(ms >= timeout)
        {
            printf("[%08lu] event: timeout\r\n", ms);

            timeout = 0xFFFFFFFF;

            GPIO3->PSOR = (1<<0);
            GPIO3->PSOR = (1<<12);
            GPIO3->PSOR = (1<<13);
        }

        // --------------------------------------------------------------------
        // Incoming data event?
        if(lpuart0_rxcnt() > 0)
        {
            int data = lpuart0_getchar();

            printf("[%08lu] event: incoming data \'%c\'\r\n", ms, data);

            if(data == ' ')
            {
                printf("[%08lu] color updated\r\n", ms);
                color = (color == 2) ? 0 : color + 1;
            }
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
