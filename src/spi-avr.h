#ifndef _SPI_AVR_H
#define _SPI_AVR_H

#include <stdint.h>

#define MOSI 3
#define SCK  5
#define CS   2
#define RST  0

#define CS_LOW()    (PORTB &= ~(1 << CS))
#define CS_HIGH()   (PORTB |= (1 << CS))

void spi_master_init(void);
static uint8_t spi_transceive(uint8_t);

#endif /* _SPI_AVR_H */