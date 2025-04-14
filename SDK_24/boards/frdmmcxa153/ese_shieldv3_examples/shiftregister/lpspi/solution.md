# Solution

Solution to the assignment.

# hc595.c

```C
void hc595_init(void)
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

    // Configure pins P3_1
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0000] = Alternative 0 (GPIO)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [1] = Enables
    // PS:  [1] = Enables internal pullup resistor
    PORT3->PCR[1] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);
    
    // P3_1 output
    GPIO3->PDDR |= (1<<1);
    
    // Toggle the #SHIFTREG_RS pin
    GPIO3->PCOR = (1<<1);
    GPIO3->PSOR = (1<<1);
    
    lpspi1_master_init();
}

void hc595_write(uint8_t data)
{
    lpspi1_master_tx(&data, 1);
}

```

---

# main.c

```C
// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile uint32_t ms = 0;
static volatile uint32_t previous_ms = 0;
static const uint32_t interval_ms = 125;

static uint8_t leds = 0;
static int32_t cnt = 0;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    hc595_init();

    // Generate an interrupt every 1 ms
    SysTick_Config(48000);   

    leds = 0x01;
    hc595_write(leds);

    while(1)
    {
        uint32_t current_ms = ms;

        // --------------------------------------------------------------------
        if((current_ms - previous_ms) >= interval_ms)
        {
            previous_ms = current_ms;

            // Next LED
            cnt++;

            cnt = cnt % 8;
            leds = (uint8_t)(0x01 << cnt);
            hc595_write(leds);
        }
    }
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    ms++;
}

```
