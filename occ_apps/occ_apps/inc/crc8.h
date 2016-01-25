#ifndef CRC8_H
#define CRC8_H

#include <string.h>
#include <stdint.h>
	
uint8_t crc8(uint8_t* ptr,uint16_t len);
uint8_t OCC_Use_CRC8_Check_Data(uint8_t *src);
	

#endif
