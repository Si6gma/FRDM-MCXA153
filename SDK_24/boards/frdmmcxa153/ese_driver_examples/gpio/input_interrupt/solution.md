# Solution

Solution to the assignment.

# gpio_input_interrupt.h

```C
#ifndef GPIO_INPUT_INTERRUPT_H
#define GPIO_INPUT_INTERRUPT_H

#include <MCXA153.h>
#include <stdbool.h>

void gpio_input_init(void);
bool gpio_input_sw2_pressed(void);

#endif // GPIO_INPUT_INTERRUPT_H
```

---

# gpio_input_interrupt.c

```C
void gpio_input_init(void)
{
    // From section 14.3 Functional description (NXP, 2024)
    //
    // Before a module can be used, its clocks must be enabled (CC != 00) and it
    // must be released from reset (MRCC_GLB_RST [peripherals name] = 1). If a
    // module is not released from reset (MRCC_GLB_RST [peripherals name] = 0),
    // an attempt to access a register within that module is terminated with a
    // bus error.

    // Enable modules and leave others unchanged
    // PORT1: [1] = Peripheral clock is enabled
    // GPIO1: [1] = Peripheral clock is enabled
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    // Release modules from reset and leave others unchanged
    // PORT1: [1] = Peripheral is released from reset
    // GPIO1: [1] = Peripheral is released from reset
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    // From section 11.4 Initialization (NXP, 2024)
    //
    // 1. Initialize the pin functions:
    //    -  Initialize single pin functions by writing appropriate values to
    //       PCRn
    //    -  Initialize multiple pins (up to 16) with the same configuration by
    //       writing appropriate values to Global Pin Control Low (GPCLR) or
    //       Global Pin Control High (GPCHR).
    // 2. Lock the configuration for a given pin, by writing 1 to PCRn [LK], so
    //    that it cannot be changed until the next reset.

    // 1. & 2.
    //
    // Configure pin P3_29
    // LK : [1] = Locks this PCR
    // INV: [0] = Does not invert
    // IBE: [1] = Digital Input Buffer Enable, otherwise pin is used for analog
    //            functions
    // MUX: [0000] = Alternative 0 (GPIO)
    // DSE: [0] = low drive strength is configured on the corresponding pin,
    //            if the pin is configured as a digital output
    // ODE: [0] = Disables
    // SRE: [0] = Fast
    // PE:  [0] = Disables
    // PS:  [0] = n.a.
    PORT3->PCR[29] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0);
    
    // From section 12.5 Initialization (NXP, 2024)
    //
    // 1. n.a.
    // 2. Initialize the interrupt function by writing to Interrupt Control a 
    //    (ICR0 - ICR31) for the corresponding pins and desired configuration.
    //    If the pin is previously used for a different function, first write
    //    0100_0000h to Interrupt Control a (ICR0 - ICR31) to disable the
    //    previous function and clear the flag.

    // 2.
    //
    // ISF: [1] = Clear the flag
    // IRQC : [1010] = ISF and interrupt on falling edge
    GPIO3->ICR[29] = GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1010);
    
    // Enable GPIO3 interrupts
    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);

    // Globally enable interrupts
    __enable_irq();
}

// Interrupt handler
// Refer to the file startup_MCXA153.s for finding the function name.
void GPIO3_IRQHandler(void)
{
    // Clear the interrupt
    NVIC_ClearPendingIRQ(GPIO3_IRQn);

    // Interrupt handler triggered by P3_29?
    if((GPIO3->ISFR[0] & GPIO_ISFR_ISF29(1)) != 0)
    {
        // Clear the flag
        GPIO3->ISFR[0] = GPIO_ISFR_ISF29(1);
        
        // Handle the event
        cnt++;
    }
}

bool gpio_input_sw2_pressed(void)
{
    // Any presses detected since the last time checked?
    if(cnt > 0)
    {
        // Decrement presses counter
        cnt--;

        return true;
    }
    else
    {
        return false;
    }
}
```

---

# main.c

```C
int main(void)
{
    gpio_input_init();
    gpio_output_init();

    while(1)
    {
        // Wait for interrupt
        __WFI();
        
        // Switch pressed?
        if(gpio_input_sw2_pressed())
        {
            // Toggle the green RGB LED
            GPIO3->PTOR = (1<<13);
        }
    }
}
```
