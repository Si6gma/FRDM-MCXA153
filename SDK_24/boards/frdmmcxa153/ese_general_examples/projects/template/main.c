/*! ***************************************************************************
 *
 * \brief     Main application
 * \file      main.c
 * \author    Hugo Arends
 * \date      September 2024
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
static volatile uint32_t previous_ms = 0;
static const uint32_t interval_ms = 1000;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    gpio_output_init();
    gpio_input_init();
    lpuart0_init(115200);

    // Generate an interrupt every 1 ms
    SysTick_Config(48000);

    printf("Template project");
    printf(" - %s\r\n", TARGETSTR);
    printf("Build %s %s\r\n", __DATE__, __TIME__);

    printf("\r\n");
    printf("Use this project as a template for your own project.\r\n" \
        "Read the readme.md for instructions how to rename the project\r\n" \
        "and/or add source files.\r\n"
        "\r\n");

    while(1)
    {
        uint32_t current_ms = ms;

        // --------------------------------------------------------------------
        if((current_ms - previous_ms) >= interval_ms)
        {
            previous_ms = current_ms;

            printf("[%08lu] Timeout\r\n", ms);
        }

        // Switch pressed?
        if(gpio_input_sw3_pressed())
        {
            printf("[%08lu] SW3 pressed\r\n", ms);

            // Green LED off
            GPIO3->PSOR = (1<<13);
        }

        // Switch pressed?
        if(gpio_input_sw2_pressed())
        {
            printf("[%08lu] SW2 pressed\r\n", ms);

            // Green LED on
            GPIO3->PCOR = (1<<13);
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
