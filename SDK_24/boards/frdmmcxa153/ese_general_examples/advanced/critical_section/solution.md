# Solution

Solution to the assignment.

`Q1` It takes longer to make the copy, so the chance of interrupting this process increases.

`Q2`

```C
// Critical section start
uint32_t m = __get_PRIMASK();
__disable_irq();             

// Copy the global buffer
for(uint32_t i=0; i<sizeof(global_buffer); i++)
{
    local_buffer[i] = global_buffer[i];
    
    // Artificial delay to increase the chance of interrupting this
    // critical section
    for(uint32_t d=0; d<200000; d++)
    {}                
}

__set_PRIMASK(m);
// Critical section end   
```

`Q3` The first click copies the buffer and prints it. Printing means it is written into the transmit buffer of the LPUART, which is transmitted interrupt based. If there is a second SW3 click, which disables all interrupts, the transmission of UART characters will temporarily stop.
