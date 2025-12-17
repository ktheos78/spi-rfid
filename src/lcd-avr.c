/* initialization/driver functions for LCD, using PORTD */
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
    temp = (PIND & 0x0F) | (data & 0xF0);    // keep lower 4 bits of PIND, upper 4 bits of lcd_data
    PORTD = temp;

    // enable pulse
    PORTD |= (1 << PD3);
    __asm volatile ("nop");
    __asm volatile ("nop");
    PORTD &= ~(1 << PD3);

    // send low nibble
    data <<= 4;
    temp = (PIND & 0x0F) | (data & 0xF0);    // keep lower 4 bits of PIND, lower 4 bits of lcd_data shifted to upper nibble
    PORTD = temp;

    // enable pulse
    PORTD |= (1 << PD3);
    __asm volatile ("nop");
    __asm volatile ("nop");
    PORTD &= ~(1 << PD3);
}

void lcd_data(uint8_t data)
{
    PORTD |= (1 << PD2);    // LCD_RS = 1 (PD2 = 1) -> data
    write_2_nibbles(data);
    _delay_ms(5);
}

void lcd_command(uint8_t cmd)
{
    PORTD &= ~(1 << PD2);   // LCD_RS = 0 (PD2 = 0) -> instruction
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
    _delay_ms(200);

    // send cmd to switch to 8-bit mode 3 times
    for (uint8_t i = 0; i < 3; ++i)
    {
        // switch to 8-bit mode
        PORTD = 0x30;       
        
        // enable pulse
        PORTD |= (1 << PD3);
        __asm volatile ("nop");
        __asm volatile ("nop");
        PORTD &= ~(1 << PD3);
        _delay_us(250);
    }

    // switch to 4-bit mode
    PORTD = 0x20;
    PORTD |= (1 << PD3);    // enable pulse
    __asm volatile ("nop");
    __asm volatile ("nop");
    PORTD &= ~(1 << PD3);
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