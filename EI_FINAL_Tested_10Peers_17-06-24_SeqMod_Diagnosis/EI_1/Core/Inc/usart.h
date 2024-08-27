/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "Headers.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef huart7;

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */

#define ISP_COM					huart1
#define PORT1_COM				huart5
#define PORT2_COM				huart7
#define PORT3_COM				huart4


#define UART_INTR_BUFF_SIZE		128
#define UART_BUFF_SIZE			1024
#define ISP_UART_BUFF_SIZE		2048



enum ENUM_UARTS {
	ISP_PORT, ONE_PORT, TWO_PORT, THREE_PORT, NO_OF_UARTS
};

typedef struct
{
	uint16_t Write_End;
	uint16_t Read_End;
	uint8_t CIR_BUFF[UART_BUFF_SIZE];

}UART_BUFF_t;

typedef struct
{
	uint16_t Write_End;
	uint16_t Read_End;
	uint8_t CIR_BUFF[ISP_UART_BUFF_SIZE];

}ISP_UART_BUFF_t;

extern volatile uint32_t Isp_Intr_Clk;
extern volatile uint32_t UART4_Intr_Clk;
extern volatile uint32_t DataIntervalFound;
extern UART_BUFF_t UART_BUFFS[NO_OF_UARTS];
extern ISP_UART_BUFF_t ISP_UART_BUFF[1];
extern uint8_t ISP_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
extern uint8_t PORT1_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
extern uint8_t PORT2_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
extern uint8_t PORT3_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_UART7_Init(void);
void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void UART4_Init(uint32_t BaudRate);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
uint8_t Com_DataReady(uint8_t com_num);
uint8_t Com_Get_Byte(uint8_t com_num);
void CommTransmitMessage(uint8_t com_num, uint8_t *sc_pp_Message,unsigned short int us_MaxTansmitBytes);
void Com_SendByte(uint8_t com_num, uint8_t uc_Data);
void Enable_UART_Interrupts(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

