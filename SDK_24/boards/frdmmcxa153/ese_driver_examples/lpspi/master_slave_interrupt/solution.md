# Solution

Solution to the assignment.

# lpspi0_master.c

In the function lpspi0_master_tx() change the prescaler configuration to obtain a 125 kHz SCK.

```C
// CPOL: [0] = Clock polarity inactive low
// CPHA: [0] = Clock polarity captured
// PRESCALE: [011] = Divide by 4
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
LPSPI0->TCR = LPSPI_TCR_PRESCALE(0b011) | LPSPI_TCR_CONT(1) | LPSPI_TCR_FRAMESZ(7);
```

# main.c

```C
int main(void)
{
    lpspi1_slave_init();
    lpspi0_master_init();

    memset(lpspi0_tx_data, 0, N_DATA);
    memset(lpspi1_tx_data, 0, N_DATA);

    char *name = "Hello World!";
    char *num  = "123456";

    uint32_t name_len = strlen(name);
    uint32_t num_len = strlen(num);

    memcpy(lpspi0_tx_data, name, name_len);
    memcpy(lpspi1_tx_data, num, num_len);

    while(1)
    {
        // Prepare receive buffers
        memset(lpspi0_rx_data, 0, name_len);
        memset(lpspi1_rx_data, 0, name_len);

        // LPSPI1: Start receiving by using software FIFOs
        lpspi1_slave_rx_start(lpspi1_tx_data, name_len);

        // LPSPI0: transmit by using software FIFO
        lpspi0_master_tx(lpspi0_tx_data, name_len);

        // LPSPI1: read result from software FIFO
        lpspi1_slave_rx_read(lpspi1_rx_data, name_len);

        // LPSPI0: read result from software FIFO
        lpspi0_master_rx_read(lpspi0_rx_data, name_len);

        // Check the results
        if(memcmp(lpspi0_tx_data, lpspi1_rx_data, name_len) != 0)
        {
            // Transmission error from LPSPI0 to LPSPI1
            while(1)
            {}
        }

        if(memcmp(lpspi1_tx_data, lpspi0_rx_data, name_len) != 0)
        {
            // Transmission error from LPSPI1 to LPSPI0
            while(1)
            {}
        }

        // Delay
        for(uint32_t i=0; i<10000000; i++)
        {}
    }
}
```

# Timing diagrams

![SPI assignment](./../../../../imgs/spi_assignment.png)
