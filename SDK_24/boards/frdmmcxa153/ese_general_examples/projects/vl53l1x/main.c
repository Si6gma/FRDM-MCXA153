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
 *            Hardware connections:
 *
 *                  FRDM-MCXA153 | VL53L1X
 *              -----------------+--------
 *                           3V3 - VIN
 *                           GND - GND
 *              P3_27/LPI2C0_SCL - SCL
 *              P3_28/LPI2C0_SDA - SDA
 *                    P3_15/GPIO - GPIOI
 *                            nc - XSHUT
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
#include "lpi2c0_controller_polling.h"
#include "lpuart0_interrupt.h"

#include "VL53L1X_api.h"
#include "VL53l1X_calibration.h"

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
volatile uint32_t ms = 0;
static volatile uint32_t previous_ms = 0;
static const uint32_t interval_ms = 1000;

static const uint16_t dev = 0x52;

static int8_t status;
static uint16_t distance;
static uint16_t signal_rate;
static uint16_t ambient_rate;
static uint16_t spad_num;
static uint8_t range_status;
//static uint8_t data_ready;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    gpio_output_init();
    gpio_input_init();
    lpuart0_init(115200);
    lpi2c0_controller_init();

    // Generate an interrupt every 1 ms
    SysTick_Config(48000);

    printf("VL53L1X ToF project");
    printf(" - %s\r\n", TARGETSTR);
    printf("Build %s %s\r\n", __DATE__, __TIME__);

    // Print module specific info
    uint8_t byte_data;
    uint16_t word_data;

    status = VL53L1_RdByte(dev, 0x010F, &byte_data);
    printf("VL53L1X Model ID\t0x%02X\r\n", byte_data);
    status = VL53L1_RdByte(dev, 0x0110, &byte_data);
    printf("VL53L1X Module type\t0x%02X\r\n", byte_data);
    status = VL53L1_RdWord(dev, 0x010F, &word_data);
    printf("VL53L1X 0x%04X\r\n", word_data);

    uint8_t sensor_state = 0;

    while(sensor_state == 0)
    {
        status = VL53L1X_BootState(dev, &sensor_state);

        // Wait 2ms between polling the boot state
        uint32_t timeout = ms + 2;
        while(ms < timeout)
        {}
    }
    printf("VL53L1X booted\r\n");

    // This function must to be called to initialize the sensor with the default
    // setting
    status = VL53L1X_SensorInit(dev);

    // Optional functions to be used to change the main ranging parameters
    // according the application requirements to get the best ranging
    // performances

    // 1=short, 2=long
    status = VL53L1X_SetDistanceMode(dev, 2);

    // In ms, possible values are 20, 50, 100, 200, and 500
    status = VL53L1X_SetTimingBudgetInMs(dev, 20);

    // In ms, make sure IM >= TB
    status = VL53L1X_SetInterMeasurementInMs(dev, 20);

//    // Offset compensation in mm
//    status = VL53L1X_SetOffset(dev, 20);

//    // Minimum ROI 4,4
//    status = VL53L1X_SetROI(dev, 16, 16);

    // ------------------------------------------------------------------------
    // Refer to Section 4.1.1 Offset calibration procedure
    // UM2510 - A guide to using the VL53L1X ultra lite driver Rev 4 - April 2020

    int16_t offset = 58;

#if 0
    printf("Running VL53L1X_CalibrateOffset()\r\n");

    // May take few second to perform the offset cal
    status = VL53L1X_CalibrateOffset(dev, 140, &offset);

    printf("Update code as follows: int16_t offset = %d;\r\n", offset);

#else
    status = VL53L1X_SetOffset(dev, offset);
#endif

    // ------------------------------------------------------------------------
    // Refer to Section 4.2.2 Crosstalk calibration procedure
    // UM2510 - A guide to using the VL53L1X ultra lite driver Rev 4 - April 2020

    uint16_t xtalk = 0;

#if 0
    printf("Running VL53L1X_CalibrateXtalk()\r\n");

    // May take few second to perform the xtalk cal
    status = VL53L1X_CalibrateXtalk(dev, 1000, &xtalk);

    printf("Update code as follows: uint16_t xtalk = %d;\r\n", xtalk);

#else
    status = VL53L1X_SetXtalk(dev, xtalk);
#endif

    VL53L1X_Version_t vl53l1x_version;
    status = VL53L1X_GetSWVersion(&vl53l1x_version);

    // ULD = Ultra Lite Driver
    printf("VL53L1X ULD version %u.%u.%u\r\n", vl53l1x_version.major, vl53l1x_version.minor,
        vl53l1x_version.build);

    printf("VL53L1X ULD running\r\n");

//    // Set distance threshold between 500mm and 1000mm and generate an
//    // interrupt only when distance measurement is below 100mm
//    VL53L1X_SetDistanceThreshold(dev, 500, 1000, 0, 1);

    // This function has to be called to enable the ranging
    status = VL53L1X_StartRanging(dev);

    while(1)
    {
        uint32_t current_ms = ms;

        // ---------------------------------------------------------------------
        if((current_ms - previous_ms) >= interval_ms)
        {
            previous_ms = current_ms;

//            printf("[%08lu] ", ms);

              // Polling example
//            status = VL53L1X_CheckForDataReady(dev, &data_ready);
//
//            if(data_ready == 1)
//            {
//                data_ready = 0;
//
//                // Get the sensor data
//                // Clear interrupt has to be called to enable next interrupt
//                // Print sensor data
//            }

//            printf("\r\n");
        }

        // ---------------------------------------------------------------------
        // Interrupt example
        if(gpio_vl53l1x_interrupt())
        {
            // Get the sensor data
            status = VL53L1X_GetRangeStatus(dev, &range_status);
            status = VL53L1X_GetDistance(dev, &distance);
            status = VL53L1X_GetSignalRate(dev, &signal_rate);
            status = VL53L1X_GetAmbientRate(dev, &ambient_rate);
            status = VL53L1X_GetSpadNb(dev, &spad_num);

            // Clear interrupt has to be called to enable next interrupt
            status = VL53L1X_ClearInterrupt(dev);

            // Print sensor data
            printf("[%08lu] ", ms);
            printf("%u, %u, %u, %u, %u", range_status, distance, signal_rate,
                ambient_rate, spad_num);
            printf("\r\n");

            if((distance < 500) && (range_status == 0))
            {
                // Green LED on
                GPIO3->PCOR = (1<<13);
            }
            else
            {
                // Green LED off
                GPIO3->PSOR = (1<<13);
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
