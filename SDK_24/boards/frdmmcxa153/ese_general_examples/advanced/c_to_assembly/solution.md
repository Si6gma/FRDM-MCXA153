# Solution

Solution to the assignment.

`Q1` Given the following example application. For each variable:

1. Will it be located in RAM or ROM?
2. If in RAM, will it be located in the stack, heap, or static region?

```C
int var01 = 0;                        // RAM - static
unsigned char var02 = 0;              // RAM - static
char var03[32] = {0};                 // RAM - static

static int var04 = 0;                 // RAM - static
static unsigned char var05 = 0;       // RAM - static
static char var06[32] = {0};          // RAM - static

volatile int var07 = 0;               // RAM - static
volatile unsigned char var08 = 0;     // RAM - static
volatile char var09[32] = {0};        // RAM - static

const int var10 = 0;                  // ROM
const unsigned char var11 = 0;        // ROM
const char var12[32] = {0};           // ROM

int func(int a, int b)
{
    int var13 = 0;                    // RAM - stack
    unsigned char var14 = 0;          // RAM - stack
    char var15[32] = {0};             // RAM - stack

    static int var16 = 0;             // RAM - static
    static unsigned char var17 = 0;   // RAM - static
    static char var18[32] = {0};      // RAM - static

    volatile int var19 = 0;           // RAM - stack
    volatile unsigned char var20 = 0; // RAM - stack
    volatile char var21[32] = {0};    // RAM - stack

    const int var22 = 0;              // ROM
    const unsigned char var23 = 0;    // ROM
    const char var24[32] = {0};       // ROM

    return a + b;
}

int main(void)
{
    int var25 = 0;                    // RAM - stack
    unsigned char var26 = 0;          // RAM - stack
    char var27[32] = {0};             // RAM - stack
                                      
    static int var28 = 0;             // RAM - static
    static unsigned char var29 = 0;   // RAM - static
    static char var30[32] = {0};      // RAM - static
                                      
    volatile int var31 = 0;           // RAM - stack
    volatile unsigned char var32 = 0; // RAM - stack
    volatile char var33[32] = {0};    // RAM - stack
                                      
    const int var34 = 0;              // ROM
    const unsigned char var35 = 0;    // ROM
    const char var36[32] = {0};       // ROM
    
    var25 = func(1,2);
    
    while(1)
    {}
}
```

`Q2` There is no need to store the LR on the stack, because in the sum() function no other function is called.

`Q3` PC is short for program counter. The purpose of the PC is counting the addresses of the instructions that need are executed. Normally, the PC is automatically incremented, except when a branch or ISR occurs.

`Q4` LR is short for link register. The purpose of the LR is to save the return address, so the core knows where to continue after returning from a function.

`Q5` SP is short for stack pointer. The purpose of the SP is to hold the address of the top value that is stored on the stack.

`Q6` The arguments *a* and *b* are passed through core registers R0 and R1.

`Q7` The value is returned through core register R0.

`Q8` The PUSH instruction is used to save the content of the core registers on the stack. This allows to use the register in a function, because the number of available registers in the CPU core is limited. If the function is completed, the POP instruction is used to restore the value from the stack to the core register.

`Q9` An automatic variable is stored on the stack.

`Q10` It is called automatic, because the memory address of the variable varies (depending on the stack that has been build so far).

`Q11` *Static* in this context means that for this variable the memory address is fixed, not it's value.

`Q12` The heap is one of the RAM memory regions. It is used to allocate memory from explicit allocation to explicit deallocation. It is used with functions such as malloc() and free().

`Q13` The purpose of the type qualifier *const* is to store data in ROM instead of RAM. Most often, RAM is limited, so by adding const to a declaration, the compiler will store the variable in ROM instead of RAM.  This is particularly useful for large data items, such a font tables and images.

`Q14` The size of the static variable region is managed automatically by the compiler. All static declared variables will be located in this region and the compiler will fail if it doesn't fit in the available RAM memory. The stack and heap size are configured in the IDE, for example in the project settings or a scatter (.sct) file.

`Q15` The struct test_t is 40 bytes in size. According the AAPCS, this will be passed on the stack, not in the core registers. So, for func1(), the compiler creates a copy of 40 bytes on the stack and will pass a pointer to this copy as an argument. After the function finishes, the stack is restored and the copy is discarded. This is also known as *call-by-value*.

For func2(), a pointer to the struct is passed. A pointer is a fundamental data type and fits within a single core register. It will therefore be passed in R0, creating no overhead on the stack. Be aware though, that by using a pointer, changes to the struct are reflected in the original struct too! This is also known as *call-by-reference*.
