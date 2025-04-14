# Solution

Solution to the assignment.

Add the following global variables:

```C
static const uint32_t interval_ms = 1000;
static uint32_t previous_ms = 0;
```

In main while-loop:

```C
uint32_t current_ms = ms;

// --------------------------------------------------------------------
// One second event
if((current_ms - previous_ms) >= interval_ms)
{
    previous_ms = current_ms;

    printf("[%08lu] event: one second\r\n", ms);

}
```

