/*  
 * proof of concept for multiplatform project
 */
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

    #if defined(__AVR__)

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

    #elif defined(STM32G474xx)

        /* ARM init code */

        /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	    HAL_Init();

	    /* Configure the system clock */
	    SystemClock_Config();

        /* Initialize all configured peripherals */
        MX_GPIO_Init();
        MX_SPI2_Init();
        MFRC522_init();

        // init PORTA as off
	    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_4 |
							LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7);

        // ARM main loop
        while (1)
        {
            // check if card is in the field
            if (MFRC522_request(buf_atqa) == STATUS_OK)
            {
                // if present, read UID
                if (MFRC522_anticoll(uid) == STATUS_OK)
                {

                    // if tag matches hardcoded UID, keep PA{0,1} on for 5 sec, else blink them for 5s
                    if (validate(uid))
                    {
                        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_1);
                        LL_mDelay(5000);
                        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_1);
                    }

                    else
                    {
                        for (uint8_t i = 0; i < 5; ++i)
                        {
                            LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_1);
                            LL_mDelay(500);
                            LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_1);
                            LL_mDelay(500);
                        }
                    }
                }
            }
        }

    
    #else
        #error "Unknown MCU"
    #endif
}