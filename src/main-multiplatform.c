/*  
 * proof of concept for multiplatform project
 */

#if defined(__AVR__)
    #define MCU_ATMEL 1
    #define MCU_STM32 0
#elif defined(STM32G474xx)
    #define MCU_ATMEL 0
    #define MCU_STM32 1
#else
    #error "Unknown MCU"
#endif

#include <stdint.h>
#include "MFRC522.h"

/* check if scanned UID matches (hardcoded) tag UID */
int validate (uint8_t *uid)
{
	uint8_t tag_uid[] = {0xC3, 0xB1, 0xE6, 0x1B, 0x8F};

	for (uint8_t i = 0; i < 5; ++i)
	{
		if (uid[i] != tag_uid[i])
			return 0;
	}

	return 1;
}

int main(void)
{
    uint8_t buf_atqa[2], uid[5];

    if (MCU_ATMEL)
    {
        /* AVR init code */

        // set PD{0,1} as output
        DDRD = 0x03;

        // init SPI
        spi_master_init();

        // AVR main loop
        while (1)
        {
            // check if card is in the field
            if (MFRC522_request(buf_atqa) == STATUS_OK)
            {
                // if present, read UID
                if (MFRC522_anticoll(uid) == STATUS_OK)
                {
                    // if tag matches hardcoded UID, keep PD{0,1} on for 5 sec, else blink them for 5s
                    if (validate(uid))
                    {
                        PORTD |= (1 << PD0) | (1 << PD1);
                        _delay_ms(5000);
                        PORTD &= ~((1 << PD0) | (1 << PD1));
                    }

                    else
                    {
                        for (uint8_t i = 0; i < 5; ++i)
                        {
                            PORTD |= (1 << PD0) | (1 << PD1);
                            _delay_ms(500);
                            PORTD &= ~((1 << PD0) | (1 << PD1));
                            _delay_ms(500);
                        }
                    }
                }
            }
        }
    }

    else if (MCU_STM32)
    {
        /* ARM init code */

        // ARM main loop
        while (1)
        {
            
        }
    }   

    // invalid MCU
    else
        while(1);
}