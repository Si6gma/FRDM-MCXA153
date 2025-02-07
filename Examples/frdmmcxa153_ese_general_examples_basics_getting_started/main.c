#include <MCXA153.h>

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static volatile uint32_t systick_flag = 0;

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    // Initialize P3_13 GPIO output - green RGB LED
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);
    PORT3->PCR[13] = PORT_PCR_LK(1);
    GPIO3->PDOR |= (1 << 13);
    GPIO3->PDDR |= (1 << 13);

    // Initialize SysTick to generate an interrupt every 100ms
    SysTick_Config(4800000);

    // Enable interrupts
    __enable_irq();

    while (1)
    {
        // Wait for interrupt
        __WFI();

        // Check the SysTick flag
        if (systick_flag == 1)
        {
            // Reset the flag
            systick_flag = 0;

            // Toggle the pin
            GPIO3->PTOR = (1 << 13);
        }
    }
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    systick_flag = 1;
}
