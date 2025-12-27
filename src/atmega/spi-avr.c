#include "spi-avr.h"

void spi_master_init(void)
{
    // set MOSI, CS and SCK output, rest input
    DDRB |= (1 << MOSI) | (1 << SCK) | (1 << RFID_CS);

    // enable SPI, master, MSB first, set clock rate fclk/32
    SPCR0 = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
    SPSR0 = (1 << SPI2X);

    // set mode 0 (CPH = CPL = 0)
    SPCR0 &= ~((1 << CPHA) | (1 << CPOL));
}

static uint8_t spi_transceive(uint8_t data)
{
    // start transmission
    SPDR0 = data;

    // wait for transmission complete
    while (!(SPSR0 & (1 << SPIF)));

    // return cached data
    return SPDR0;
}