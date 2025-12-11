/* initialization/driver functions for LCD, using PCA9555 port expander */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "i2c-avr.h"
#include "lcd-avr.h"

void write_2_nibbles(uint8_t data)
{
    uint8_t temp;

    // send high nibble
    temp = PCA9555_0_read(REG_OUTPUT_0);    // read previous value
    temp = (temp & 0x0F) | (data & 0xF0);   // keep lower 4 bits of IO0, upper 4 bits of lcd_data
    PCA9555_0_write(REG_OUTPUT_0, temp);

    // enable pulse
    temp |= (1 << IO0_3);
    PCA9555_0_write(REG_OUTPUT_0, temp);
    temp &= ~(1 << IO0_3);
    PCA9555_0_write(REG_OUTPUT_0, temp);

    // send low nibble
    data <<= 4;
    temp = PCA9555_0_read(REG_OUTPUT_0);
    temp = (temp & 0x0F) | (data & 0xF0);   // keep lower 4 bits of IO0, lower 4 bits of lcd_data

    // enable pulse
    temp |= (1 << IO0_3);
    PCA9555_0_write(REG_OUTPUT_0, temp);
    temp &= ~(1 << IO0_3);
    PCA9555_0_write(REG_OUTPUT_0, temp);
}

void lcd_data(uint8_t data)
{
    uint8_t temp;
    
    temp = PCA9555_0_read(REG_OUTPUT_0);
    temp |= (1 << IO0_2);                   // LCD_RS = 1 -> data
    PCA9555_0_write(REG_OUTPUT_0, temp);
    write_2_nibbles(data);
    _delay_ms(5);
}

void lcd_command(uint8_t cmd)
{
    uint8_t temp;

    temp = PCA9555_0_read(REG_OUTPUT_0);
    temp &= ~(1 << IO0_2);                  // LCD_RS = 0 -> instruction
    PCA9555_0_write(REG_OUTPUT_0, temp);
    write_2_nibbles(cmd);
    _delay_ms(5);
}

void lcd_clear_display(void)
{
    lcd_command(0x01);
    _delay_ms(5);
}

void lcd_init(void)
{
    uint8_t temp;

    _delay_ms(200);

    // send cmd to switch to 8-bit mode 3 times
    for (uint8_t i = 0; i < 3; ++i)
    {
        // switch to 8-bit mode
        temp = 0x30;
        PCA9555_0_write(REG_OUTPUT_0, temp);

        // enable pulse
        temp |= (1 << IO0_3);
        PCA9555_0_write(REG_OUTPUT_0, temp);
        temp &= ~(1 << IO0_3);
        PCA9555_0_write(REG_OUTPUT_0, temp);
        _delay_us(250);
    }

    // switch to 4-bit mode
    temp = 0x20;
    PCA9555_0_write(REG_OUTPUT_0, temp);
    temp |= (1 << IO0_3);                   // enable pulse
    PCA9555_0_write(REG_OUTPUT_0, temp);
    temp &= ~(1 << IO0_3);
    PCA9555_0_write(REG_OUTPUT_0, temp);
    _delay_us(250);

    // set 4-bit mode, 2 lines, 5x8 dots
    lcd_command(0x28);

    // display on, cursor off
    lcd_command(0x0C);
    lcd_clear_display();

    // increase address, no display shift
    lcd_command(0x06);
}

void lcd_print(const char *str)
{
    while (*str)
    {
        lcd_data(*str);
        str++;
    }
}