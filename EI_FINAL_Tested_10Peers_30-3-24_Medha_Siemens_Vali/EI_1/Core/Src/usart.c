/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
volatile uint32_t UART4_Intr_Clk = 0;
volatile uint32_t Isp_Intr_Clk = 0;

UART_BUFF_t UART_BUFFS[NO_OF_UARTS];
ISP_UART_BUFF_t ISP_UART_BUFF[1];
uint8_t ISP_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
uint8_t PORT1_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
uint8_t PORT2_PORT_RX_BUF[UART_INTR_BUFF_SIZE];
uint8_t PORT3_PORT_RX_BUF[UART_INTR_BUFF_SIZE];




/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart7;
UART_HandleTypeDef huart1;

/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */
	HAL_UARTEx_ReceiveToIdle_IT(&huart4, PORT3_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
  /* USER CODE END UART4_Init 2 */

}
/* UART5 init function */
void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */
	HAL_UARTEx_ReceiveToIdle_IT(&huart5, PORT1_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
  /* USER CODE END UART5_Init 2 */

}
/* UART7 init function */
void MX_UART7_Init(void)
{

  /* USER CODE BEGIN UART7_Init 0 */

  /* USER CODE END UART7_Init 0 */

  /* USER CODE BEGIN UART7_Init 1 */

  /* USER CODE END UART7_Init 1 */
  huart7.Instance = UART7;
  huart7.Init.BaudRate = 115200;
  huart7.Init.WordLength = UART_WORDLENGTH_8B;
  huart7.Init.StopBits = UART_STOPBITS_1;
  huart7.Init.Parity = UART_PARITY_NONE;
  huart7.Init.Mode = UART_MODE_TX_RX;
  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART7_Init 2 */
	HAL_UARTEx_ReceiveToIdle_IT(&huart7, PORT2_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
  /* USER CODE END UART7_Init 2 */

}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	HAL_UARTEx_ReceiveToIdle_IT(&huart1, ISP_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
    PeriphClkInitStruct.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PH13     ------> UART4_TX
    PH14     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = UART4_TX_Pin|UART4_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5;
    PeriphClkInitStruct.Uart5ClockSelection = RCC_UART5CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**UART5 GPIO Configuration
    PD2     ------> UART5_RX
    PB9     ------> UART5_TX
    */
    GPIO_InitStruct.Pin = UART5_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(UART5_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = UART5_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_UART5;
    HAL_GPIO_Init(UART5_TX_GPIO_Port, &GPIO_InitStruct);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
  }
  else if(uartHandle->Instance==UART7)
  {
  /* USER CODE BEGIN UART7_MspInit 0 */

  /* USER CODE END UART7_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART7;
    PeriphClkInitStruct.Uart7ClockSelection = RCC_UART7CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART7 clock enable */
    __HAL_RCC_UART7_CLK_ENABLE();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**UART7 GPIO Configuration
    PF6     ------> UART7_RX
    PF7     ------> UART7_TX
    */
    GPIO_InitStruct.Pin = UART7_RX_Pin|UART7_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* UART7 interrupt Init */
    HAL_NVIC_SetPriority(UART7_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART7_IRQn);
  /* USER CODE BEGIN UART7_MspInit 1 */

  /* USER CODE END UART7_MspInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = USART1_TX_Pin|USART1_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PH13     ------> UART4_TX
    PH14     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOH, UART4_TX_Pin|UART4_RX_Pin);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();

    /**UART5 GPIO Configuration
    PD2     ------> UART5_RX
    PB9     ------> UART5_TX
    */
    HAL_GPIO_DeInit(UART5_RX_GPIO_Port, UART5_RX_Pin);

    HAL_GPIO_DeInit(UART5_TX_GPIO_Port, UART5_TX_Pin);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART7)
  {
  /* USER CODE BEGIN UART7_MspDeInit 0 */

  /* USER CODE END UART7_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART7_CLK_DISABLE();

    /**UART7 GPIO Configuration
    PF6     ------> UART7_RX
    PF7     ------> UART7_TX
    */
    HAL_GPIO_DeInit(GPIOF, UART7_RX_Pin|UART7_TX_Pin);

    /* UART7 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART7_IRQn);
  /* USER CODE BEGIN UART7_MspDeInit 1 */

  /* USER CODE END UART7_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, USART1_TX_Pin|USART1_RX_Pin);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void UART4_Init(uint32_t BaudRate)
{

	/* USER CODE BEGIN UART4_Init 0 */

	/* USER CODE END UART4_Init 0 */

	/* USER CODE BEGIN UART4_Init 1 */

	/* USER CODE END UART4_Init 1 */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = BaudRate;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART4_Init 2 */
	HAL_UARTEx_ReceiveToIdle_IT(&huart4, PORT3_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
	/* USER CODE END UART4_Init 2 */

}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART1)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, ISP_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
		for(uint16_t i=0;i<Size;++i)
		{
			ISP_UART_BUFF[ISP_PORT].CIR_BUFF[ISP_UART_BUFF[ISP_PORT].Write_End++] = ISP_PORT_RX_BUF[i];
			if(ISP_UART_BUFF[ISP_PORT].Write_End >= ISP_UART_BUFF_SIZE)
			{
				ISP_UART_BUFF[ISP_PORT].Write_End = 0;
			}
		}
		Isp_Intr_Clk = HAL_GetTick();
	}
	else if(huart->Instance == UART5)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart5, PORT1_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
		for(uint16_t i=0;i<Size;++i)
		{
			UART_BUFFS[ONE_PORT].CIR_BUFF[UART_BUFFS[ONE_PORT].Write_End++] = PORT1_PORT_RX_BUF[i];
			if(UART_BUFFS[ONE_PORT].Write_End >= UART_BUFF_SIZE)
			{
				UART_BUFFS[ONE_PORT].Write_End = 0;
			}
		}
	}
	else if(huart->Instance == UART7)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart7, PORT2_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt

		for(uint16_t i=0;i<Size;++i)
		{
			UART_BUFFS[TWO_PORT].CIR_BUFF[UART_BUFFS[TWO_PORT].Write_End++] = PORT2_PORT_RX_BUF[i];
			if(UART_BUFFS[TWO_PORT].Write_End >= UART_BUFF_SIZE)
			{
				UART_BUFFS[TWO_PORT].Write_End = 0;
			}
		}

	}
	else if(huart->Instance == UART4)
	{
		HAL_UARTEx_ReceiveToIdle_IT(&huart4, PORT3_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt

		for(uint16_t i=0;i<Size;++i)
		{
			UART_BUFFS[THREE_PORT].CIR_BUFF[UART_BUFFS[THREE_PORT].Write_End++] = PORT3_PORT_RX_BUF[i];
			if(UART_BUFFS[THREE_PORT].Write_End >= UART_BUFF_SIZE)
			{
				UART_BUFFS[THREE_PORT].Write_End = 0;
			}
		}
		UART4_Intr_Clk = HAL_GetTick();
	}
}



uint8_t Com_DataReady(uint8_t com_num)
{
	uint8_t IS_DATA_AVAILABLE = NO;
	if(com_num == ISP_PORT)
	{
		if(ISP_UART_BUFF[com_num].Write_End != ISP_UART_BUFF[com_num].Read_End)
		{
			IS_DATA_AVAILABLE = YES;
		}
	}
	else
	{
		if (UART_BUFFS[com_num].Write_End != UART_BUFFS[com_num].Read_End)
		{
			IS_DATA_AVAILABLE = YES;
		}
	}
	return IS_DATA_AVAILABLE;
}


uint8_t Com_Get_Byte(uint8_t com_num)
{
	uint8_t Return_data = 0;


	if(com_num == 0)
	{
		Return_data = ISP_UART_BUFF[com_num].CIR_BUFF[ISP_UART_BUFF[com_num].Read_End++];
		if ( ISP_UART_BUFF[com_num].Read_End >= ISP_UART_BUFF_SIZE)
		{
			ISP_UART_BUFF[com_num].Read_End = 0;
		}
	}
	else
	{
		Return_data = UART_BUFFS[com_num].CIR_BUFF[UART_BUFFS[com_num].Read_End++];
		if ( UART_BUFFS[com_num].Read_End >= UART_BUFF_SIZE)
		{
			UART_BUFFS[com_num].Read_End = 0;
		}
	}
	return Return_data;
}


void CommTransmitMessage(uint8_t com_num,uint8_t *sc_pp_Message,unsigned short int us_MaxTansmitBytes)
{
	unsigned short int us_LoopVar = ZERO;


	/* Transmit the packet based on the randomization number
	 */
	for(us_LoopVar = ZERO; us_LoopVar <= us_MaxTansmitBytes; us_LoopVar++)
	{
		if (  (ZERO != us_LoopVar)&&(START_OF_MESSAGE == sc_pp_Message[us_LoopVar]))
		{
			Com_SendByte(com_num,DLE);
		}
		if (  (us_LoopVar >= ONE)&& (us_LoopVar < us_MaxTansmitBytes)&& (DLE == sc_pp_Message[us_LoopVar] ))
		{
			Com_SendByte(com_num,DLE);
		}
		if (  (END_OF_MESSAGE == sc_pp_Message[us_LoopVar])&& (us_LoopVar != us_MaxTansmitBytes))
		{
			//printf("\n Sent END_OF_MESSAGE");
			Com_SendByte(com_num,DLE);
		}
		Com_SendByte(com_num, sc_pp_Message[us_LoopVar]);
	}

	return;
}

void Com_SendByte(uint8_t com_num, uint8_t uc_Data)
{
	uint8_t *ptr = &uc_Data;
	if(com_num == ISP_PORT)
	{
		HAL_UART_Transmit(&ISP_COM,(const uint8_t *) ptr, 1, 0xffff);
	}
	else if(com_num == ONE_PORT)
	{
		HAL_UART_Transmit(&PORT1_COM,(const uint8_t *) ptr, 1, 0xffff);
	}
	else if(com_num == TWO_PORT)
	{
		HAL_UART_Transmit(&PORT2_COM,(const uint8_t *) ptr, 1, 0xffff);
	}
	else if(com_num == THREE_PORT)
	{
		HAL_UART_Transmit(&PORT3_COM,(const uint8_t *) ptr, 1, 0xffff);
	}

	ptr=NULL;
	free(ptr);
}

void Enable_UART_Interrupts(void)
{
	/*
	 * This function is called because of UART interrupt is not receiving when the
	 * following calls are made in Interrupt (HAL_UARTEx_RxEventCallback) and it is Only happening when inserted in LOCO,RIU bin's
	 * so that's why calling HAL_UARTEx_ReceiveToIdle_IT functions explicitly.
	 *
	 */

	HAL_UARTEx_ReceiveToIdle_IT(&huart1, ISP_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
	HAL_UARTEx_ReceiveToIdle_IT(&huart5, PORT1_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
	HAL_UARTEx_ReceiveToIdle_IT(&huart7, PORT2_PORT_RX_BUF, UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
	HAL_UARTEx_ReceiveToIdle_IT(&huart4, PORT3_PORT_RX_BUF , UART_INTR_BUFF_SIZE);//To Enable Next Interrupt
}


/* USER CODE END 1 */
