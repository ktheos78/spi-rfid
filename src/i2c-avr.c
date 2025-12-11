#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "i2c-avr.h"

/*********
 *  TWI  *
 ********/

// initialize TWI clock
void twi_init(void)
{
    TWSR0 = 0;          // prescaler = 1
    TWBR0 = TWBR0_VAL;  // SCL_CLK = 100kHz
}

// read 1 byte from TWI device (request more data)
uint8_t twi_read_ack(void)
{
    // TWEA: ensures ACK is created on channel
    TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    // poll TWINT
    while (!(TWCR0 & (1 << TWINT)));

    return TWDR0;
}

// read 1 byte from TWI device, read is followed by STOP condition
uint8_t twi_read_nak(void)
{
    TWCR0 = (1 << TWINT) | (1 << TWEN);

    // poll TWINT
    while (!(TWCR0 & (1 << TWINT)));

    return TWDR0;
}

// issues a start condition and sends address and transfer direction
// ret: 0 = device accessible, 1 = failed to access device
uint8_t twi_start(uint8_t address)
{
    uint8_t twi_status;

    // send START condition
    TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // wait until transmission completed
    while (!(TWCR0 & (1 << TWINT)));

    // check status
    twi_status = TW_STATUS;
    if ((twi_status != TW_START) && (twi_status != TW_REP_START))
        return 1;

    // send device address
    TWDR0 = address;
    TWCR0 = (1 << TWINT) | (1 << TWEN);

    // wait until transmission completed and ACK/NACK received
    while (!(TWCR0 & (1 << TWINT)));

    // check status
    twi_status = TW_STATUS;
    if ((twi_status != TW_MT_SLA_ACK) && (twi_status != TW_MR_SLA_ACK))
        return 1;

    return 0;
} 

// send START condition, address, transfer direction
// use ACK polling to wait until device is ready
void twi_start_wait(uint8_t address)
{
    uint8_t twi_status;

    while (1)
    {
        // send START condition
        TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

        // wait until transmission completed
        while (!(TWCR0 & (1 << TWINT)));

        // check status
        twi_status = TW_STATUS;
        if ((twi_status != TW_START) && (twi_status != TW_REP_START))
            continue;

        // send device address
        TWDR0 = address;
        TWCR0 = (1 << TWINT) | (1 << TWEN);

        // wait until transmission completed
        while (!(TWCR0 & (1 << TWINT)));

        // check status
        twi_status = TW_STATUS;
        if ((twi_status == TW_MT_SLA_NACK) || (twi_status == TW_MR_DATA_NACK))
        {
            // device busy, send STOP condition to terminate write (TWSTO)
            TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

            // wait until STOP is executed and bus is released
            while (TWCR0 & (1 << TWSTO));

            continue;
        }

        break;
    }
}

// send one byte to TWI device
// ret: 0 = write successful, 1 = write failed
uint8_t twi_write(uint8_t data)
{
    // send data to previously addressed device
    TWDR0 = data;
    TWCR0 = (1 << TWINT) | (1 << TWEN);

    // wait until transmission completed
    while (!(TWCR0 & (1 << TWINT)));

    // check status for ACK
    if (TW_STATUS != TW_MT_DATA_ACK)
        return 1;

    return 0;
}

// send repeated start condition, address, transfer direction
// ret: 0 = device accessible, 1 = failed to access device
uint8_t twi_rep_start(uint8_t address)
{
    return twi_start(address);
}

// terminates the data transfer and releases the TWI bus
void twi_stop(void)
{
    // send STOP condition
    TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

    // wait until STOP is executed
    while (TWCR0 & (1 << TWSTO));
}


/*************
 *  PCA9555  *
 *************/

void PCA9555_0_write(PCA9555_REG reg, uint8_t value)
{
    twi_start_wait(PCA9555_0_ADDR + TWI_WRITE);
    twi_write(reg);
    twi_write(value);
    twi_stop();
}

uint8_t PCA9555_0_read(PCA9555_REG reg)
{
    uint8_t ret;

    twi_start_wait(PCA9555_0_ADDR + TWI_WRITE);
    twi_write(reg);
    (void)twi_rep_start(PCA9555_0_ADDR + TWI_READ);
    ret = twi_read_nak();
    twi_stop();

    return ret;
}

