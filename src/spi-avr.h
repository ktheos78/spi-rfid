#ifndef _SPI_AVR_H
#define _SPI_AVR_H

#include <stdint.h>

#define MOSI 3
#define SCK  5
#define CS   2
#define RST  0

#define CS_LOW()    (PORTB &= ~(1 << CS))
#define CS_HIGH()   (PORTB |= (1 << CS))

#define RST_LOW()   (PORTB &= ~(1 << RST))
#define RST_HIGH()  (PORTB |= (1 << RST))

void spi_master_init(void);
void spi_master_transmit(uint8_t);
uint8_t spi_master_receive(void);

#endif /* _SPI_AVR_H */