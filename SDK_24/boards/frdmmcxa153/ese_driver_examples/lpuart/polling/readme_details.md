# Low-Power UART - polling

Resources: *ese_driver_examples\lpuart\polling*

## Goal

To know how to configure the LPUART for basic serial communication and use the LPUART by polling bits in the status register.

## LPUART features

The MCXA153 has three instances of the LPUART module. These are called LPUART0, LPUART1 and LPUART2. The LPUART is described in the [reference manual chapter 37](./../../../../datasheets/MCXAP64M96FS3RM.pdf#37%20Low%20Power%20Universal%20Asynchronous%20Receiver/Transmitter%20(LPUART)){:target="_blank"}.

The MCU-Link on the FRDM-MCXA153 supports the VCOM serial port feature, which adds a serial COM port on the host computer and connects it to the target MCU while working as a USB-to-UART bridge.

Refer to [board schematic page 6](./../../../../datasheets/SPF-90829_A1.pdf#page=6){:target="_blank"}. It shows that the following pins are connected to the MCU-Link:

- P0_2 - LPUART0_RXD
- P0_3 - LPUART0_TXD

The PORT module must be used to configure the function for a pin. The [reference manual paragraph 10.3](./../../../../datasheets/MCXAP64M96FS3RM.pdf#10.3%20Pin%20Assignments){:target="_blank"} can be used to find the alternative functions that must be selected for both these pins:

- P0_2 - LPUART0_RXD - ALT2
- P0_3 - LPUART0_TXD - ALT2

## LPUART configuration

There are a lot of configuration options for he LPUART, however, configuring the LPUART for straight forward 8 data bit, no parity and 1 stop bit communication, is relatively easy.

The steps to take are:

1. Configure clock source in the MRCC module
2. Enable modules in the MRCC module
3. Configure alternate function in the PORT module
4. Set baud rate (depending on clock source) in the LPUART module
5. Enable receiver and/or transmitter in the LPUART module

These steps are explained in more detail in the following sections.

### 1. Configure clock source

```C
// Set clock source
// MUX: [010] = FRO_HF_DIV (defaults: FRO_HF = 48 MHz; DIV = 1)
MRCC0->MRCC_LPUART0_CLKSEL = MRCC_MRCC_LPUART0_CLKSEL_MUX(0b010);

// HALT: [0] = Divider clock is running
// RESET: [0] = Divider isn't reset
// DIV: [0000] = divider value = (DIV+1) = 1
MRCC0->MRCC_LPUART0_CLKDIV = 0;
```

### 2. Enable modules

```C
// Enable modules and leave others unchanged
// LPUART0: [1] = Peripheral clock is enabled
// PORT0: [1] = Peripheral clock is enabled
MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_LPUART0(1);
MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT0(1);

// Release modules from reset and leave others unchanged
// LPUART0: [1] = Peripheral is released from reset
MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_CC0_LPUART0(1);
MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_CC0_PORT0(1);
```

### 3. Configure alternate function

```C
// Configure P0_2
// LK : [1] = Locks this PCR
// INV: [0] = Does not invert
// IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
//            functions
// MUX: [0010] = Alternative 2 - LPUART0_RXD
// DSE: [0] = low drive strength is configured on the corresponding pin,
//            if the pin is configured as a digital output
// ODE: [0] = Disables
// SRE: [0] = Fast
// PE:  [0] = Disables
// PS:  [0] = n.a.
PORT0->PCR[2] = PORT_PCR_LK(1) | PORT_PCR_MUX(2) | PORT_PCR_IBE(1);

// Configure P0_3
// LK : [1] = Locks this PCR
// INV: [0] = Does not invert
// IBE: [0] = Input buffer disable
// MUX: [0010] = Alternative 2 - LPUART0_TXD
// DSE: [0] = low drive strength is configured on the corresponding pin,
//            if the pin is configured as a digital output
// ODE: [0] = Disables
// SRE: [0] = Fast
// PE:  [0] = Disables
// PS:  [0] = n.a.
PORT0->PCR[3] = PORT_PCR_LK(1) | PORT_PCR_MUX(2);
```

### 4. Set baud rate

The baud rate is set in the LPUART->BAUD register by the OSR and SBR bit fields. This register is described in the [reference manual](./../../../../datasheets/MCXAP64M96FS3RM.pdf){:target="_blank"}.

- OSR configures the oversampling ratio. The reset value is 0b01111, which results in an OSR of 16.
- SBR sets the modulo divide rate for the baud rate generator. The reset value is 0b0000000000100.

The reference manual description for SBR also shows the formula for calculating the ***baud rate***:

```C
// baud rate = baud clock / ((OSR + 1) * SBR)
//
// Rewritten: SBR = baud clock / (baud rate * (OSR+1))
```

In this formula, the ***baud clock*** is set in the MRCC in step 1., which in this example is set to 48 MHz.

```C
// Configure baud rate
// OSR: [01111] = Results in an OSR of 16 (15+1)
// SBR: [.............] = baud rate = baud clock / ((OSR + 1) * SBR)
//                        => SBR = baud clock / (baud rate * (OSR+1))
LPUART0->BAUD = LPUART_BAUD_OSR(0b01111) |
    LPUART_BAUD_SBR(CLK_FRO_48MHZ / (baudrate * 16));
```

### 5. Enable receiver and/or transmitter

The receiver and transmitter are enable in the LPUART0->CTRL register.

```C
// TE: [1] = Transmitter Enable
// RE: [1] = Receiver Enable
LPUART0->CTRL |= LPUART_CTRL_TE(1) | LPUART_CTRL_RE(1);
```

## Putting a character

A polling function for putting a character is implemented by checking the Transmit Data Register Empty Flag (TDRE). A while-loop waits as long as the transmit data register is empty.

```C
void lpuart0_putchar(const int data)
{
    while((LPUART0->STAT & LPUART_STAT_TDRE_MASK) == 0)
    {}

    LPUART0->DATA = (uint8_t)data;
}
```

## Getting a character

A polling function for getting a character is implemented by checking the Receive Data Register Full Flag (RDRF). A while-loop waits as long as the receive data register is not full.

```C
int lpuart0_getchar(void)
{
    while((LPUART0->STAT & LPUART_STAT_RDRF_MASK) == 0)
    {}

    return (uint8_t)(LPUART0->DATA);
}
```

## Final assignment

None.

## Extra: retarget IO

The functions *lpuart0_putchar()* and *lpuart0_getchar()* can be used for sending and receiving individual characters. But what if you wanted to print a 32-bit integer in hexadecimal format? In that case, it would be very convenient to have the stdio functions for formatting input and output, such as *printf()*. This is possible and is called *retargeting IO*.

IO formatting functions, such as *printf()*, are part of the [C standard library](https://en.wikipedia.org/wiki/C_standard_library){:target="_blank"}. These libraries can be implemented in different ways, for example by taking special considerations when dealing with the limited resources on embedded systems. When using a function from such a library, it is added to the executable by the linker.

Some examples of C standard libraries are:

- [Microsoft C run-time library](https://en.wikipedia.org/wiki/Microsoft_Windows_library_files#CRT){:target="_blank"}
- [GNU C Library](https://en.wikipedia.org/wiki/Glibc){:target="_blank"}
- [Newlib C standard library](https://en.wikipedia.org/wiki/Newlib){:target="_blank"}
- Newlib nano C standard library
- Redlib

Depending on the installed toolchain, one or more of these libraries might be available. Selecting a library is done in the project settings.

When using *printf()*, for example, on an embedded device without display, where should the printed string be displayed? The C standard libraries have an option to overwrite the built in `read()` and `write()` functions. These functions are declared so-called `weak`, which allows a developer to write his own functions. In these functions, IO can be retargeted to a specific interface, such as a UART, display, and/or USB. Different libraries may require different `read()` and `write()` functions.

For example:

- In Newlib these functions are:
```C
int _write(int fd, const void *buf, size_t count);
int _read(int fd, const void *buf, size_t count);
```
- In Redlib these functions are:
```C
int __sys_write(int handle, char *buffer, int size);
int __sys_readc(void);
```
- In MDK-ARM these functions are:
```C
int stdout_putchar(int ch);
int stdin_getchar(void);
```

These functions have been implemented in the file *retarget.c*. Notice the use of the defines *MDKARM*, *\__NEWLIB__*, and *\__REDLIB__*, for conditional compilation for more than one C standard library.

```C
#ifdef MDKARM

// Functions for redirecting standard output to LPUART0 for
// the ARM C libraries (used for example in MDK-ARM)

int stdout_putchar(int ch)
{
    lpuart0_putchar(ch);
    return ch;
}

int stdin_getchar(void)
{
    return lpuart0_getchar();
}

#endif

#ifdef __NEWLIB__

// Functions for redirecting standard output to LPUART0 for
// the Newlib C standard library

int _write(int fd, const void *buf, size_t count)
{
    (void)fd;

    for(size_t i=0; i<count; i++)
    {
        lpuart0_putchar(((char *)buf)[i]);
    }

    return count;
}

int _read(int fd, const void *buf, size_t count)
{
    (void)fd;

    for(size_t i=0; i<count; i++)
    {
        ((char *)buf)[i] = lpuart0_getchar();
    }

    return count;
}

#endif

#ifdef __REDLIB__

// Functions for redirecting standard output to LPUART0 for
// the Redlib C standard library

int __sys_write(int handle, char *buffer, int size)
{
    if (NULL == buffer)
    {
        // return -1 if error
        return -1;
    }

    // This function only writes to "standard out" and "standard err" for
    // all other file handles it returns failure
    if ((handle != 1) && (handle != 2))
    {
        return -1;
    }

    // Send data
    for(size_t i=0; i<size; i++)
    {
        lpuart0_putchar(((char *)buffer)[i]);
    }

    return size;
}

int __sys_readc(void)
{
    return lpuart0_getchar();
}

#endif
```

C standard libraries for embedded systems tend to have printing of floating point numbers disabled. The following sections describe how to do this for *MCUXpresso-IDE* and *MCUXpresso for VSCode*.

### MCUXpresso-IDE

MCUXpresso-IDE projects use Redlib by default. Two defines need to be updated in the project settings as depicted below:

- Delete the define CR_INTEGER_PRINTF
- Set PRINTF_FLOAT_ENABLE=1

![Enable printing floats](./../../../../imgs/lpuart_polling_printf_float.png)

### MCUXpresso for VSCode

MCUXpresso for VSCode projects use Newlib by default.

- Open the file *armgcc\flags.cmake*
- Find the following:

``` TXT
IF(NOT DEFINED SPECS)
    SET(SPECS "--specs=nano.specs --specs=nosys.specs")
ENDIF()
```

- Add the linker flags `-u _printf_float` as follows:

``` TXT
IF(NOT DEFINED SPECS)
    SET(SPECS "--specs=nano.specs -u _printf_float --specs=nosys.specs")
ENDIF()
```

> *Note: In general, stdio printing (such as printf()), should not be performed in an interrupt handler!*
