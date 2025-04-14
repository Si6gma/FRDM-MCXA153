# Solution

Solution to the assignment.

`Q1` The TOP value (VAL1) is 59.

`Q2` The time between two reload interrupt events is (TOP+1) x 1/48MHz = 1.25us.

`Q3` These durations are:

- T0H = (15+1) x 1/48MHz = 333ns
- T0L = ((59+1)-16) x 1/48MHz = 916ns
- T1H = (30+1) x 1/48MHz = 645ns
- T1L = ((59+1)-31) x 1/48MHz = 604ns

`Q4` See the following result:

![neopixel](./../../../../imgs/neopixel.png)

*Notice that the samples were taken with 100MS/s (100 million samples per second). The precision of the sample is therefore 1/100M = 10ns. Hence the reason for the the rounded numbers in the image.*

`Q5`

main.c

```C
static volatile uint32_t direction = 0;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    // 96 MHz FIRC clock selected
    SCG0->FIRCCFG = SCG_FIRCCFG_FREQ_SEL(0b101);
        
    gpio_output_init();
    gpio_input_init();
    neopixel_init();

    // Initialize SysTick to generate an interrupt every 1ms
    SysTick_Config(96000);    
    NVIC_SetPriority(SysTick_IRQn, 1);
    
    rgb_t pixels[N_NEOPIXELS] = {0};
    neopixel_update(pixels);
    while(ms < 500)
    {}

    const uint8_t brightness = 10;

    while(1)
    {        
        uint32_t current_ms = ms;
        
        if((current_ms - previous_ms) >= interval_ms)
        {
            previous_ms = current_ms;
            
            // All LEDs of in current pixel
            pixels[cnt] = (rgb_t){0,0,0};

            // Next pixel, depending on direction
            if(direction == 0)
            {
                cnt = cnt + 1;
                
                if(cnt >= N_NEOPIXELS)
                {
                    cnt = 0;
                    color = (color + 1) % 3;
                }
            }
            else
            {
                cnt = cnt - 1;
                
                if(cnt < 0)
                {
                    cnt = N_NEOPIXELS-1;
                    color = (color + 1) % 3;
                }
            }
            
            // Selected color on in selected pixel
            pixels[cnt] = (rgb_t){color == 0 ? brightness : 0,
                                  color == 1 ? brightness : 0,
                                  color == 2 ? brightness : 0};

            neopixel_update(pixels);
        }

        // SW3 pressed?
        if(gpio_input_sw3_pressed())
        {
            direction = !direction;
        }
    }
}
```
