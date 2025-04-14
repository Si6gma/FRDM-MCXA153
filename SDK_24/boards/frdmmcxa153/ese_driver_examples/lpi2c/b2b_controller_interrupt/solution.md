# Solution

Solution to the assignment.

# main.c

```C
while(1)
{
    uint32_t current_ms = ms;

    // --------------------------------------------------------------------
    if((current_ms - previous_ms) >= interval_ms)
    {
        previous_ms = current_ms;

        // Wait as long as bus or controller is busy
        lpi2c0_wait_busy();

        // Clear all status flags
        LPI2C0->MSR = LPI2C_MSR_STF_MASK | LPI2C_MSR_DMF_MASK |
            LPI2C_MSR_PLTF_MASK | LPI2C_MSR_FEF_MASK | LPI2C_MSR_ALF_MASK |
            LPI2C_MSR_NDF_MASK | LPI2C_MSR_SDF_MASK | LPI2C_MSR_EPF_MASK;

        // Fill transmit fifo
        lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(0x7E << 1));
        lpi2c0_putword(LPI2C_MTDR_CMD(0b000) | LPI2C_MTDR_DATA(rgb));
        lpi2c0_putword(LPI2C_MTDR_CMD(0b100) | LPI2C_MTDR_DATA(0x7E << 1) | 1);
        lpi2c0_putword(LPI2C_MTDR_CMD(0b001) | LPI2C_MTDR_DATA(2));

        // Red
        if((rgb & 0x01) == 0)
        {
            GPIO3->PSOR = (1<<12);
        }
        else
        {
            GPIO3->PCOR = (1<<12);
        }

        // Green
        if((rgb & 0x02) == 0)
        {
            GPIO3->PSOR = (1<<13);
        }
        else
        {
            GPIO3->PCOR = (1<<13);
        }

        // Blue
        if((rgb & 0x04) == 0)
        {
            GPIO3->PSOR = (1<<0);
        }
        else
        {
            GPIO3->PCOR = (1<<0);
        }

        rgb = (rgb == 4) ? 1 : rgb << 1;
    }

    if(lpi2c0_rxcnt() >= 3)
    {
        uint8_t data1 = lpi2c0_getword();
        uint8_t data2 = lpi2c0_getword();
        uint8_t data3 = lpi2c0_getword();

        // Check data from target
        if((data1 != 0x01) || (data2 != 0x02) || (data3 != 0x03))
        {
            // Data invalid
            GPIO3->PCOR = (1<<12);
            GPIO3->PSOR = (1<<13);
            GPIO3->PSOR = (1<<0);

            while(1)
            {}
        }
    }
}
```
