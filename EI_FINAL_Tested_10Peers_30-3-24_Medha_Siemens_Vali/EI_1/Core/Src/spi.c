/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */

unsigned char gucFlagCheck = 0;
unsigned char ConfigFlag = 0;

const uint8_t EEPROM_READ = 0b00000011;
const uint8_t EEPROM_WRITE = 0b00000010;
const uint8_t EEPROM_WRDI = 0b00000100;
const uint8_t EEPROM_WREN = 0b00000110;
const uint8_t EEPROM_RDSR = 0b00000101;
const uint8_t EEPROM_WRSR = 0b00000001;

uint8_t spi_cmd[4]={0};
uint8_t SPI_RX_Data;
GPIO_TypeDef* CHIP_Port;
uint32_t CHIP_GPIO_Pin;
uint16_t Build_CRC = 0;
uint16_t EI_Dat_CRC = 0;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PD7     ------> SPI1_MOSI
    PG9     ------> SPI1_MISO
    */
    GPIO_InitStruct.Pin = SPI1_SCK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(SPI1_SCK_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI1_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(SPI1_MOSI_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI1_MISO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(SPI1_MISO_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PD7     ------> SPI1_MOSI
    PG9     ------> SPI1_MISO
    */
    HAL_GPIO_DeInit(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin);

    HAL_GPIO_DeInit(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin);

    HAL_GPIO_DeInit(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


void saveToFlash(uint8_t uc_Sector,uint32_t Ul_Address,void *pp_DataPtr,uint16_t us_Size){
	uint8_t uc_ChipSelect = (uc_Sector/2)+1;
	uint8_t uc_ChipSel = 0;
	uint32_t Ul_MemAddr = Ul_Address;


	switch(uc_ChipSelect)
	{
	case 1:

		uc_ChipSel=1;

		break;
	case 2:

		uc_ChipSel=2;
		break;
	case 3:

		uc_ChipSel=3;
		break;
	case 4:

		uc_ChipSel=4;
		break;
	}

	SPI_WriteInFlash(uc_ChipSel,Ul_MemAddr,us_Size,pp_DataPtr);
}

uint8_t  SPI_WriteInFlash(uint8_t Chip,uint32_t Ul_Address,uint16_t uc_Len,uint8_t *uc_pp_Buff)
{

	uint8_t Return_Value=0;
	switch(Chip)
	{
	case CHIP1://To select FLASH1
		CHIP_Port = CS1_GPIO_Port;
		CHIP_GPIO_Pin = CS1_Pin;
		break;
	case CHIP2://To select FLASH2
		CHIP_Port = CS2_GPIO_Port;
		CHIP_GPIO_Pin = CS2_Pin;
		break;
	case CHIP3://To select FLASH3
		CHIP_Port = CS3_GPIO_Port;
		CHIP_GPIO_Pin = CS3_Pin;
		break;
	case CHIP4://To select FLASH4
		CHIP_Port = CS4_GPIO_Port;
		CHIP_GPIO_Pin = CS4_Pin;
		break;
	default:printf("Wrong Chip selection\n");
	}
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);





	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_WREN, 1, 100);/* write enable=0x06 */
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

	while (1)
	{
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_RDSR, sizeof(EEPROM_RDSR), 100);/* check status to see if write enabled is latched=0x05 */
		HAL_SPI_Receive(&hspi1, &SPI_RX_Data, sizeof(SPI_RX_Data), 100);
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);
		if ( (SPI_RX_Data & 0x02) == 0x02 )	/* bit 0 is ready, bit 1 is write enable */
		{
			//printf("\n Write enable latched.......");
			break;
		}
		else
		{

			printf("\n Write enable not latched.......");
		}
	}

	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	spi_cmd[0]=EEPROM_WRITE;/* Write command 0x02 */
	spi_cmd[1]=Ul_Address>>16;/* Read address offset is 0x000000 */
	spi_cmd[2]=Ul_Address>>8;
	spi_cmd[3]=Ul_Address;
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 4, 100);

	HAL_SPI_Transmit(&hspi1, uc_pp_Buff,uc_Len , 1000);


	//	printf("Data for Writing:\n");
	//	for(uint32_t ulAddress=0;ulAddress<uc_Len;++ulAddress)
	//	{
	//		if( (ulAddress%8) == 0)
	//		{
	//			printf("\n");
	//		}
	//		printf("[%03lu]:%c ",ulAddress,uc_pp_Buff[ulAddress]);
	//	}


	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

	while (1)
	{
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
		spi_cmd[0]=EEPROM_RDSR;/* check status to see if write cycle is done or not */
		HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 1, 100);
		HAL_SPI_Receive(&hspi1, &SPI_RX_Data, sizeof(SPI_RX_Data), 100);
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);
		if((SPI_RX_Data & 0x01) == 0x00 )	/* bit 0 to 0 is ready */
		{
			Return_Value=1;
			//int	ui_test = 1;
			//printf("ui_test in write flash=%d\n",ui_test);
			break;
		}
		else
		{
			;//DisplayString("\nDevice is busy in Erasing.....",DisPort);
		}
	}
	/*Write Disable*/
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	spi_cmd[0]=EEPROM_WRDI;/* write disable */
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 1, 100);
	HAL_SPI_Receive(&hspi1, &SPI_RX_Data, sizeof(SPI_RX_Data), 100);
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

	return Return_Value;
}


int SPI_MemoryEraseBySector(uint8_t Chip,uint32_t Ul_Address){

	int	Flash_Erase=0;

	switch(Chip)
	{
	case CHIP1:
		CHIP_Port = CS1_GPIO_Port;
		CHIP_GPIO_Pin = CS1_Pin;
		break;
	case CHIP2:
		CHIP_Port = CS2_GPIO_Port;
		CHIP_GPIO_Pin = CS2_Pin;
		break;
	case CHIP3:
		CHIP_Port = CS3_GPIO_Port;
		CHIP_GPIO_Pin = CS3_Pin;
		break;
	case CHIP4:
		CHIP_Port = CS4_GPIO_Port;
		CHIP_GPIO_Pin = CS4_Pin;
		break;
	default:printf("Wrong Chip selection\n");
	}


	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_WREN, 1, 100);/* write enable */
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

	while (1)
	{

		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
		HAL_SPI_Transmit(&hspi1, (uint8_t *)&EEPROM_RDSR, sizeof(EEPROM_RDSR), 100);
		HAL_SPI_Receive(&hspi1, &SPI_RX_Data, sizeof(SPI_RX_Data), 100);
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);
		if ( (SPI_RX_Data & 0x02) == 0x02 )	/* bit 0 is ready, bit 1 is write enable */
		{
			//printf("\n Write enable latched in erase mode.......");
			break;
		}
		else
		{

			printf("\n Write enable not latched in erase mode.......");
		}
	}
	//printf("\nMemory Erase operation started");

	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	spi_cmd[0]=0x20;/* Bluck Erase command is 0xC7, low 256 bytes only */
	spi_cmd[1]=Ul_Address>>16;
	spi_cmd[2]=Ul_Address>>8;
	spi_cmd[3]=Ul_Address;
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 4, 100);
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

	while (1)
	{
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
		spi_cmd[0]=EEPROM_RDSR;/* check status to see if write cycle is done or not */
		HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 1, 100);
		HAL_SPI_Receive(&hspi1, &SPI_RX_Data, sizeof(SPI_RX_Data), 100);
		HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

		if((SPI_RX_Data & 0x01) == 0x00 )	/* bit 0 to 0 is ready */
		{
			Flash_Erase = 1;
			//printf("\nui_test in earse=%d",Flash_Erase);
			break;
		}
		else
		{
			;//DisplayString("\nDevice is busy in Erasing.....",DisPort);
		}
	}
	/*Write Disable*/
	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	spi_cmd[0]=EEPROM_WRDI;/* write disable */
	HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 1, 100);

	HAL_SPI_Receive(&hspi1, &SPI_RX_Data, sizeof(SPI_RX_Data), 100);

	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);
	return Flash_Erase;

}



void readFromFlash(uint8_t uc_Sector, uint32_t Ul_Address,uint16_t us_Size,uint8_t * uc_pp_Buff)
{

	unsigned char	uc_ChipSel =0;

	switch(uc_Sector)
	{
	case 1:
		uc_ChipSel = 1;
		break;
	case 2:
		uc_ChipSel = 2;
		break;
	case 3:
		uc_ChipSel = 3;
		break;
	case 4:
		uc_ChipSel = 4;
		break;
	}

	SPI_ReadFromFlash(uc_ChipSel,Ul_Address ,us_Size,uc_pp_Buff);

}



void SPI_ReadFromFlash(uint8_t Chip,uint32_t Ul_Address,uint16_t uc_Len,uint8_t *uc_pp_Buff)
{
	switch(Chip)
	{
	case CHIP1:
		CHIP_Port = CS1_GPIO_Port;
		CHIP_GPIO_Pin = CS1_Pin;
		break;
	case CHIP2:
		CHIP_Port = CS2_GPIO_Port;
		CHIP_GPIO_Pin = CS2_Pin;
		break;
	case CHIP3:
		CHIP_Port = CS3_GPIO_Port;
		CHIP_GPIO_Pin = CS3_Pin;
		break;
	case CHIP4:
		CHIP_Port = CS4_GPIO_Port;
		CHIP_GPIO_Pin = CS4_Pin;
		break;
	default:printf("Wrong Chip selection\n");
	}

	memset(uc_pp_Buff,0,uc_Len);



	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_RESET);
	spi_cmd[0]=EEPROM_READ;/* Read command is 0x03, low 256 bytes only */
	spi_cmd[1]=Ul_Address>>16;
	spi_cmd[2]=Ul_Address>>8;
	spi_cmd[3]=Ul_Address;


	HAL_SPI_Transmit(&hspi1, (uint8_t *)&spi_cmd, 4, 100);
	HAL_SPI_Receive(&hspi1, uc_pp_Buff, uc_Len, 1000);

	//	printf("Data for Reading:\n");
	//	for(uint32_t ulAddress=0;ulAddress<uc_Len;++ulAddress)
	//	{
	//		if( (ulAddress%8) == 0)
	//		{
	//			printf("\n");
	//		}
	//		printf("[%03lu]:%c ",ulAddress,uc_pp_Buff[ulAddress]);
	//	}


	HAL_GPIO_WritePin(CHIP_Port, CHIP_GPIO_Pin, GPIO_PIN_SET);

}




void Get_Config(void)
{
	uint32_t uc_times = 0;
	//	unsigned char uc_SspIndex =0;//,uc_Block_Index = 0;


	size_t SIZE = (sizeof(EI_t))+6;
	size_t SIZE_VAR  = SIZE;
	//	printf("Size of HITACHI_t Structure:%u\n",sizeof(HITACHI_t));
	//	printf("Size of SIEMENS_t Structure:%u\n",sizeof(SIEMENS_t));
	//	printf("Size of MEDHA_PRMS_t Structure:%u\n",sizeof(MEDHA_PRMS_t));

	//	printf("Size of EI Structure:%u\n",sizeof(EI_t));



#if 0
	printf("Size of EI Structure:%u\n",sizeof(EI_t));
	EI_data.ProtocolType = SIEMENS;

	if(EI_data.ProtocolType == HITACHI)
	{

#if 1

		EI_data.Hitachi_st.SOURCE_ADDRESSES[0] = 141;
		EI_data.Hitachi_st.DEST_ADDRESSES[0] 	= 411;
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][0] = 0x0A06;
		EI_data.Hitachi_st.Bool_outputs[0]		= 30;
		EI_data.Hitachi_st.Interface_Type[0] 	= ETHERNET;

		EI_data.Hitachi_st.SOURCE_ADDRESSES[1] = 142;
		EI_data.Hitachi_st.DEST_ADDRESSES[1] 	= 412;
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][1] = 0x0A06;
		EI_data.Hitachi_st.Bool_outputs[1]		= 4;
		EI_data.Hitachi_st.Interface_Type[1] 	= ETHERNET;

		EI_data.Hitachi_st.SOURCE_ADDRESSES[2] = 143;
		EI_data.Hitachi_st.DEST_ADDRESSES[2] 	= 413;
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][2] = 0x0A06;
		EI_data.Hitachi_st.Bool_outputs[2]		= 32;
		EI_data.Hitachi_st.Interface_Type[2] 	= ETHERNET;

#if EI_1
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][3] = 1;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][0][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][0][RX_PORT] = 60000;
		/* ______________ MLKW 1A______________ */

		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][3] = 101;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][1][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][1][RX_PORT] = 60000;
		/* ______________ MLKW 1B______________ */



		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][3] = 1;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][2][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][2][RX_PORT] = 60000;
		/* ______________ MLKW 1A______________ */

		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][3] = 101;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][3][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][3][RX_PORT] = 60000;
		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][3] = 1;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][4][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][4][RX_PORT] = 60000;
		/* ______________ MLKW 1A______________ */

		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][3] = 101;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][5][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][5][RX_PORT] = 60000;
		/* ______________ MLKW 1B______________ */

		/* ___________ TCAS 1A	___________ */
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][0] = 192;
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][1] = 168;
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][2] = 1;
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][3] = 81;
		/* ___________ TCAS 1A	___________ */


		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][0] = 0x0C;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][1] = 0x1D;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][2] = 0x12;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][3] = 0xE0;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][4] = 0x11;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][5] = 0x11;
		EI_data.Hitachi_st.Self_Listening_Port[CARD_INDEX] = 60000;

#endif
#if EI_2
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][2] = 2;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][0][3] = 1;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][0][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][0][RX_PORT] = 60000;
		/* ______________ MLKW 1A______________ */

		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][2] = 2;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][1][3] = 101;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][1][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][1][RX_PORT] = 60000;
		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][2][3] = 1;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][2][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][2][RX_PORT] = 60000;
		/* ______________ MLKW 1A______________ */

		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][3][3] = 101;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][3][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][3][RX_PORT] = 60000;
		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][4][3] = 1;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][4][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][4][RX_PORT] = 60000;
		/* ______________ MLKW 1A______________ */

		/* ______________ MLKW 1B______________ */

		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][0] = 192;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][1] = 168;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][2] = 1;
		EI_data.Hitachi_st.Source_IP[CARD_INDEX][5][3] = 101;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][5][TX_PORT] = 60000;
		EI_data.Hitachi_st.PORTS[CARD_INDEX][5][RX_PORT] = 60000;
		/* ______________ MLKW 1B______________ */



		/* ___________ TCAS 1A	___________ */
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][0] = 192;
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][1] = 168;
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][2] = 2;
		EI_data.Hitachi_st.Self_IP[CARD_INDEX][3] = 81;
		/* ___________ TCAS 1A	___________ */


		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][0] = 0x0C;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][1] = 0x1D;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][2] = 0x12;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][3] = 0xE0;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][4] = 0x11;
		EI_data.Hitachi_st.Self_MAC[CARD_INDEX][5] = 0X22;
		EI_data.Hitachi_st.Self_Listening_Port[CARD_INDEX] = 60000;

#endif


		EI_data.Hitachi_st.No_of_IP_s = 6;
		EI_data.Hitachi_st.BaudRate = 19200;
		EI_data.Hitachi_st.EnableSerialPort = SET;

		EI_data.Hitachi_st.SOURCE_ADDRESSES[3] = 9411;
		EI_data.Hitachi_st.DEST_ADDRESSES[3] = 1941;

#ifdef EI_1
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][3] = 0x6AD5;
#endif
#ifdef EI_2
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][3] = 0x391D;
#endif

		EI_data.Hitachi_st.Bool_outputs[3]		= 47;
		EI_data.Hitachi_st.Interface_Type[3] 	= SERIAL;


		EI_data.Hitachi_st.SOURCE_ADDRESSES[4] = 9211;
		EI_data.Hitachi_st.DEST_ADDRESSES[4] 	= 1921;
#ifdef EI_1
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][4] = 0xF6EA;
#endif
#ifdef EI_2
		EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][4] = 0xF112;
#endif
		EI_data.Hitachi_st.Bool_outputs[4]		= 77;

		EI_data.Hitachi_st.Interface_Type[4] 	= SERIAL;

		EI_data.Hitachi_st.PeersCnt = 5;


		EI_data.Hitachi_st.CAN_EXCHANGE_TIME_OUT 		= SIX_SEC;
		EI_data.Hitachi_st.CAN_Xchange_Time			= FIVE_SEC;//FIVE_HUNDRED_MS;
		EI_data.Hitachi_st.HEALTH_PACKET_TIME			= ONE_MIN;
		EI_data.Hitachi_st.MIE_SIGNAL_SEND_CLK			= TWO_SEC;



		EI_data.Hitachi_st.STALE_DATA_TIME_OUT[0]		= (FIVE * ONE_SEC)+1500;
		EI_data.Hitachi_st.LinkEstablishTimeout[0]		= (EIGHT * ONE_SEC);
		EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[0]	= (SIX * ONE_SEC);
		EI_data.Hitachi_st.ClockMaster[0]	= SET;

		EI_data.Hitachi_st.STALE_DATA_TIME_OUT[1]		= (FIVE * ONE_SEC)+1500;
		EI_data.Hitachi_st.LinkEstablishTimeout[1]		= (EIGHT * ONE_SEC);
		EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[1]	= (SIX * ONE_SEC);
		EI_data.Hitachi_st.ClockMaster[1]	= SET;


		EI_data.Hitachi_st.STALE_DATA_TIME_OUT[2]		= (FIVE * ONE_SEC)+1500;
		EI_data.Hitachi_st.LinkEstablishTimeout[2]		= (EIGHT * ONE_SEC);
		EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[2]	= (SIX * ONE_SEC);
		EI_data.Hitachi_st.ClockMaster[2]	= SET;

		for(uint8_t Traverser = 3;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
		{
			//		EI_data.Hitachi_st.LAST_RECV_LOGGING_TIMEOUT[Traverser]= THIRTY_SEC;
			//		EI_data.Hitachi_st.RCV_STATUS_FAIL_TIME_OUT[Traverser]	= THIRTY_SEC;
			//		EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser]		= (THIRTY_SEC + FIVE_HUNDRED_MS);
			//		EI_data.Hitachi_st.STATUS_MSG_TIMEOUT[Traverser]		= (THIRTY_SEC);
			//		EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser]	= THIRTY_SEC;

			EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser]		= (FIFTEEN * ONE_SEC)+ONE_SEC;
			EI_data.Hitachi_st.LinkEstablishTimeout[Traverser]		= (FIFTEEN * ONE_SEC);
			EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser]	= (FIFTEEN * ONE_SEC);
			EI_data.Hitachi_st.ClockMaster[Traverser]	= SET;
		}

		EI_Dat_CRC = 0x3131;
	}
	else if(EI_data.ProtocolType == SIEMENS)
	{

		EI_data.Siemens_st.DestSysType =  10;
		EI_data.Siemens_st.SourceSysType = 10;
		EI_data.Siemens_st.DestPort = 8;
		EI_data.Siemens_st.SourcePort = 3;

		EI_data.Siemens_st.Booloutputs = 64;
		EI_data.Siemens_st.CAN_EXCHANGE_TIME_OUT = FIVE_SEC;
		EI_data.Siemens_st.CAN_Xchange_Time = FIVE_HUNDRED_MS;
		EI_data.Siemens_st.COMM_FAILURE_TIME_OUT = TWO_SEC;
		EI_data.Siemens_st.DestAddress = 0;
		EI_data.Siemens_st.SourceAddress = 610272;
		EI_data.Siemens_st.HEALTH_PACKET_TIME = FIVE_SEC;
		EI_data.Siemens_st.No_of_IP_s = 4;
		EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount = 10;
		EI_data.Siemens_st.MIE_SIGNAL_SEND_CLK = TWO_SEC;



#ifdef EI_1

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 101;
		EI_data.Siemens_st.PORTS[CARD_INDEX][0][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][0][RX_PORT] = 3500;

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 102;
		EI_data.Siemens_st.PORTS[CARD_INDEX][1][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][1][RX_PORT] = 3500;

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 103;
		EI_data.Siemens_st.PORTS[CARD_INDEX][2][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][2][RX_PORT] = 3500;

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 104;
		EI_data.Siemens_st.PORTS[CARD_INDEX][3][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][3][RX_PORT] = 3500;

		EI_data.Siemens_st.No_of_IP_s = 4;

		EI_data.Siemens_st.Self_IP[CARD_INDEX][0] = 10;
		EI_data.Siemens_st.Self_IP[CARD_INDEX][1] = 91;
		EI_data.Siemens_st.Self_IP[CARD_INDEX][2] = 134;
		EI_data.Siemens_st.Self_IP[CARD_INDEX][3] = 203;
		EI_data.Siemens_st.Self_Listening_Port[CARD_INDEX] = 3500;


		EI_data.Siemens_st.Self_MAC[CARD_INDEX][0] = 0x10;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][1] = 0x11;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][2] = 0x12;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][3] = 0x13;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][4] = 0x14;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][5] = 0x15;
#endif
#ifdef EI_2

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 101;
		EI_data.Siemens_st.PORTS[CARD_INDEX][0][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][0][RX_PORT] = 3500;

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 102;
		EI_data.Siemens_st.PORTS[CARD_INDEX][1][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][1][RX_PORT] = 3500;

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 103;
		EI_data.Siemens_st.PORTS[CARD_INDEX][2][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][2][RX_PORT] = 3500;

		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][0] = 10;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][1] = 91;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][2] = 134;
		EI_data.Siemens_st.Source_IP[CARD_INDEX][0][3] = 104;
		EI_data.Siemens_st.PORTS[CARD_INDEX][3][TX_PORT] = 3500;
		EI_data.Siemens_st.PORTS[CARD_INDEX][3][RX_PORT] = 3500;

		EI_data.Siemens_st.No_of_IP_s = 4;

		EI_data.Siemens_st.Self_IP[CARD_INDEX][0] = 10;
		EI_data.Siemens_st.Self_IP[CARD_INDEX][1] = 91;
		EI_data.Siemens_st.Self_IP[CARD_INDEX][2] = 134;
		EI_data.Siemens_st.Self_IP[CARD_INDEX][3] = 204;
		EI_data.Siemens_st.Self_Listening_Port[CARD_INDEX] = 3500;


		EI_data.Siemens_st.Self_MAC[CARD_INDEX][0] = 0x20;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][1] = 0x21;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][2] = 0x22;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][3] = 0x23;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][4] = 0x24;
		EI_data.Siemens_st.Self_MAC[CARD_INDEX][5] = 0x25;

#endif

	}
	else if(EI_data.ProtocolType == MEDHA)
	{
		uint8_t Temp_buff[100] = {0,};
		strcpy((char *)EI_data.STN_NAME,"BHARWARI");
		EI_data.Medha_Prms_st.BaudRate 				= 115200;
		EI_data.Medha_Prms_st.Comport  				= 3;
		EI_data.Medha_Prms_st.Dest_Addr 			= 37128;
		EI_data.Medha_Prms_st.Source_Addr 			= 109;
		EI_data.Medha_Prms_st.NoofRelays 			= 150;
		EI_data.Medha_Prms_st.Packet_Interval_ms 	= 1000;
		EI_data.Medha_Prms_st.Packet_Timeout_Count 	= 6;
		EI_data.Medha_Prms_st.Max_Mismatch_value 	= 6;
		EI_data.Medha_Prms_st.SendSignal2MIE_CLK    = 2000;
		EI_data.Medha_Prms_st.CAN_Xchange_Time		= 150;
		EI_data.Medha_Prms_st.CAN_Xchange_Tout 		= 5000;

		memcpy(&Temp_buff,&EI_data,sizeof(EI_t));
		EI_Dat_CRC = CommGetCRC16(0, Temp_buff, sizeof(EI_t));
		printf("CRC OK from Flash\n");
		printf("Application CRC:0X%04X\n",EI_Dat_CRC);
	}
	else
	{
		printf("Please Select Protocol Type\n");
		while(1);
	}


	return;
#endif

#endif

	for(uc_times = 0; uc_times < SIZE; uc_times+=32)
	{
		HAL_Delay(20);
		if(SIZE_VAR <32)
		{
			SPI_ReadFromFlash(1, uc_times, SIZE_VAR, Config_Buff+uc_times);
		}
		else
		{
			SPI_ReadFromFlash(1, uc_times, 32, Config_Buff+uc_times);
			SIZE_VAR-=32;
		}
	}
	if(CheckCRC(Config_Buff, (sizeof(EI_t)+4)))
	{
		/* CRC OK From the Flash */
		//EI_Config_ptr = &EI_data+4;
		//memcpy(EI_Config_ptr,&Config_Buff[4],sizeof(EI_t));
		memcpy(&EI_data,&Config_Buff[4],sizeof(EI_t));
		EI_Dat_CRC = ExtractBits(0, 16,&Config_Buff[(sizeof(EI_t)+4)]);
		//		printf("DAT CRC:0X%04X\n",EI_Dat_CRC);
	}
	else
	{
		printf("\n EI Configuration CRC from Flash:%u\n",(sizeof(EI_t)+4));
		EI_Config_ptr = (EI_t *)&Config_Buff[4];
		printf("\nCRC READ:[%x][%x]\n",Config_Buff[sizeof(EI_t)+3],Config_Buff[sizeof(EI_t)+4]);
		printf("\nCheck for CRC:");
		gucFlagUpdate =3;
		printf("\n*************PLEASE LOAD EI DATS USING APPLICATION************\n");

		while(1)
		{
			Data_Processing();
		}
	}
	if( 0 == UpdateFlag )
	{
		readCanToStructs();
	}
}


void Data_Processing(void)
{
	switch(gucFlagUpdate)
	{
	case 3:
		gucUpdateFlag = CheckForRqst(ISP_PORT);
		if(1 == gucUpdateFlag)
		{
			AutoLoadDat(ISP_PORT); //
		}
		break;
	default:
		break;
	}
}

void AutoLoadDat(unsigned char uc_port)
{
	unsigned char AckCount=0, NoOfBytes;
	while(1)
	{
		NoOfBytes=ReadAndFormConfig(uc_port,Config_Buff,1024);
		HAL_IWDG_Refresh(&hiwdg);

		//SpoiWDTReload();
		//CommWatchDogStrobe();
		if ( NoOfBytes > 1)
		{
			gucUpdateFlag = 0;
			if(!((Config_Buff[1] == ((DATLOADING>>8)&0XFF)) && (Config_Buff[2] == (DATLOADING & 0XFF)) && (Config_Buff[3] == DATCONFRMTION)))
			{
				printf("\nReceived Request Again\n");
				return;
			}
			EI_Config_ptr = (EI_t *)&Config_Buff[4];
			saveConfig();
			gucFlagCheck =1;
			ConfigFlag = 1;
			printf("\nData Received\n");
			break;
		}
		else if(1 == NoOfBytes)
		{
			if(3 == AckCount)
			{
				gucUpdateFlag = 0;
				SendAck(uc_port, DATLOADING, CRCNOK);
				break;
			}
			SendAck(uc_port, DATLOADING, ACKOK);
			gucReqstCurTim = HAL_GetTick();
			AckCount++;
		}
		else
		{
			if( (labs(HAL_GetTick() - gucReqstCurTim) > (WAITFORDAT * 1000)) )
			{
				gucUpdateFlag = 0;
				printf("\nNo Data recieved to Update\n");
				SendAck(uc_port, DATLOADING, NODATA);
				break;
			}
		}
	}
}

unsigned char saveConfig(void)
{
	uint32_t ulAddress = 0;

	printf("\nSaveConfig\n");
	for(ulAddress=0;ulAddress<6;ulAddress++)
	{
		if(SPI_MemoryEraseBySector(1,(4096*ulAddress)))/*memory erase before writing*/
		{
			HAL_IWDG_Refresh(&hiwdg);
			printf("FLASH ERASE SUCCESS\n");
		}
	}
	size_t SIZE = (sizeof(EI_t)+7);
	size_t SIZE_VAR  = SIZE;

	for(ulAddress = 0; ulAddress < SIZE; ulAddress+=32)
	{
		HAL_IWDG_Refresh(&hiwdg);
		if(SIZE_VAR <32)
		{
			SPI_WriteInFlash(1, ulAddress, SIZE_VAR, Config_Buff+ulAddress);
		}
		else
		{
			SPI_WriteInFlash(1, ulAddress, 32, Config_Buff+ulAddress);
			SIZE_VAR-=32;
		}
	}

	printf("\nCFG written successfully");


	//	for(uint16_t i=0;i<SIZE;i++)
	//	{
	//		printf("[%02u]:0x%02x|",i,Config_Buff[i]);
	//		if((i%8)==0)
	//		{
	//			printf("\n");
	//		}
	//	}
	NVIC_SystemReset();
	return 1;
}


void Print_BuildDat_Details(void)
{
	if(Debug_flag == 0)
	{
		return;
	}
	static uint32_t PrintClk = 0;
	if(HAL_Elaps_Ticks(PrintClk) >= TEN_SEC)
	{
		//		strcpy((char *)EI_data.STN_NAME,"IRISET");
		printf("\n__________________BUILD DETAILS__________________\n");
		printf("STATION NAME:%s|",EI_data.STN_NAME);
		printf("APP CRC:0X%04X|",EI_Dat_CRC);
		printf("EXE CRC:0X%04X\n\n",Build_CRC);
		//printf("*********************************************************\n");

		PrintClk = HAL_GetTick();
	}
}
/* USER CODE END 1 */
