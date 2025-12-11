#ifndef _LCD_AVR_H
#define _LCD_AVR_H

#include <stdint.h>

// LCD commands
#define CMD_DEC_CURSOR          0x04
#define CMD_INC_CURSOR          0x06
#define CMD_CURSOR_ON           0x0E        
#define CMD_CURSOR_OFF          0x0C
#define CMD_RESET_CURSOR        0x80
#define CMD_CURSOR_SECOND_LINE  0xC0

void write_2_nibbles(uint8_t);
void lcd_data(uint8_t);
void lcd_command(uint8_t);
void lcd_clear_display(void);
void lcd_init(void);
void lcd_print(const char *);

#endif /* _LCD_AVR_H */