/*! ***************************************************************************
 *
 * \brief     LCD functions
 * \file      lcd.c
 * \author    Hugo Arends
 * \date      December 2024
 *
 *                  MCXA153                 1602 LCD I2C
 *            ------------------+         +--------------+
 *                              |         |
 *              P3_27/LPI2C0_SCL+---------+SCL
 *              P3_28/LPI2C0_SDA+---------+SDA
 *                              |         |
 *                           5V0+---------+VCC
 *                           GND+---------+GND
 *                              |         |
 *            ------------------+         +--------------+
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
#include "lcd.h"
#include "delay.h"
#include "lpi2c0_controller_interrupt.h"

// -----------------------------------------------------------------------------
// Local type definitions
// -----------------------------------------------------------------------------
#define I2C_ADDRESS (0x27)

typedef enum
{
    cmd = 0,
    dat = 1,
}rs_t;

// -----------------------------------------------------------------------------
// Local function prototypes
// -----------------------------------------------------------------------------
void write_4bits(const rs_t rs, const uint8_t data);
void write_8bits(const rs_t rs, const uint8_t data);

// -----------------------------------------------------------------------------
// Local variables
// -----------------------------------------------------------------------------
static uint8_t out = 0;

// -----------------------------------------------------------------------------
// Local function implementation
// -----------------------------------------------------------------------------

/*!
 * \brief    Initialize the LCD
 *
 * \note     This function initializes the LCD. It initializes LPI2C0, sets the
 *           LCD in 4-bit mode and clears the display.
 */
void lcd_init(void)
{
    lpi2c0_controller_init();

    // Startup delay
    delay_ms(100);

    // Clear all pins
    //  7  6  5  4  3  2  1  0
    // D7 D6 D5 D4 BL  E RW RS
    out = 0b00000000;
    lpi2c0_write(I2C_ADDRESS, &out, 1);

    // Start the initialization sequence
    write_4bits(cmd, 0x3);
    delay_ms(1);
    write_4bits(cmd, 0x3);
    delay_ms(1);
    write_4bits(cmd, 0x3);
    write_4bits(cmd, 0x2);
    write_8bits(cmd, 0x28);
    write_8bits(cmd, 0x0C);
    write_8bits(cmd, 0x06);
    write_8bits(cmd, 0x80);

    // Clear the LCD
    lcd_clear();
    lcd_return_home();
}

bool lcd_busy(void)
{
    //  7  6  5  4  3  2  1  0
    // D7 D6 D5 D4 BL  E RW RS

    // D7-D4: Clear
    // BL:    Unchanged
    // E:     Clear
    // RW:    Clear
    // RS:    Clear
    out &= ~0b11110111;

    // D7-D4: Set (set pins to input)
    // RW:    Set
    out |= 0b11110010;
    lpi2c0_write(I2C_ADDRESS, &out, 1);

    // E:     Set
    out |= 0b00000100;
    lpi2c0_write(I2C_ADDRESS, &out, 1);

    // Read the status MSB
    uint8_t status_msb = 0;
    lpi2c0_read(I2C_ADDRESS, &status_msb, 1);
    status_msb &= ~0xF0;

    while(lpi2c0_busy())
    {}

    // E:     Clear
    out &= ~0b00000100;
    lpi2c0_write(I2C_ADDRESS, &out, 1);

    // E:     Set
    out |= 0b00000100;
    lpi2c0_write(I2C_ADDRESS, &out, 1);

    // Read the status LSB
    uint8_t status_lsb = 0;
    lpi2c0_read(I2C_ADDRESS, &status_lsb, 1);
    status_lsb >>= 4;

    while(lpi2c0_busy())
    {}

    return (status_msb & 0x80) != 0;
}

/*!
 * \brief    Set the backlight of the LCD
 *
 * \param    bl    The backlight level
 *
 * \note     This function controls the backlight of the LCD. The backlight can
 *           be set to a value of bl_t.
 */
void lcd_backlight(const bl_t bl)
{
    //  7  6  5  4  3  2  1  0
    // D7 D6 D5 D4 BL  E RW RS

    // D7-D4: Unchanged
    // BL:    Clear
    // E:     Unchanged
    // RW:    CleUnchangedar
    // RS:    Unchanged
    out &= ~(1<<3);

    // BL:    bl
    out |= (bl<<3);

    lpi2c0_write(I2C_ADDRESS, &out, 1);

    while(lpi2c0_busy())
    {}
}

/*!
 * \brief    Clear the LCD
 *
 * \note     Clears entire display and sets DDRAM address 0 in address counter.
 */
void lcd_clear(void)
{
    write_8bits(cmd, 0x01);

    // Wait for the LCD to finish
    while(lcd_busy())
    {}
}

/*!
 * \brief    Return the display to the home position
 *
 * \note     Sets DDRAM address 0 in address counter. Also returns display from
 *           being shifted to original position. DDRAM contents remain
 *           unchanged.
 */
void lcd_return_home(void)
{
    // Write the command
    write_8bits(cmd, 0x02);

    // Wait for the LCD to finish
    while(lcd_busy())
    {}
}

/*!
 * \brief    Set the cursor of the LCD
 *
 * \param    col    The column, a value from  0 to 15
 * \param    row    The row, a value from 0 to 1
 *
 * \note     This function sets the cursor of the LCD.
 */
void lcd_set_cursor(const uint8_t col, const uint8_t row)
{
    uint8_t address;

    // Calculate the address
    address = (row * 0x40) + col;
    address |= 0x80;

    // Set the address
    write_8bits(cmd, address);
}

/*!
 * \brief    Put a character on the LCD
 *
 * \param    c    The character
 *
 * \note     This function puts a character on the LCD at the current
 *           cursor position.
 */
void lcd_put(const char *str)
{
    while(*str)
    {
        write_8bits(dat, *str++);
    }
}

/*!
 * \brief    Write 4 bits to the LCD
 *
 * \param    rs    The register select
 * \param    data  The data
 *
 * \note     This function writes the 4 LSB bits of \p data to the LCD.
 */
void write_4bits(const rs_t rs, const uint8_t data)
{
    //  7  6  5  4  3  2  1  0
    // D7 D6 D5 D4 BL  E RW RS

    // D7-D4: Clear
    // BL:    Unchanged
    // E:     Clear
    // RW:    Clear
    // RS:    Clear
    out &= ~0b11110111;

    // D7-D4: data[3:0]
    // E:     Set
    // RS:    rs
    out |= (data << 4) | (1<<2) | (rs<<0);
    lpi2c0_write(I2C_ADDRESS, &out, 1);

    // E:     Clear
    out &= ~0b00000100;
    lpi2c0_write(I2C_ADDRESS, &out, 1);
}

/*!
 * \brief    Write 8 bits to the LCD
 *
 * \param    rs    The register select
 * \param    data  The data
 *
 * \note     This function writes the 8 bits in \p data to the LCD.
 */
void write_8bits(const rs_t rs, const uint8_t data)
{
    write_4bits(rs, data >> 4);
    write_4bits(rs, data);
}
