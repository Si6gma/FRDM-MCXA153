/*! ***************************************************************************
 *
 * \brief     Main application
 * \file      main.c
 * \author    Hugo Arends
 * \date      February 2024
 *
 * \see       NXP. (2024). MCX A153, A152, A143, A142 Reference Manual. Rev. 4,
 *            01/2024. From:
 *            https://www.nxp.com/docs/en/reference-manual/MCXAP64M96FS3RM.pdf
 *
 * \copyright 2024 HAN University of Applied Sciences. All Rights Reserved.
 *            \n\n
 *            Permission is hereby granted, free of charge, to any person
 *            obtaining a copy of this software and associated documentation
 *            files (the "Software"), to deal in the Software without
 *            restriction, including without limitation the rights to use,
 *            copy, modify, merge, publish, distribute, sublicense, and/or sell
 *            copies of the Software, and to permit persons to whom the
 *            Software is furnished to do so, subject to the following
 *            conditions:
 *            \n\n
 *            The above copyright notice and this permission notice shall be
 *            included in all copies or substantial portions of the Software.
 *            \n\n
 *            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *            EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *            OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *            NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *            HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *            WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *            FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *            OTHER DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#include <MCXA153.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
typedef struct
{
    uint32_t level;
    char name[32];
} test_t;

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------
int32_t sum(int32_t a, int32_t b);
extern int32_t multiply(int32_t a, int32_t b);

void func1(test_t test);
void func2(test_t *test);

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Main application
// -----------------------------------------------------------------------------
int main(void)
{
    int32_t s = 0;
    int32_t m = 0;
    test_t test = {0};

    s = sum(1, 2);
    s = sum(8, 8);
    s = sum(8, 0);

    m = multiply(1, 2);
    m = multiply(8, 8);
    m = multiply(8, 0);

    func1(test);
    func2(&test);

    while(1)
    {}
}

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------
int32_t sum(int32_t a, int32_t b)
{
    return a + b;
}

void func1(test_t test)
{
    test.level = 1;
    strcpy(test.name, "func1");
}

void func2(test_t *test)
{
    test->level = 2;
    strcpy(test->name, "func2");
}
