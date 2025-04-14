# Solution

Solution to the assignment.

`Q1`

See [P3T1755 paragraph 7.5](./../../../../datasheets/P3T1755.pdf#7.5%20Register%20list){:target="_blank"}.

- T_LOW  = 0x4B00 = 75 degrees Celsius
- T_HIGH = 0x5000 = 80 degrees Celsius

# p3t1755_interrupt.h

```C
#ifndef P3T1755_H
#define P3T1755_H

#include <MCXA153.h>
#include <stdbool.h>

void p3t1755_init(void);

void p3t1755_get_configuration_reg_start(void);
bool p3t1755_get_configuration_reg_done(uint8_t *reg);

void p3t1755_set_configuration_reg_start(const uint8_t val);
bool p3t1755_set_configuration_reg_done(void);

void p3t1755_get_temperature_start(void);
bool p3t1755_get_temperature_done(float *temperature);

void p3t1755_get_t_low_start(void);
bool p3t1755_get_t_low_done(uint16_t *reg);

void p3t1755_get_t_high_start(void);
bool p3t1755_get_t_high_done(uint16_t *reg);

#endif // P3T1755_H
```

# p3t1755_interrupt.c

```C
void p3t1755_get_t_low_start(void)
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
    lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0b00000010));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA((0b1001000 << 1) | 1 ));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b001) | LPI2C_MTDR_DATA(1));
}

bool p3t1755_get_t_low_done(uint16_t *reg)
{
    // All bytes read?
    if(lpi2c0_rxcnt() < 2)
    {
        return false;
    }

    // Get the bytes
    uint16_t tmp = lpi2c0_getword() << 8;
    tmp |= lpi2c0_getword();

    *reg = tmp;

    return true;
}

void p3t1755_get_t_high_start(void)
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
    lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(0b00000011));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA((0b1001000 << 1) | 1 ));
    lpi2c0_putword(LPI2C_MTDR_CMD(0b001) | LPI2C_MTDR_DATA(1));
}

bool p3t1755_get_t_high_done(uint16_t *reg)
{
    // All bytes read?
    if(lpi2c0_rxcnt() < 2)
    {
        return false;
    }

    // Get the bytes
    uint16_t tmp = lpi2c0_getword() << 8;
    tmp |= lpi2c0_getword();

    *reg = tmp;

    return true;
}
```

# Timing diagrams

![I2C t_low](./../../../../imgs/i2c_interrupt_t_low.png)

![I2C t_high](./../../../../imgs/i2c_interrupt_t_high.png)
