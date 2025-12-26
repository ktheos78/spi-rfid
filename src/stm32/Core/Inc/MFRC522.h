#ifndef __MFRC522_H
#define __MFRC522_H

#include <stdint.h>

#include "spi.h"

#define CS_LOW() LL_GPIO_ResetOutputPin(RFID_CS_GPIO_Port, RFID_CS_Pin);
#define CS_HIGH() LL_GPIO_SetOutputPin(RFID_CS_GPIO_Port, RFID_CS_Pin);

// commands
#define PCD_IDLE        0x00
#define PCD_TRANSCEIVE  0x0C
#define PCD_RESETPHASE  0x0F

// card command words
#define PICC_REQIDL     0x26
#define PICC_ANTICOLL   0x93

// registers
#define CommandReg      0x01
#define CommIEnReg      0x02
#define CommIrqReg     0x04
#define ErrorReg        0x06
#define FIFODataReg     0x09
#define FIFOLevelReg    0x0A
#define ControlReg      0x0C
#define BitFramingReg   0x0D
#define TxControlReg    0x14
#define TxASKReg        0x15
#define RFCfgReg        0x26
#define TModeReg        0x2A
#define TPrescalerReg   0x2B
#define TReloadRegH     0x2C
#define TReloadRegL     0x2D

// status macros
#define STATUS_OK       0
#define STATUS_ERR      1

// bitmasks
#define TimerIRq        1

// maximum array length
#define MAX_LEN         16

// core MFRC522 functions
void MFRC522_init(void);
void MFRC522_write_reg(uint8_t, uint8_t);
uint8_t MFRC522_read_reg(uint8_t);
uint8_t MFRC522_to_card(uint8_t, uint8_t *, uint8_t, uint8_t *, uint8_t *);
uint8_t MFRC522_request(uint8_t *);
uint8_t MFRC522_anticoll(uint8_t *);

// helpers
void set_bitmask(uint8_t, uint8_t);
void clear_bitmask(uint8_t, uint8_t);

#endif /* __MFRC522_H */
