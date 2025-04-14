
/*
* This file is part of VL53L1 Platform
*
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "vl53l1_platform.h"
#include <string.h>
#include <time.h>
#include <math.h>

#include "lpi2c0_controller_polling.h"

extern volatile uint32_t ms;

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    // Wait as long as bus or controller is busy
    lpi2c0_wait_busy();

    // Clear all status flags
    LPI2C0->MSR = LPI2C_MSR_STF_MASK | LPI2C_MSR_DMF_MASK |
        LPI2C_MSR_PLTF_MASK | LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK |
        LPI2C_MSR_NDF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

    // Command: 100b - Generate (repeated) Start on the I2C bus and transmit
    //          the address in DATA[7:0]
    // Data   : Slave address + w
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(dev);

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : index high byte
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA((uint8_t)(index>>8));

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : index low byte
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA((uint8_t)(index));

    for(uint32_t i=0; i<count; ++i)
    {
        // Command: 000b - Transmit the value in DATA[7:0]
        // Data   : data
        LPI2C0->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(pdata[i]);

        // Wait while transmit fifo full
        while(lpi2c0_txfifo_full())
        {}
    }

    // Command: 010b - Generate Stop condition on I2C bus
    // Data   : n.a.
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b010);

    return 0;
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    // Wait as long as bus or controller is busy
    lpi2c0_wait_busy();

    // Clear all status flags
    LPI2C0->MSR = LPI2C_MSR_STF_MASK | LPI2C_MSR_DMF_MASK |
        LPI2C_MSR_PLTF_MASK | LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK |
        LPI2C_MSR_NDF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

    // Command: 100b - Generate (repeated) Start on the I2C bus and transmit
    //          the address in DATA[7:0]
    // Data   : Slave address + w
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(dev);

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : index high byte
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA((uint8_t)(index>>8));

    // Command: 000b - Transmit the value in DATA[7:0]
    // Data   : index low byte
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA((uint8_t)(index));

    // Command: 100b - Generate (repeated) Start on the I2C bus and transmit
    //          the address in DATA[7:0]
    // Data   : Slave address + r
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(dev | 1 );

    // Wait while transmit fifo full
    while(lpi2c0_txfifo_full())
    {}

    // Command: 001b - Receive (DATA[7:0] + 1) bytes. DATA[7:0] is used as a
    //          byte counter.
    // Data   : count
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b001) | LPI2C_MTDR_DATA(count - 1);

    // Receive the data
    for(uint32_t i=0; i<count; ++i)
    {
        // Wait while the RXFIFO is empty
        while(lpi2c0_rxfifo_empty())
        {}

        // Read the data
        pdata[i] = (uint8_t)LPI2C0->MRDR;
    }

    // Command: 010b - Generate Stop condition on I2C bus
    // Data   : n.a.
    LPI2C0->MTDR = LPI2C_MTDR_CMD(0b010);

    return 0;
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data)
{
    return VL53L1_WriteMulti(dev, index, &data, 1);
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data)
{
    uint8_t tmp[2];

    tmp[0] = (data >> 8) & 0x00FF;
    tmp[1] = (data >> 0) & 0x00FF;

    return VL53L1_WriteMulti(dev, index, tmp, 2);
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data)
{
    uint8_t tmp[4];

    tmp[0] = (data >> 24) & 0x00FF;
    tmp[1] = (data >> 16) & 0x00FF;
    tmp[2] = (data >>  8) & 0x00FF;
    tmp[3] = (data >>  0) & 0x00FF;

    return VL53L1_WriteMulti(dev, index, tmp, 4);
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data)
{
    return VL53L1_ReadMulti(dev, index, (uint8_t *)data, 1);
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data)
{
    uint8_t tmp[2];

    int8_t status = VL53L1_ReadMulti(dev, index, tmp, 2);

    *data = (((uint16_t)(tmp[0])) << 8) |
            (((uint16_t)(tmp[1])) << 0);

    return status;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data)
{
    uint8_t tmp[4];

    int8_t status = VL53L1_ReadMulti(dev, index, tmp, 4);

    *data = (((uint16_t)(tmp[0])) << 24) |
            (((uint16_t)(tmp[1])) << 16) |
            (((uint16_t)(tmp[2])) <<  8) |
            (((uint16_t)(tmp[3])) <<  0);

    return status;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms)
{
    int32_t timeout = ms + wait_ms;

    while(ms < timeout)
    {}

    return 0;
}
