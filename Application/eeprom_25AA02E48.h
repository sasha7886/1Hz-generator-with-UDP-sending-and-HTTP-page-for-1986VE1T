#ifndef __EEPROM_25AA02E48_H
#define __EEPROM_25AA02E48_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define READ_INSTR					0x03 // иструкция чтения для 25AA02E48
#define EUI_48_START_ADDR		0xFA // начальный адрес блока EUI для 25AA02E48
#define EUI_48_SIZE					6 // размер блока EUI для 25AA02E48

uint8_t EUI_25AA02E48_Read(uint8_t *eui);
uint8_t EUI_25AA02E48_Init(uint8_t *eui);

#ifdef __cplusplus
}
#endif	
	
#endif
