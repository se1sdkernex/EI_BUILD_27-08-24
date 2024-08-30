/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.h
 * @brief   This file contains all the function prototypes for
 *          the spi.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "Headers.h"


/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */



extern unsigned char gucFlagCheck;
extern unsigned char ConfigFlag;
extern uint16_t Build_CRC;
extern uint16_t EI_Dat_CRC;
/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */
void saveToFlash(uint8_t uc_Sector,uint32_t Ul_Address,void *pp_DataPtr,uint16_t us_Size);
uint8_t  SPI_WriteInFlash(uint8_t Chip,uint32_t Ul_Address,uint16_t uc_Len,uint8_t *uc_pp_Buff);
int SPI_MemoryEraseBySector(uint8_t ,uint32_t);
void readFromFlash(uint8_t uc_Sector, uint32_t Ul_Address,uint16_t us_Size,uint8_t * uc_pp_Buff);
void SPI_ReadFromFlash(uint8_t Chip,uint32_t Ul_Address,uint16_t uc_Len,uint8_t *uc_pp_Buff);
void Get_Config(void);
void Data_Processing(void);
void AutoLoadDat(unsigned char uc_port);
unsigned char saveConfig(void);
void Print_BuildDat_Details(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

