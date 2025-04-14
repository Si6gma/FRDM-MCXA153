/*! ***************************************************************************
 *
 * \brief     Low-Power I2C controller interrupt
 * \file      lpi2c0_target_interrupt.c
 * \author    Hugo Arends
 * \date      June 2024
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
#include "lpi2c0_target_interrupt.h"
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
static fifo_t tx;
static fifo_t rx;
static uint8_t tx_buffer[32];
static uint8_t rx_buffer[32];

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void lpi2c0_target_init(void)
{
    // Initialize FIFOs
    f_init(&tx, tx_buffer, sizeof(tx_buffer));
    f_init(&rx, rx_buffer, sizeof(rx_buffer));

    // Set clock source
    // MUX: [010] = FRO_HF_DIV
    MRCC0->MRCC_LPI2C0_CLKSEL = MRCC_MRCC_LPI2C0_CLKSEL_MUX(2);

    // HALT: [0] = Divider clock is running
    // RESET: [0] = Divider isn't reset
    // DIV: [0000] = divider value = (DIV+1) = 1
    MRCC0->MRCC_LPI2C0_CLKDIV = 0;

    // Enable modules and leave others unchanged
    // LPI2C0: [1] = Peripheral clock is enabled
    // PORT1: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_LPI2C0(1);
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT1(1);

    // Release modules from reset and leave others unchanged
    // LPI2C0: [1] = Peripheral is released from reset
    // PORT1: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_LPI2C0(1);
    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT1(1);

    // Configure P1_8 and P1_9
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0011] = Alternative 3
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disable
    // PS:  [0] = n.a.
    PORT1->PCR[8] = PORT_PCR_LK(1) | PORT_PCR_MUX(3) | PORT_PCR_PE(1) |
        PORT_PCR_PS(1) | PORT_PCR_ODE(1) | PORT_PCR_IBE(1); // LPI2C0_SDA
    PORT1->PCR[9] = PORT_PCR_LK(1) | PORT_PCR_MUX(3) | PORT_PCR_PE(1) |
        PORT_PCR_PS(1) | PORT_PCR_ODE(1) | PORT_PCR_IBE(1); // LPI2C0_SCL

    // From section 36.5 Initialization (NXP, 2024)
    //
    // To initialize the LPI2C target:

    // 1. Configure Target Address Match (SAMR) with the I2C address of the
    //    target location on the I2C bus.
    // 2. Configure Target Configuration 0 (SCFGR0) and Target Configuration 1
    //    (SCFGR1) as required by the application.
    // 3. Configure Target Configuration 2 (SCFGR2) to satisfy the timing
    //    requirements of the I2C mode supported by the application.
    // 4. Enable target interrupts and DMA requests as required by the
    //    application.
    // 5. Enable the LPI2C target by writing 1 to SCR[SEN].

    // 1.
    //
    LPI2C0->SAMR = LPI2C_SAMR_ADDR0(0x7E);

    // 2.
    //
    // TXDSTALL: [1] = Enables SCL clock stretching when SSR[TDF] = 1 during a
    //                 target-transmit transfer. If clock stretching is enabled
    //                 the transmit data transfer is stalled until this
    //                 register is updated.
    LPI2C0->SCFGR1 = LPI2C_SCFGR1_TXDSTALL(1);

    // 3.
    //
    // I2C timing parameters to setup the following specifications (see
    // paragraph 36.6 (NXP, 2024)):
    // I2C mode: FAST
    // Clock frequency: 48 MHz
    // I2C baud rate: 400 kbits/s
    LPI2C0->SCFGR2 = LPI2C_SCFGR2_FILTSDA(1) | LPI2C_SCFGR2_FILTSCL(1) |
        LPI2C_SCFGR2_DATAVD(0x0F) | LPI2C_SCFGR2_CLKHOLD(0x1D);

    // 4.
    //
    // RDIE: [1] = Receive Data Interrupt Enable
    LPI2C0->SIER |= LPI2C_SIER_RDIE(1);

    // Enable LPI2C0 interrupts
    NVIC_SetPriority(LPI2C0_IRQn, 3);
    NVIC_ClearPendingIRQ(LPI2C0_IRQn);
    NVIC_EnableIRQ(LPI2C0_IRQn);

    // Globally enable interrupts
    __enable_irq();

    // Make sure the bus is not busy before enabling the controller
    lpi2c0_wait_busy();

    // 5.
    //
    // SEN: [1] = Target Enable
    LPI2C0->SCR |= LPI2C_SCR_SEN(1);
}

void lpi2c0_putchar(const uint8_t data)
{
    // Wait for space to open up
    while(!f_push(&tx, data))
    {}

    // TDIE: [1] = Transmit Data Interrupt Enable
    LPI2C0->SIER |= LPI2C_SIER_TDIE(1);
}

uint8_t lpi2c0_getchar(void)
{
    uint8_t c=0;

    // Wait for data.
    // If waiting is not desired, call the function lpi2c0_rxcnt() first to
    // make sure data is available.
    while(!f_pop(&rx, &c))
    {}

    return c;
}

uint32_t lpi2c0_rxcnt(void)
{
    return f_cnt(&rx);
}

uint32_t lpi2c0_txcnt(void)
{
    return f_cnt(&tx);
}

inline void lpi2c0_wait_busy(void)
{
    // Wait as long as Bus Busy Flag or Controller Busy Flag is set
    while((LPI2C0->SSR & (LPI2C_SSR_BBF_MASK | LPI2C_SSR_SBF_MASK)) != 0)
    {}
}

void LPI2C0_IRQHandler(void)
{
    uint8_t c;

    NVIC_ClearPendingIRQ(LPI2C0_IRQn);

    // Transmit interrupt?
    if((LPI2C0->SSR & LPI2C_SSR_TDF_MASK) != 0)
    {
        // Put as many data items in the transmit fifo
        if(f_pop(&tx, &c))
        {
            LPI2C0->STDR = c;
        }
        else
        {
            // TDIE: [0] = Transmit Data Interrupt Disable
            LPI2C0->SIER &= ~LPI2C_SIER_TDIE(1);
        }
    }

    // Receive interrupt?
    if((LPI2C0->SSR & LPI2C_SSR_RDF_MASK) !=0)
    {
        // Read data
        c = (uint8_t)(LPI2C0->SRDR);

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
