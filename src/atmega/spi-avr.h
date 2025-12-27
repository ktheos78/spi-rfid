#ifndef _SPI_AVR_H
#define _SPI_AVR_H

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdint.h>

#define MOSI 3
#define SCK  5
#define RFID_CS   2
#define RFID_RST  0

#define CS_LOW()    (PORTB &= ~(1 << RFID_CS))
#define CS_HIGH()   (PORTB |= (1 << RFID_CS))

void spi_master_init(void);
static uint8_t spi_transceive(uint8_t);

#endif /* _SPI_AVR_H */