/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "crc.h"
#include "i2c.h"
#include "iwdg.h"
#include "lwip.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Headers.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#warning "Check to update Version"
#define __BUILD_NAME__ "EI_PROTOCOL_CONVERTER"
#define __BUILD_VERSION__ "2.0.5"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	char *ptr= NULL;
	/* USER CODE END 1 */

	/* MPU Configuration--------------------------------------------------------*/
	MPU_Config();

	/* Enable I-Cache---------------------------------------------------------*/
	SCB_EnableICache();

	/* Enable D-Cache---------------------------------------------------------*/
	SCB_EnableDCache();

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	ptr = reset_cause_get_name(reset_cause_get());
#if 0
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CAN2_Init();
	MX_I2C1_Init();
	MX_I2C2_Init();
	MX_IWDG_Init();
	MX_SPI1_Init();
	MX_TIM4_Init();
	MX_TIM5_Init();
	MX_TIM6_Init();
	MX_TIM7_Init();
	MX_UART4_Init();
	MX_UART5_Init();
	MX_UART7_Init();
	MX_USART1_UART_Init();
	MX_LWIP_Init();
	MX_CRC_Init();
	MX_RNG_Init();
	MX_CAN3_Init();
	/* USER CODE BEGIN 2 */
#endif

	MX_GPIO_Init();
	MX_USART1_UART_Init();
	Get_Slot();
	MX_CAN2_Init();
	MX_CAN3_Init();
	MX_I2C1_Init();
	MX_I2C2_Init();
	MX_SPI1_Init();
	Get_Config();
	MX_RNG_Init();
	memset(&Hitachi_fault_injection_st,0,sizeof(HITACHI_FAULT_INJECTION_t));
	HAL_Delay(250);
	printf("\b\b\b\b\b\b\b\b\b\b");
	printf("\n___________________________________ EiPC Start ____________________________\n");
	printf("\r\r\r\nLast Reset Reason:%s\n",ptr);
	Get_Build_Details();
	switch(EI_data.ProtocolType)
	{
	case HITACHI:
		printf("Protocol Selected is HITACHI\n");
		printf("Initializing Ethernet Please Wait...!!\n");
		MX_LWIP_Init();
		udpServer_init();
		printf("Initializing Ethernet Done\n");
		if((EI_data.Hitachi_st.EnableSerialPort == 1) && (EI_data.Hitachi_st.BaudRate > 0) )
		{
			printf("Initializing UART with Baudrate:%lu\n",EI_data.Hitachi_st.BaudRate);
			UART4_Init(EI_data.Hitachi_st.BaudRate);
		}
		print_self_Details();
		Update_HitachiCommands();
		break;
	case SIEMENS:
		printf("Protocol Selected is SIEMENS\n");
		printf("Initializing Ethernet Please Wait...!!\n");
		MX_LWIP_Init();
		udpServer_init();
		printf("Initializing Ethernet Done\n");
		print_self_DetailsSiemens();
		Update_SiemensCommands();
		UpdateSIGNALSNames();
		break;

	case MEDHA:
		printf("Protocol Selected is MEDHA\n");
		if(EI_data.Medha_Prms_st.BaudRate > 0)
		{
			UART4_Init(EI_data.Medha_Prms_st.BaudRate);
		}
		else
		{
			UART4_Init(115200);
		}
		Update_MedhaCommands();
		Print_Medha_Self_Details();
		break;

	case KYOSAN:
		break;

	default:
		printf("Please Select Protocol Type\n");
		while(1);
	}


	printf("Initialization Success\n");
	//	printf("start Ticks:%lu",Ticks);
	MX_IWDG_Init();
	//udpClient_connect();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1)
	{
		switch(EI_data.ProtocolType)
		{
		case HITACHI:
			Process_HitachiEI();
			break;

		case SIEMENS:
			Process_SiemensEI();
			break;

		case MEDHA:
			Process_Medha_EI();
			break;

		default:
			printf("Please Select Protocol Type\n");
			while(1);
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}

	while(1)
	{
		Data_Processing();
		HAL_IWDG_Refresh(&hiwdg);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 6;
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void Get_Build_Details(void)
{
	uint8_t CRC_Array[50] = {0,};
	printf("\n_____________________________________________\n");
	printf("Build Date:%s\n",__DATE__);
	printf("Build Time:%s\n",__TIME__);
	strcpy((char *)CRC_Array,(const char *)__DATE__);
	strcat((char *)CRC_Array,(const char *)__TIME__);
	Build_CRC = GetCRC16(0, CRC_Array, strlen((const char *)CRC_Array));
	printf("Build Name:%s_VERSION:%s\n",__BUILD_NAME__,__BUILD_VERSION__);
	printf("_____________________________________________\n");

}
/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct = {0};

	/* Disables the MPU */
	HAL_MPU_Disable();

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x0;
	MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
	MPU_InitStruct.SubRegionDisable = 0x87;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
