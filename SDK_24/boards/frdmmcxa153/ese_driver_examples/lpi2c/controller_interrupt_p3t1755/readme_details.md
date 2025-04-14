# LPI2C Controller example - interrupt

Resources: *ese_driver_examples\lpi2c\controller_interrupt_p3t1755*

## Goal

To know how to configure the Low-Power I2C module for basic serial communication and use the LPI2C with interrupts.

## Circular buffer

Similar to using an interrupt driven UART, transmitted and received data needs to be stored temporarily. Fortunately, the MCXA153A microcontroller features a LPI2C module with a four word hardware FIFO (see [LPI2C - polling](../controller_polling_p3t1755/readme_details.md){:target="_blank"} for more info). However, if more than four words need to be temporary stored, a software solution is required. Similar to the UART, a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer){:target="_blank"} is a suitable data structure.

## API

For the [P3T1755](./../../../../datasheets/P3T1755.pdf){:target="_blank"}, the following API will be implemented:

```C
void p3t1755_init(void);

void p3t1755_get_configuration_reg_start(void);
bool p3t1755_get_configuration_reg_done(uint8_t *reg);

void p3t1755_set_configuration_reg_start(const uint8_t val);
bool p3t1755_set_configuration_reg_done(void);

void p3t1755_get_temperature_start(void);
bool p3t1755_get_temperature_done(float *temperature);
```

The function p3t1755_init() initializes the LPI2C module and after that, the [P3T1755](./../../../../datasheets/P3T1755.pdf){:target="_blank"} by sending the appropriate commands.

Communication with the [P3T1755](./../../../../datasheets/P3T1755.pdf){:target="_blank"} is done in two stages:

1. p3t1755_..._start()

    Starts a transfer by pushing all commands in the software transmit fifo.
    
2. p3t1755_..._done()

    Returns true if the expected data is received. It pops all expected data from the receive fifo, transforms the data if applicable, and returns the result by writing to the pointer that is provided as an argument.

To keep things straight forward, there is no internal checking of the order in which these functions are called. That means for example, when calling the p3t1755_get_configuration_reg_start() function, the main application should call p3t1755_get_configuration_reg_done() until there it returns true. If another function is called, let's say p3t1755_get_temperature_done(), incorrect data is returned!

## Putting a word

Transmitter interrupts are disabled by default. Putting a word takes the following steps:

1. Append the word to the transmit fifo.
2. Enable transmit interrupts.
3. Wait for the interrupt to occur. In the interrupt handler, read data from the transmit fifo and transmit this data via the LPI2C0->MTDR register.
4. If there is no more data, disable transmit interrupts.

## Getting a word

Receive interrupts are enabled by default. Getting a word takes the following steps:

1. Wait for an interrupt to occur. In the interrupt handler, read data from the LPI2C0->MRDR register and put it in the receive fifo.
2. Provide a function to check if data is available in the receive fifo.
3. If data is read, return the first item from the fifo.

## Initialization

When compared to the [LPI2C - polling](../controller_polling_p3t1755/readme_details.md){:target="_blank"} example, the following addition steps are required:

- Stop condition automatically generated. When using a software fifo, it is not desirable to wait until the stop condition command can be written to the MTDR register. Instead, the LPI2C module contains the option to automatically generate a stop condition when the LPI2C controller is busy and the transmit fifo is empty.
- Interrupts are enabled, in the LPI2C0 module and the NVIC.
- An interrupt handler is implemented, taking care of the transmission and reception of data by using the software fifo.

## Temperature reading

Reading the temperature is started as follows:

```C
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
```

The main can check if a new measurement is finished by using the following function:

```C
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
```

## Assignment

`Q1` According to the [P3T1755](./../../../../datasheets/P3T1755.pdf){:target="_blank"} datasheet, what is the default value of the registers T_LOW and T_HIGH?

Implement the following API functions:

```C
void p3t1755_get_t_low_start(void);
bool p3t1755_get_t_low_done(uint16_t *reg);

void p3t1755_get_t_high_start(void);
bool p3t1755_get_t_high_done(uint16_t *reg);
```

Test these functions by calling them in p3t1755_init() as follows:

```C
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

    uint16_t t_low = 0;
    uint16_t t_high = 0;

    p3t1755_get_t_low_start();

    // Wait for result
    while(!p3t1755_get_t_low_done(&t_low))
    {}

    p3t1755_get_t_high_start();

    // Wait for result
    while(!p3t1755_get_t_high_done(&t_low))
    {}

    if((t_low != 0x4B00) || (t_high != 0x5000))
    {
        // Error
        while(1)
        {}
    }

    #endif
}
```
 
Use a logic analyzer to visualize the SDA and SCL signals and verify that the default value of the registers T_LOW and T_HIGH indeed is transmitted.


[Solution](./solution.md){:target="_blank"}
