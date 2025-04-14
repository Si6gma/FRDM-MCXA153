/*! ***************************************************************************
 *
 * \brief     Low level driver for the Low Power Serial Peripheral Interface
 *            (LPSPI) in master mode controlling an M950x0-R EEPROM
 * \file      eeprom.c
 * \author    Hugo Arends
 * \date      March 2024
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
 ******************************************************************************/
#include <stddef.h>
#include "eeprom.h"
#include "lpspi1_master.h"
#include "fifo.h"

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
void eeprom_init(void)
{
    // Enable modules and leave others unchanged
    // PORT3: [1] = Peripheral clock is enabled
    // GPIO3: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    // Release modules from reset and leave others unchanged
    // PORT3: [1] = Peripheral is released from reset
    // GPIO3: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    // Configure P3_11
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0010] = Alternative 2
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disables
    // PS:  [0] = n.a.
    PORT3->PCR[11] = PORT_PCR_LK(1) | PORT_PCR_MUX(2) | PORT_PCR_IBE(1); // LPSPI1_PCS0

    lpspi1_master_init();
}

/*
void lpspi1_tx(uint8_t *buffer, const uint32_t n, uint32_t rxmsk)
{
    // Wait as long as bus or controller is busy
    lpspi1_wait_busy();

    // Copy data to FIFO
    for(uint32_t i=0; i<n; i++)
    {
        f_push(&tx, buffer[i]);
    }

    // Flush FIFOs
    LPSPI1->CR |= LPSPI_CR_RTF(1) | LPSPI_CR_RRF(1);

    // Clear all status flags
    LPSPI1->SR = LPSPI_SR_MBF_MASK | LPSPI_SR_DMF_MASK | LPSPI_SR_REF_MASK |
        LPSPI_SR_TEF_MASK | LPSPI_SR_TCF_MASK | LPSPI_SR_FCF_MASK |
        LPSPI_SR_WCF_MASK | LPSPI_SR_RDF_MASK | LPSPI_SR_TDF_MASK;

    // CPOL: [0] = Clock polarity inactive low
    // CPHA: [0] = Clock polarity captured
    // PRESCALE: [000] = Divide by 1
    // PCS: [00] = Transfer using PCS[0]
    // LSBF: [0] = MSB first
    // BYSW: [0] = Disable byte swap
    // CONT: [1] = Continuous transfer enabled.  In Master mode, this field
    //             keeps PCS asserted at the end of the frame size until a
    //             command word is received that starts a new frame.
    // CONTC: [0] = Command word for start of new transfer
    // RXMSK: [0] = Normal transfer
    // TXMSK: [0] = Normal transfer
    // WIDTH: [00] = 1-bit transfer
    // FRAMESZ: [000000000111] = Frame size = (7+1)
    LPSPI1->TCR = LPSPI_TCR_PRESCALE(0b000) | LPSPI_TCR_CONT(1) |
        LPSPI_TCR_FRAMESZ(7) | LPSPI_TCR_RXMSK(rxmsk);

    // Note: In case receiving is not used, set RXMSK [1]

    // Enable transmit interrupt
    LPSPI1->IER = LPSPI_IER_RDIE(1) | LPSPI_IER_TDIE(1);
}

void lpspi1_rx_read(uint8_t *buffer, const uint32_t n)
{
    for(uint32_t i=0; i<n; i++)
    {
        // Wait if the FIFO is empty
        while(f_cnt(&rx) == 0)
        {}

        f_pop(&rx, &buffer[i]);
    }
}

inline void lpspi1_wait_busy(void)
{
    // Wait as long as Bus Busy Flag or Controller Busy Flag is set
    while((LPSPI1->SR & LPSPI_SR_MBF_MASK) != 0)
    {}
}

void LPSPI1_IRQHandler(void)
{
    uint8_t c;

    // Clear the interrupt
    NVIC_ClearPendingIRQ(LPSPI1_IRQn);

    // Data transmitted?
    if((LPSPI1->SR & LPSPI_SR_TDF_MASK) != 0)
    {
        // Send another character?
        if(f_pop(&tx, &c))
        {
            LPSPI1->TDR = c;
        }
        else
        {
            // CONT: [1] = Continuous transfer disabled
            LPSPI1->TCR &= ~(LPSPI_TCR_CONT(1));

            // FIFO is empty so disable TDRE interrupt
            LPSPI1->IER &= ~LPSPI_IER_TDIE_MASK;
        }
    }

    // Data received?
    if((LPSPI1->SR & LPSPI_SR_RDF_MASK) != 0)
    {
        // Read data
        c = (uint8_t)(LPSPI1->RDR);

        // Put in receive FIFO
        if(!f_push(&rx, c))
        {
            // Error: receive FIFO full!!
            // Should not happen, so freeze the system. Update FIFO size to
            // match your application.
            while (1)
            {}
        }
    }
}
*/

void eeprom_read(uint8_t address, uint8_t *buffer, const uint32_t n)
{
    // READ Read from memory array 0000X011
    f_push(&tx, 0x03); // Instruction
    f_push(&tx, address); // Address

    // Copy dummy data to transmit FIFO
    for(uint32_t i=0; i<n; i++)
    {
        f_push(&tx, 0x00);
    }

    // Start transfer, TXFIFO is already filled
    lpspi1_master_tx(NULL, 0, 0, 0);

    // Read the data from the RXFIFO
    lpspi1_master_rx_read(buffer, n);
}

void eeprom_write(uint8_t address, uint8_t *buffer, const uint32_t n)
{
    // WRITE Write to memory array 0000X010
    f_push(&tx, 0x02); // Instruction
    f_push(&tx, address); // Address

    // Write the data, ignore data from EEPROM
    lpspi1_master_tx(buffer, n, 0, 1);
}

uint8_t eeprom_rdsr(void)
{
    // RDSR Read Status register 0000 X101
    uint8_t tx_data[2] = {0x05, 0x00};
    uint8_t rx_data[2] = {0x00, 0x00};

    lpspi1_master_tx(tx_data, sizeof(tx_data), 0, 0);
    lpspi1_master_rx_read(rx_data, sizeof(rx_data));

    return rx_data[1];
}

bool eeprom_wip(void)
{
    uint8_t sr = eeprom_rdsr();

    return (sr & 0x01) != 0;
}

void eeprom_we(bool wel)
{
    // WREN Write enable 0000X110
    // WRDI Write disable 0000X100

    uint8_t tx_data[1] = {wel ? 0x06 : 0x04};
    uint8_t rx_data[1] = {0x00};

    lpspi1_master_tx(tx_data, sizeof(tx_data), 0, 0);
    lpspi1_master_rx_read(rx_data, sizeof(rx_data));
}
