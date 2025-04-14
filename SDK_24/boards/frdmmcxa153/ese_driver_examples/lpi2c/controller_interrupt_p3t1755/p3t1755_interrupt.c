/*! ***************************************************************************
 *
 * \brief     P3T1755 - I3C, I 2 C-bus interface, 0.5 �C accuracy, digital
 *            temperature sensor
 * \file      p3t1755_interrupt.c
 * \author    Hugo Arends
 * \date      February 2024
 *
 * \see       NXP. (2024). P3T1755 - I3C, I 2 C-bus interface, 0.5 �C accuracy,
 *            digital temperature sensor - Product data sheet. Rev. 1.1,
 *            04/01/2023. From:
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
#include "p3t1755_interrupt.h"
#include "lpi2c0_controller_interrupt.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
// Statemachine keeping track of update states
typedef enum
{
    STATE_IDLE = 0,
    STATE_UPDATING,
    STATE_UPDATED,
}state_t;
// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void p3t1755_init(void)
{
    lpi2c0_controller_init();

    #ifdef DEBUG

    // Check connectivity by reading the control register. The POR value is
    // 0x28 (NXP, 2023).
    //
    p3t1755_get_configuration_reg_start();

    uint8_t reg = 0;

    // Wait for result
    while(!p3t1755_get_configuration_reg_done(&reg))
    {}

    if(reg != 0x28)
    {
        // Error
        while(1)
        {}
    }

    #endif
}

void p3t1755_get_configuration_reg_start(void)
{
    // Wait as long as bus or controller is busy
    lpi2c0_wait_busy();

    // Clear all status flags
    LPI2C0->MSR = LPI2C_MSR_STF_MASK | LPI2C_MSR_DMF_MASK |
        LPI2C_MSR_PLTF_MASK | LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK |
        LPI2C_MSR_NDF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

    // Reset (hardware) fifo
    LPI2C0->MCR |= LPI2C_MCR_RTF(1) | LPI2C_MCR_RRF(1);

    // Fill transmit fifo
    // Command: 100b - Generate (repeated) Start on the I2C bus and transmit
    //         the address in DATA[7:0]
    // Data   : Slave address + w
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(0b1001000 << 1));

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : Pointer byte select configuration register
    lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0b00000001));

    // Command: 100b - Generate (repeated) Start on the I2C bus and transmit
    //          the address in DATA[7:0]
    // Data   : Slave address + r
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA((0b1001000 << 1) | 1 ));

    // Command: 001b - Receive (DATA[7:0] + 1) bytes. DATA[7:0] is used as a
    //          byte counter.
    // Data   : 0
    lpi2c0_putword(LPI2C_MTDR_CMD(0b001) | LPI2C_MTDR_DATA(0));
}

bool p3t1755_get_configuration_reg_done(uint8_t *reg)
{
    if(lpi2c0_rxcnt() < 1)
    {
        return false;
    }

    *reg = lpi2c0_getword();

    return true;
}

void p3t1755_set_configuration_reg_start(const uint8_t val)
{
    // Wait as long as bus or controller is busy
    lpi2c0_wait_busy();

    // Clear all status flags
    LPI2C0->MSR = LPI2C_MSR_STF_MASK | LPI2C_MSR_DMF_MASK |
        LPI2C_MSR_PLTF_MASK | LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK |
        LPI2C_MSR_NDF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

    // Reset (hardware) fifo
    LPI2C0->MCR |= LPI2C_MCR_RTF(1) | LPI2C_MCR_RRF(1);

    // Fill transmit fifo
    // Command: 100b - Generate (repeated) Start on the I2C bus and transmit
    //          the address in DATA[7:0]
    // Data   : Slave address + w
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(0b1001000 << 1));

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : Pointer byte select configuration register
    lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0b00000001));

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : Value
    lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(val));
}

bool p3t1755_set_configuration_reg_done(void)
{
    // Check if tx fifo is empty
    return lpi2c0_txcnt() == 0;
}

void p3t1755_get_temperature_start(void)
{
    // Wait as long as bus or controller is busy
    lpi2c0_wait_busy();

    // Clear all status flags
    LPI2C0->MSR = LPI2C_MSR_STF_MASK | LPI2C_MSR_DMF_MASK |
        LPI2C_MSR_PLTF_MASK | LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK |
        LPI2C_MSR_NDF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

    // Reset (hardware) fifo
    LPI2C0->MCR |= LPI2C_MCR_RTF(1) | LPI2C_MCR_RRF(1);

    // Fill transmit fifo
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(0b1001000 << 1));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0b00000000));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA((0b1001000 << 1) | 1 ));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b001) | LPI2C_MTDR_DATA(1));
}

bool p3t1755_get_temperature_done(float *temperature)
{
    // All bytes read?
    if(lpi2c0_rxcnt() < 2)
    {
        return false;
    }

    // Get the bytes
    uint16_t msb = lpi2c0_getword();
    uint16_t lsb = lpi2c0_getword();

    // Calculate temperature
    uint16_t temp_data = (msb << 4) | (lsb >> 4);

    float t = 0;

    // Positive temperature?
    if((temp_data & 0b0000100000000000) == 0)
    {
        t = temp_data * 0.0625f;
    }
    else
    {
        temp_data = (~temp_data) & 0x0FFF;
        t = -((temp_data + 1) * 0.0625f);
    }

    // Copy result
    *temperature = t;

    return true;
}
