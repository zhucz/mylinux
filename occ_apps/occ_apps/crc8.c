#include "./inc/crc8.h"

uint8_t crc8(uint8_t* ptr,uint16_t len)
{
  uint8_t i = 0;
  uint8_t crc = 0;
  while(len--)
  {
    i = 0x80;
	while(i != 0)
	{
      if(((crc & 0x80) != 0) && (((*ptr) & i) != 0))//CRC最高位为1且校验bit为1
	  {
        crc <<= 1;
	  }
	  else if(((crc & 0x80)!= 0) && (((*ptr) & i)==0))//CRC最高位为1且校验bit为0
	  {
	     crc <<= 1;
		 crc ^= 0x31;
	  }
	  else if(((crc & 0x80) == 0)&& (((*ptr) & i) !=0))//CRC最高位为0且校验bit为1
	  {
         crc <<= 1;
		 crc ^= 0x31;
	  }
	  else if(((crc & 0x80) == 0) &&(((*ptr)&i)== 0))//CRC最高位为0且校验bit为0
	  {
	     crc <<= 1;
	  }
	  i >>= 1;
	}
	ptr++;
  }
  return(crc);
}

uint8_t OCC_Use_CRC8_Check_Data(uint8_t *src)
{
	uint16_t calced_len = 0;
	uint8_t  calced_crc = 0;

	if(src == NULL) return 0;
	calced_len = ((*(src + 1) << 8)| (*(src + 2)));

	if(calced_len > 2370){
		return 0;
	}else{
		calced_crc = crc8(src,calced_len);
		if(calced_crc == *(src + calced_len)){
			return 1;
		}else{
			return 0;
		}
	}
}






#if 0
#include <string.h>


uint8_t crc8(uint8_t *ptr,uint16_t len)
{
	uint8_t i = 0;
	uint8_t crc = 0;
	while(len--)
	{
		i = 0x80;
		while(i != 0)
		{
			if(((crc & 0x80) != 0) && (((*ptr) & i) != 0))
			{
				crc << 1;
			}
			if(((crc & 0x80) != 0) && (((*ptr) & i) == 0))
			{
				crc << 1;
				crc ^= 0x31;
			}
			if(((crc & 0x80) == 0) && (((*ptr) & i) != 0))
			{
				crc << 1;
				crc ^= 0x31;
			}
			if(((crc & 0x80) == 0) && (((*ptr) & i) == 0))
			{
				crc << 1;
			}
			i >> 1;
		}
		ptr++;
	}
	return(crc);
}
#endif

