#ifndef _I2C_AVR_H
#define _I2C_AVR_H

#include <stdint.h>


/*********
 *  TWI  *
 *********/
 
#define TWI_READ        1           // read from TWI device
#define TWI_WRITE       0           // write to TWI device
#define SCL_CLK         100000L     // TWI clock in Hz

// F_SCL = F_CPU/(16 + 2*TWBRO_VAL*PRESCALER)
#define TWBR0_VAL       ((F_CPU/SCL_CLK) - 16)/2

// status = TWSR0[7:3]
#define TW_STATUS (TWSR0 & 0xF8)

// master transmitter/receiver
#define TW_START        0x08
#define TW_REP_START    0x10

// master transmitter
#define TW_MT_SLA_ACK   0x18
#define TW_MT_SLA_NACK  0x20
#define TW_MT_DATA_ACK  0x28
#define TW_MT_DATA_NACK 0x30

// master receiver
#define TW_MR_SLA_ACK   0x40
#define TW_MR_SLA_NACK  0x48
#define TW_MR_DATA_ACK  0x50
#define TW_MR_DATA_NACK 0x58

void twi_init(void);
uint8_t twi_read_ack(void);
uint8_t twi_read_nack(void);
uint8_t twi_start(uint8_t address);
void twi_start_wait(uint8_t address);
uint8_t twi_write(uint8_t data);
uint8_t twi_rep_start(uint8_t address);
void twi_stop(void);


/*************
 *  PCA9555  *
 *************/

// A0 = A1 = A2 = 0 by hardware
#define PCA9555_0_ADDR  0x40

// registers
typedef enum {
    REG_INPUT_0,
    REG_INPUT_1,
    REG_OUTPUT_0,
    REG_OUTPUT_1,
    REG_POLARITY_INV_0,
    REG_POLARITY_INV_1,
    REG_CONFIGURATON_0,
    REG_CONFIGURATON_1
} PCA9555_REG;

void PCA9555_0_write(PCA9555_REG reg, uint8_t value);
uint8_t PCA9555_0_read(PCA9555_REG reg);

#endif /* _I2C_AVR_H */