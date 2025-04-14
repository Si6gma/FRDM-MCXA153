# LPSPI Master and slave example - interrupt

Resources: *ese_driver_examples\lpspi\master_slave_interrupt*

## Goal

To know how to configure the Low-Power SPI module for basic serial communication and use the SPI with interrupts.

## Circular buffer

Similar to using an interrupt driven UART and I2C, transmitted and received data needs to be stored temporarily. Fortunately, the MCXA153A microcontroller features a LPSPI module with a four word hardware FIFO (see [LPSPI - polling](../master_slave_polling/readme_details.md){:target="_blank"} for more info). However, if more than four words need to be temporary stored, a software solution should be made. Similar to the UART and I2C, a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer){:target="_blank"} is a suitable data structure.

## API

The API for both master and slave will be as follows:

```C
// Initialize the LPSPI module in master mode
void lpspi0_master_init(void);

// Writes n bytes from buffer to a software FIFO and initiates the first SPI transfer to the slave
void lpspi0_master_tx(uint8_t *buffer, const uint32_t n);

// Reads n bytes from a software FIFO into buffer
void lpspi0_master_rx_read(uint8_t *buffer, const uint32_t n);

// Initialize the LPSPI module in slave mode
void lpspi1_slave_init(void);

// Writes n bytes from buffer to a software FIFO to be transmitted to the master
void lpspi1_slave_rx_start(uint8_t *buffer, const uint32_t n);

// Reads n bytes from a software FIFO into buffer
void lpspi1_slave_rx_read(uint8_t *buffer, const uint32_t n);
```

This API is similar to the [LPSPI - polling](../master_slave_polling/readme_details.md){:target="_blank"} example. However, due to the software FIFO, the number of data items is restricted by the size of the software FIFO and not the 4-word hardware FIFO.

## Assignment

Change the example so it transmits the ASCII characters of your name from master to slave. The slave sends back your student number, followed by 0x00 for any remaining characters. Use a logic analyzer to visualize the MOSI, MISO, SCK and CS0 signals.
Also change the clock settings so the SCK frequency is equal to 125 kHz. Tip: only the master clock configuration needs to be updated.

[Solution](./solution.md){:target="_blank"}
