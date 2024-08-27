/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    crc.c
 * @brief   This file provides code for the configuration
 *          of the CRC instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "crc.h"

/* USER CODE BEGIN 0 */
const unsigned short int ulCrc16Table[16] = {0x0000, 0xCC01, 0xD801, 0x1400,
		0xF001, 0x3C00, 0x2800, 0xE401,
		0xA001, 0x6C00, 0x7800, 0xB401,
		0x5000, 0x9C01, 0x8801, 0x4400 };
/* USER CODE END 0 */

CRC_HandleTypeDef hcrc;

/* CRC init function */
void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
  hcrc.Init.GeneratingPolynomial = 79764919;
  hcrc.Init.CRCLength = CRC_POLYLENGTH_32B;
  hcrc.Init.InitValue = 0;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

void HAL_CRC_MspInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspInit 0 */

  /* USER CODE END CRC_MspInit 0 */
    /* CRC clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
  /* USER CODE BEGIN CRC_MspInit 1 */

  /* USER CODE END CRC_MspInit 1 */
  }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* crcHandle)
{

  if(crcHandle->Instance==CRC)
  {
  /* USER CODE BEGIN CRC_MspDeInit 0 */

  /* USER CODE END CRC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
  /* USER CODE BEGIN CRC_MspDeInit 1 */

  /* USER CODE END CRC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

short int CheckCRC(unsigned char *pcMsg,short int iCount)
{
	static unsigned short int uiCrc,k = 0;
	short int retVal = 0;

#if 0
	uint16_t i = 0;
	printf("For crc calculation:\n");
	for(i=0;i<iCount;++i)
	{
		printf("[%03u]:%02X ",i,pcMsg[i]);
	}
#endif

	uiCrc = GetCRC16( 0, pcMsg,iCount);
	k = (unsigned short int ) (pcMsg[iCount] & 0xff);
	k = (unsigned short int ) (k<<8);
	k |= pcMsg[iCount+1] & 0xff;

	if(k == uiCrc)
	{
		retVal = 1;
		return k;
	}
	printf("\nCalculated CRC:%04x\n",uiCrc);
	printf("\nExtracted CRC :%04x\n",k);
	return retVal;
}

unsigned short int GetCRC16 (short int iStart,unsigned char *pcPtr, short int iNum)
{
	unsigned short int uiCrc = (unsigned short int ) iStart;
	short int r;

	/* while there is more data to process */
	while (iNum-- > 0)
	{
		/* compute checksum of lower four bits of *pcPtr */
		r = ulCrc16Table[uiCrc & 0xF];
		uiCrc = (unsigned short int) ((uiCrc >> 4) & 0x0FFF);
		uiCrc = (unsigned short int) (uiCrc ^ r ^ ulCrc16Table[*pcPtr & 0xF]);

		/* now compute checksum of upper four bits of *pcPtr */
		r = ulCrc16Table[uiCrc & 0xF];
		uiCrc =(unsigned short int) ((uiCrc >> 4) & 0x0FFF);
		uiCrc =(unsigned short int) (uiCrc ^ r ^ ulCrc16Table[(*pcPtr >> 4) & 0xF]);

		/* next... */
		pcPtr++;
	}
	return(uiCrc);
}

unsigned short int ComputeCRC(unsigned char *uc_pp_pcMsg, const short int Ksi_iCount)
{
	static unsigned short int Susi_uiCrc = 0,Susi_CalculateCRC = 0;
	Susi_uiCrc = GetCRC16( 0, uc_pp_pcMsg, Ksi_iCount);
	Susi_CalculateCRC=Susi_uiCrc;

	uc_pp_pcMsg[Ksi_iCount+1] = (char) (Susi_uiCrc & 0x00ff);
	Susi_uiCrc =(unsigned short int) (Susi_uiCrc >> 8);
	uc_pp_pcMsg[Ksi_iCount] = (char) (Susi_uiCrc & 0x00ff);


	return Susi_CalculateCRC;
}


unsigned short int CommCheckCRC(unsigned char *puc_Msg,
		unsigned short int us_Count)
{
	static unsigned short int sus_Crc,sus_CalCrc;
	unsigned short int us_RetVal = ZERO;

	sus_Crc = CommGetCRC16( ZERO, puc_Msg,us_Count);
	sus_CalCrc = ZERO;
	sus_CalCrc = (unsigned short int ) (puc_Msg[us_Count] & HEX_FF);
	sus_CalCrc = (unsigned short int ) (sus_CalCrc << EIGHT);
	sus_CalCrc |= puc_Msg[us_Count + ONE] & HEX_FF;

	if(sus_CalCrc == sus_Crc)
	{
		us_RetVal = ONE;
	}

	return us_RetVal;
}

unsigned short int CommGetCRC16(unsigned short int us_Start,
		unsigned char *puc_Ptr, unsigned short int us_Num)
{
	unsigned short int us_Crc =  us_Start;
	unsigned short int us_r = ZERO;

	/* while there is more data to process */
	while (us_Num-- > ZERO)
	{
		/* compute checksum of lower four bits of *pcPtr */
		us_r = ulCrc16Table[us_Crc & HEX_F];
		us_Crc = (unsigned short int) ((us_Crc >> FOUR) & HEX_FFF);
		us_Crc = (unsigned short int)
                        		(us_Crc ^ us_r ^ ulCrc16Table[*puc_Ptr & HEX_F]);

		/* now compute checksum of upper four bits of *pcPtr */
		us_r = ulCrc16Table[us_Crc & HEX_F];
		us_Crc =(unsigned short int) ((us_Crc >> FOUR) & HEX_FFF);
		us_Crc =(unsigned short int)
                		(us_Crc ^ us_r ^ ulCrc16Table[(*puc_Ptr >> FOUR) & HEX_F]);

		/* next... */
		puc_Ptr++;
	}
	return(us_Crc);
}	/* End of CommGetCRC16 */




/*____________________________ANSALDO_CRC__________________________*/

unsigned int ReverseCRCPolynomial(unsigned int polynomial)
{
	unsigned int RevPol = 0;
	unsigned int TempPol = 0;
	unsigned int Temp = 0;
	int i;

	TempPol = polynomial << 1; // Remove MSB by shifting one position

	for (i = 0; i < 32; i++)
	{

		Temp = (TempPol & 1);
		RevPol = RevPol | (Temp << (31 - i));
		TempPol = TempPol >> 1;
	}

	return (RevPol);
}

unsigned int CalculateDirectReverseCrc(unsigned char *Arr, unsigned short int Listsize)
{
	int Crc = 0;
	unsigned int RPoly = 0;
	int converted = 0;
	int i=0;
	unsigned int polynomial = 0xD20D8A81;

	//printf("Arr[0]=0x%x\n",Arr[0]);
	//printf("Arr[%d]=0x%x\n",Listsize,Arr[Listsize]);
	RPoly = ReverseCRCPolynomial(polynomial);

	for (i = 0; i < Listsize; i++)
	{
		//	printf("ARR[%d]=0x%x|",i,Arr[i]);
		Crc = AnsoldoCrc(Crc, Arr[i], RPoly);
	}
	/*____________________REVERSE CRC____________________*/
	//printf("Before Calculated crc:0x%x\n", Crc);
	converted |= ((0xff & Crc) << 24);
	converted |= (((0xff << 8) & Crc) << 8);
	converted |= (((0xff << 16) & Crc) >> 8);
	converted |= (((0xffu << 24) & Crc) >> 24); //Appended u to avoid warning out of range
	/*____________________REVERSE CRC____________________*/
	//printf("After Calculated crc:0x%x\n", converted);

	if(Rx_Seq_failure == 1)
	{
		return (converted+(HAL_RNG_GetRandomNumber(&hrng) % 10) + 1);
	}

	return converted;
}

unsigned int AnsoldoCrc(unsigned int Crc, unsigned char InputData, unsigned int Rpoly)
{

	unsigned char i;
	unsigned int xorvalue; /* XOR of bit of data and EDC values */
	unsigned int CrcLsb;   /* Bit 0 of EDC value / uint DataLsb; / Bit 0 of data byte */
	unsigned int DataLsb;
	unsigned int databyte;				/* Current shifted value of data byte */
	databyte = (unsigned int)InputData; /* translated from char to int format */

	for (i = 1; i <= 8; i++)
	{
		CrcLsb = Crc & 1;		/* get LSB of CRC Value */
		DataLsb = databyte & 1; /*get LSB of Data */
		/*byte of current bits left in data */
		Crc >>= 1; /* shift CRC by one bit */
		xorvalue = CrcLsb ^ DataLsb;

		if (xorvalue == 1)
		{
			Crc ^= Rpoly; /* XOR not zero then xor in poly */
		}
		databyte >>= 1; /* Shift data byte*/
	}
	return (Crc);
}





/*____________________________ANSALDO_CRC__________________________*/



/* USER CODE END 1 */
