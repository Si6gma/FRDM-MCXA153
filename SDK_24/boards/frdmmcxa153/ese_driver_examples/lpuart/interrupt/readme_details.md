# Low-Power UART - interrupt

Resources: *ese_driver_examples\lpuart\interrupt*

## Goal

To understand what it takes to use the LPUART with interrupts when compared to polling.

## Interrupts instead of polling

Using LPUART transmitter and receiver interrupts is actually pretty straight forward, as soon as polling has been implemented:

1. Enable the interrupts in the LPUART0->CTRL register.
2. Enable LPUART0 interrupts in the NVIC and set priority.
3. Implement an interrupt handler called LPUART0_IRQHandler().

The hard part is to implement a data structure to:

- Temporary store incoming data
- Temporary store outgoing data

## Circular buffer

One way to implement a data structure for storing data is a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer){:target="_blank"}. One advantage is that it uses static memory, of which the size is set at compile time. This makes it a fast implementation, when, for example, compared to a dynamically allocated singly linked list.

For a modular approach, the circular buffer is implemented in the files *fifo.c* and *fifo.h*.

## Putting a character

Transmitter interrupts are disabled by default. Putting a character takes the following steps:

1. Append the character to the transmit fifo.
2. Enable transmit interrupts.
3. Wait for the interrupt to occur. In the interrupt handler, read data from the transmit fifo and transmit this data via the LPUART0->DATA register.
4. If there is no more data, disable transmit interrupts.

*`Q1` Examine the file lpuart0_interrupt.c. Relate all of these steps to the respective line(s) of code.*

*`Q2` What will happen if the transmit fifo is full?*

## Getting a character

Receive interrupts are enabled by default. Getting a character takes the following steps:

1. Wait for an interrupt to occur. In the interrupt handler, read data from the LPUART0->DATA register and put it in the receive fifo.
2. Provide a function to check if data is available in the receive fifo.
3. If data is read, return the first item from the fifo.

*`Q3` Examine the file lpuart0_interrupt.c. Relate all of these steps to the respective line(s) of code.*

*`Q4` Examine the file lpuart0_interrupt.c. What will happen if the receive fifo is full?*

## Assignment

*`Q5` At 115200-8n1, how long does it take to transmit 1024 bytes?*

Implement a driver to support LPUART2. Proceed as follows:

- Copy the file lpuart0_interrupt.h and rename it to lpuart2_interrupt.h.
- Copy the file lpuart0_interrupt.c and rename it to lpuart2_interrupt.c.
- Add both files to the project.
- Rewrite all functions and function prototypes to support LPUART2. Configure the pins as follows:
    - LPUART2_RXD - P1_4
    - LPUART2_TXD - P1_5
- Connect a USB-to-UART convertor or a Bluetooth module for testing and verification.

[Solution](./solution.md){:target="_blank"}
