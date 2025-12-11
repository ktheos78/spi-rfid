#if defined(__AVR__)

    /* AVR headers */
    #define F_CPU 16000000UL
    #include <avr/io.h>
    #include <util/delay.h>
    #include <stdint.h>

    #include "i2c-avr.h"
    #include "spi-avr.h"
    #include "lcd-avr.h"
    #include "MFRC522.h"

    #define MCU_ATMEL 1
    #define MCU_STM32 0

#elif defined(STM32G474xx)

    /* ARM headers */
    #include <stdint.h>
    #include "i2c-arm.h"
    #include "spi-arm.h"
    #include "lcd-arm.h"
    #include "MFRC522.h"

    #define MCU_ATMEL 0
    #define MCU_STM32 1

#else
    #error "Unknown MCU"
#endif

int main(void)
{
    if (MCU_ATMEL)
    {
        /* AVR init code */

        // AVR main loop
        while (1)
        {
            
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
}