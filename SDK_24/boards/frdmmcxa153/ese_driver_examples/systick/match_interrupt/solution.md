# Solution

Solution to the assignment.

`Q1` The green LED blinks with a frequency of 1 Hz. The modulo operator is used to check if the variable is a multiple of 1000.

`Q2` (2^32)-1 ms = 4294967295 ms ~= 1193 h ~= 49.7 days

# main.c 

```C
// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile uint32_t ms = 0;
static volatile uint32_t previous_ms = 0;
static const uint32_t interval_ms = 2000;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    gpio_output_init();
    
    // Generate an interrupt every 1 ms
    SysTick_Config(48000);    
        
    // Enable interrupts
    __enable_irq();    
    
    while(1)
    {
        // Wait for interrupt
        __WFI();

        uint32_t current_ms = ms;

        // Interval milliseconds passed?
        if((current_ms - previous_ms) >= interval_ms)
        {
            previous_ms = current_ms;

            // Toggle the red LED
            GPIO3->PTOR = (1<<12);
        }
    }
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------

void SysTick_Handler(void)
{
    ms++;

    if((ms % 1000) == 0)
    {
        // Toggle the green LED
        GPIO3->PTOR = (1<<13);
    }
}
```
