# SPI RFID

## About

This is a project implementing contactless Near Field Communication (NFC) and RFID between an ARM/AVR MCU and an MFRC522 RFID Module. More specifically, when the "correct" NFC tag (whose unique identifier is hardcoded into the project) is scanned, two LEDs (`PA{0,1}` on the STM32G474RE, `PD{0,1}` on the Atmega328PB) stay on for 5 seconds; otherwise, if a different NFC tag is scanned, these same LEDs blink repeatedly for 5 seconds. This simulates the functionality of an an RFID lock. 

The project has two major building blocks: the implementation of SPI communication on the aforementioned ARM/AVR boards, as well as the implementation of a multi-platform driver for the MFRC522 RFID module.

## Implementation

### SPI Communication - STM32G474RE



### SPI Communication - Atmega328PB



### MFRC522 RFID Module Driver



### Main Project - RFID Lock



## Additional Notes