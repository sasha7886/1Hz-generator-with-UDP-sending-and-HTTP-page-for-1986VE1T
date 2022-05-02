#ifndef __EEPROM_NEW_H
#define __EEPROM_NEW_H
#include <stdint.h>

uint8_t eeprom_read(uint8_t *eui);
static uint8_t eephrom_read_byte (uint8_t addr);
uint8_t eeprom_new_init(uint8_t *eui);
void Delay (uint16_t ticks);
void SendByte (uint8_t byte);
uint8_t ReadByteSPI (void);


#endif