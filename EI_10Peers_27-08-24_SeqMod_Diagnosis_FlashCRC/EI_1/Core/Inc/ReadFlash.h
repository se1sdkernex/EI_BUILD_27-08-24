/*
 * ReadFlash.h
 *
 *  Created on: 18-Jun-2024
 *      Author: ravit
 */

#ifndef INC_READFLASH_H_
#define INC_READFLASH_H_


#include "Headers.h"


#define FLASH_START_ADDRESS 	0x08000000  // Example start address
//#define FLASH_SIZE_CRC_ADDRESS 	0x08033FE0  // CRC AND SIZE OF DATA RECORD Stroing address
#define FLASH_SIZE_CRC_ADDRESS 	0x081CFFE0
#define FLASH_TOTAL_SIZE    	0x00020000  // Example total size (128 KB)



extern uint32_t FlashDataSize;
extern uint16_t FlashCrc;
extern uint16_t HexFileCrc;


void find_flash_used_size(void);
uint16_t crc16_arc(uint16_t crc, const uint8_t *buf, size_t len);
void CheckCRConStartup(void);
uint8_t CheckCRConRunTime(void);

#endif /* INC_READFLASH_H_ */
