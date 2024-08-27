/*
 * ReadFlash.c
 *
 *  Created on: 18-Jun-2024
 *      Author: ravit
 */


#include "Headers.h"



uint32_t FlashDataSize = 0;
uint16_t FlashCrc = 0;
uint8_t guc_Completed_FlashCRC_Flag = 0;
uint16_t HexFileCrc=0;


void find_flash_used_size(void)
{
	uint32_t *flash_ptr = (uint32_t *)FLASH_SIZE_CRC_ADDRESS;
	uint8_t Size[4] = {0,};
	uint8_t CRC_Buff[4] = {0,};

	memcpy(&Size,&flash_ptr[0],sizeof(uint32_t));
	memcpy(&CRC_Buff,&flash_ptr[1],sizeof(uint32_t));

	FlashDataSize = (uint32_t)ExtractBits(0, 32, Size);
	FlashCrc = (uint16_t)ExtractBits(0, 16, CRC_Buff);

	printf("\r\nFlash Size:%lu Bytes\n",FlashDataSize);
	printf("CRC       :0X%04X\n",FlashCrc);
}



void CheckCRConStartup(void)
{


	static unsigned long  sUl_Count =0;
	unsigned short int us_IncVar = 0;
	unsigned char uc_Buf[20] ="\0";
	unsigned short int us_CRCRet =0;


	find_flash_used_size();

	//	printf("\n DATA FOR CRC\n");

	for(sUl_Count=0;sUl_Count<FlashDataSize;sUl_Count++)
	{
		uc_Buf[us_IncVar] = (*(volatile unsigned char *)(FLASH_START_ADDRESS+sUl_Count));
		us_IncVar++;

		if(0 == (us_IncVar %16))
		{
			us_CRCRet = crc16_arc(us_CRCRet,uc_Buf,us_IncVar);
			us_IncVar=0;
		}
	}
	//	printf("\nloop cont :%ld\n",sUl_Count);

	if(us_IncVar != 0)
	{
		us_CRCRet = crc16_arc(us_CRCRet,uc_Buf,us_IncVar);
		us_IncVar=0;
	}

	if(us_CRCRet != FlashCrc)
	{
		for(uint16_t i=0;i<200;++i)
		{
			printf("$");
		}
		printf("\n");
		printf("CRC FAIL FROM THE STM32 FLASH\n");
		printf("Calc:0X%04X\n",us_CRCRet);
		printf("FlashCrc:0X%04X\n",FlashCrc);
		HexFileCrc  = FlashCrc;
	}
	else
	{
		printf("\r\nCRC OK From STM32 FLASH with CRC:0X%04X\n",FlashCrc);
		HexFileCrc = FlashCrc;
		printf("Build_CRC:0x%04x\n",HexFileCrc);
	}
}



uint8_t CheckCRConRunTime(void)
{
	static unsigned long  sUl_Count =0;
	unsigned short int us_IncVar = 0;
	unsigned char uc_Buf[100] ="\0";

	static unsigned short int j = 0;
	static unsigned short int us_CRCRet = 0;
	unsigned short int usiNoOfBytes = 992,us_FlashDataFailed=1;	 //992

	for(; sUl_Count < FlashDataSize; sUl_Count++)
	{
		uc_Buf[j] = (*(volatile unsigned char *)(FLASH_START_ADDRESS+sUl_Count));
		j++;

		if(0 == (j % 16))
		{
			//printf("\n Before CRC : %d",usiTempCRC);
			us_CRCRet = crc16_arc(us_CRCRet, uc_Buf, j);
			j=0;
			usiNoOfBytes = usiNoOfBytes - 16;
		}

		if(0==usiNoOfBytes)
		{
			sUl_Count++;
			return (us_FlashDataFailed);
		}
	}

	if(j != 0)
	{
		//printf("\nJJJJJJ....%d",j);
		us_CRCRet = crc16_arc(us_CRCRet, uc_Buf, j);
		j=0;
	}


	if(sUl_Count == FlashDataSize)
	{
		sUl_Count = 0;
		j = 0;
		//		gui_FlashRunCRCClk = SpoiGetSysTICs() ;
		guc_Completed_FlashCRC_Flag=1;
		printf("\nCheckCRC: %u %lu",us_CRCRet,FlashDataSize);
		if(us_CRCRet == FlashCrc)
		{
			printf("\nFlash Data valid at Runtime\n");
			us_CRCRet = 0;
			us_FlashDataFailed=1;
		}
		else
		{
			printf("\nFlash CRC Not OK");
			for(uint16_t i=0;i<200;++i)
			{
				printf("$");
			}
			printf("\n");
			us_CRCRet = 0;
			us_FlashDataFailed=0;
		}
	}
	guc_Completed_FlashCRC_Flag=1;
	return (us_FlashDataFailed);

}



uint16_t crc16_arc(uint16_t crc, const uint8_t *buf, size_t len)
{
	//	static uint32_t k =0;
	//
	//	k++;
	//	if(k > 20400)
	//	{
	//		for(uint8_t i=0;i<len;++i)
	//		{
	//			printf("%02X ",buf[i]);
	//		}
	//		printf("\n");
	//	}
	while (len--)
	{
		crc ^= *buf++;
		for (int k = 0; k < 8; k++)
		{
			if (crc & 1)
			{
				crc = (crc >> 1) ^ 0xA001;
			}
			else
			{
				crc = crc >> 1;
			}
		}
	}
	return crc;
}
