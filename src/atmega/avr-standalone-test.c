#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/*********
 *  SPI  *
 *********/

#define MOSI 3
#define SCK  5
#define RFID_CS   2
#define RFID_RST  0

#define CS_LOW()    (PORTB &= ~(1 << RFID_CS))
#define CS_HIGH()   (PORTB |= (1 << RFID_CS))

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

/*************
 *  MFRC522  *
 *************/

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

void MFRC522_write_reg(uint8_t reg, uint8_t val)
{
    // select MFRC522
    CS_LOW();

    // transmit register address
    // leftshifted by 1, R/W bit (MSB) set to 0 for write
    (void)spi_transceive((reg << 1) & 0x7E);

    // transmit data
    (void)spi_transceive(val);

    // deselect MFRC522
    CS_HIGH();
}

uint8_t MFRC522_read_reg(uint8_t reg)
{
    uint8_t ret;

    // select MFRC522
    CS_LOW();

    // transmit register address
    // leftshifted by 1, R/W bit (MSB) set to 1 for read
    (void)spi_transceive(((reg << 1) & 0x7E) | 0x80);

    // get read
    ret = spi_transceive(0xFF);

    // deselect MFRC522
    CS_HIGH();

    return ret;
}

void set_bitmask(uint8_t reg, uint8_t mask)
{
    uint8_t temp = MFRC522_read_reg(reg);
    MFRC522_write_reg(reg, temp | mask);
}

void clear_bitmask(uint8_t reg, uint8_t mask)
{
    uint8_t temp = MFRC522_read_reg(reg);
    MFRC522_write_reg(reg, temp & ~(mask));
}

void MFRC522_init(void)
{
    // soft reset
    MFRC522_write_reg(CommandReg, PCD_RESETPHASE);
    _delay_ms(50);

    // init timer
    MFRC522_write_reg(TModeReg, 0x80);
    MFRC522_write_reg(TPrescalerReg, 0xA9);

    // set 25ms timeout
    MFRC522_write_reg(TReloadRegH, 0x03);
    MFRC522_write_reg(TReloadRegL, 0xE8);

    // force 100% ASK modulation
    MFRC522_write_reg(TxASKReg, 0x40);
    
    // set max gain
    MFRC522_write_reg(RFCfgReg, 0x70);

    // antenna on
    set_bitmask(TxControlReg, 0x03);
}


/*
 *  RC522 and ISO14443 card communication
 *  params: command, data to send, and a buffer for the data to be received
 *  return value: 0 -> OK | 1 -> ERR
 *  received data gets placed in in_data buffer
 */
uint8_t MFRC522_to_card(uint8_t cmd, uint8_t *out_data, uint8_t out_len, uint8_t *in_data, uint8_t *in_len)
{
    uint8_t irq_en = 0x00, wait_irq = 0x00, status = STATUS_ERR;
    uint8_t last_bits, n, err;
    uint16_t timer;

    if (cmd == PCD_TRANSCEIVE)
    {
        irq_en = 0x77;
        wait_irq = 0x30;
    }

    MFRC522_write_reg(CommIEnReg, irq_en | 0x80);   // interrupt request
    clear_bitmask(CommIrqReg, 0x80);                // clear all interrupt request bit
    set_bitmask(FIFOLevelReg, 0x80);                // init FIFO

    // cancel current command
    MFRC522_write_reg(CommandReg, PCD_IDLE);

    // write data to FIFO
    for (uint8_t i = 0; i < out_len; ++i)
        MFRC522_write_reg(FIFODataReg, out_data[i]);

    // execute command
    MFRC522_write_reg(CommandReg, cmd);
    if (cmd == PCD_TRANSCEIVE)
        set_bitmask(BitFramingReg, 0x80);          // start transmission

    // wait to receive data to complete (timeout loop)
    timer = 2000;   // in accordance with 25ms timeout
    do
    {
        // CommIrqReg[7:0]
        // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = MFRC522_read_reg(CommIrqReg);
        timer--;
    } while (timer > 0 && !(n & (1 << TimerIRq)) && !(n & wait_irq));

    // StartSend = 0, transmission over
    clear_bitmask(BitFramingReg, 0x80);

    if (timer != 0)
    {
        err = MFRC522_read_reg(ErrorReg);

        // check for BufferOvfl, CollErr, ParityErr, ProtocolErr
        if ((err & 0x1B))
            return STATUS_ERR;
        status = STATUS_OK;

        // check for command specific errors
        if (n & irq_en & 0x01)
            status = STATUS_ERR;

        if (cmd == PCD_TRANSCEIVE)
        {
            n = MFRC522_read_reg(FIFOLevelReg);     // num of bytes received
            last_bits = MFRC522_read_reg(ControlReg) & 0x07;

            *in_len = (last_bits) ? ((n - 1)*8 + last_bits) : (n*8);

            if (n == 0)
                n = 1;
            else if (n > MAX_LEN)
                n = MAX_LEN;

            // read received data in fifo
            for (uint8_t i = 0; i < n; ++i)
                in_data[i] = MFRC522_read_reg(FIFODataReg);
        }
    }

    return status;
}


/*
 *  Find cards, read card type number
 */
uint8_t MFRC522_request(uint8_t *buf_atqa)
{
    uint8_t status, recv_len, cmd_buf[1];

    // TxLastBits = BitFramingReg[2:0] (7-bit frame)
    MFRC522_write_reg(BitFramingReg, 0x07);

    // request command
    cmd_buf[0] = PICC_REQIDL;

    status = MFRC522_to_card(PCD_TRANSCEIVE, cmd_buf, 1, buf_atqa, &recv_len);
    if (status != STATUS_OK || recv_len != 16)
        status = STATUS_ERR;

    return status;
}

/*
 *  Anti-collision, writes UID into parameter pointer
 *  ret: 0 -> OK | 1 -> ERR
 */
uint8_t MFRC522_anticoll(uint8_t *uid)
{
    uint8_t status, len, cmd_buf[2];

    // TxLastBits = 0 (8-bit frame)
    MFRC522_write_reg(BitFramingReg, 0x00);

    cmd_buf[0] = PICC_ANTICOLL;
    cmd_buf[1] = 0x20;  // number of valid bits = 2 bytes sent

    status = MFRC522_to_card(PCD_TRANSCEIVE, cmd_buf, 2, uid, &len);

    return status;
}

int main(void)
{
    uint8_t buf_atqa[2], uid[5];

    // init SPI
    spi_master_init();

    // init MFRC522
    MFRC522_init();

    // leds for debugging
    DDRC = 0xFF;

    // main loop
    while (1)
    {
        // check if card is in the field
        if (MFRC522_request(buf_atqa) == STATUS_OK)
        {
            // if present, read UID
            if (MFRC522_anticoll(uid) == STATUS_OK)
            {
                // tag detected, blink PORTC
                PORTC = 0xFF;
                _delay_ms(500);
                PORTC = 0x00;
                _delay_ms(500);
                PORTC = 0xFF;
                _delay_ms(500);
                PORTC = 0x00;
                _delay_ms(500);
                PORTC = 0xFF;
                _delay_ms(500);
                PORTC = 0x00;
                _delay_ms(500);
            }
        }
    }
}