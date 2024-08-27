/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.c
 * @brief   This file provides code for the configuration
 *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */

CAN_msg       CAN_TxMsg;      /* CAN messge for sending */
canSynch canStatus[2][MAXCANID];
canReadStruct canRead;
CAN_TxHeaderTypeDef TxHeader2,TxHeader3;
uint32_t pTxMailbox2,pTxMailbox3;
CAN_RxHeaderTypeDef RxHeader0,RxHeader1,RxHeader2;
uint8_t RxData_can1[8] = {0,};
uint8_t RxData_can2[8] = {0,};
uint8_t RxData_can3[8] = {0,};
uint8_t TxData[8] = {0,};
uint8_t Error;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan2;
CAN_HandleTypeDef hcan3;

/* CAN2 init function */
void MX_CAN2_Init(void)
{

	/* USER CODE BEGIN CAN2_Init 0 */

	/* USER CODE END CAN2_Init 0 */

	/* USER CODE BEGIN CAN2_Init 1 */

	/* USER CODE END CAN2_Init 1 */
	hcan2.Instance = CAN2;
	hcan2.Init.Prescaler = 18;
	hcan2.Init.Mode = CAN_MODE_NORMAL;
	hcan2.Init.SyncJumpWidth = CAN_SJW_4TQ;
	hcan2.Init.TimeSeg1 = CAN_BS1_2TQ;
	hcan2.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan2.Init.TimeTriggeredMode = DISABLE;
	hcan2.Init.AutoBusOff = DISABLE;
	hcan2.Init.AutoWakeUp = DISABLE;
	hcan2.Init.AutoRetransmission = ENABLE;
	hcan2.Init.ReceiveFifoLocked = DISABLE;
	hcan2.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CAN2_Init 2 */

#if 0
	CAN_FilterTypeDef Can_FilterConfig2;

	Can_FilterConfig2.FilterActivation = CAN_FILTER_ENABLE ;
	Can_FilterConfig2.FilterBank = 7;
	Can_FilterConfig2.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	Can_FilterConfig2.FilterIdHigh = 0x200<<5;
	Can_FilterConfig2.FilterIdLow=0x0000;
	Can_FilterConfig2.FilterMaskIdHigh=0x200<<5;
	Can_FilterConfig2.FilterMaskIdLow =0x0000;
	Can_FilterConfig2.FilterMode= CAN_FILTERMODE_IDMASK;
	Can_FilterConfig2.FilterScale = CAN_FILTERSCALE_32BIT;
	Can_FilterConfig2.SlaveStartFilterBank = 6;

	HAL_CAN_ConfigFilter(&hcan2,&Can_FilterConfig2 );
#endif
	CAN_FilterTypeDef Can_FilterConfig3;
	/*____________for Testing________________*/
	Can_FilterConfig3.FilterActivation = CAN_FILTER_ENABLE ;
	Can_FilterConfig3.FilterBank = 4;
	Can_FilterConfig3.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	Can_FilterConfig3.FilterIdHigh = (GPS_TIME_PKT<<5);//CM_LPM_GPSINFO
	Can_FilterConfig3.FilterIdLow=0x0000;
	Can_FilterConfig3.FilterMaskIdHigh= (GPS_TIME_PKT<<5);
	Can_FilterConfig3.FilterMaskIdLow =0x0000;
	Can_FilterConfig3.FilterMode= CAN_FILTERMODE_IDLIST;
	Can_FilterConfig3.FilterScale = CAN_FILTERSCALE_32BIT;
	Can_FilterConfig3.SlaveStartFilterBank = 2;


	HAL_CAN_ConfigFilter(&hcan2,&Can_FilterConfig3 );

	CAN_FilterTypeDef Can_FilterConfig4;
	/*____________for Testing________________*/
	Can_FilterConfig4.FilterActivation = CAN_FILTER_ENABLE ;
	Can_FilterConfig4.FilterBank = 5;
	Can_FilterConfig4.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	Can_FilterConfig4.FilterIdHigh = (GPS2_TIME_PKT<<5);//CM_LPM_GPSINFO
	Can_FilterConfig4.FilterIdLow=0x0000;
	Can_FilterConfig4.FilterMaskIdHigh= (GPS2_TIME_PKT<<5);
	Can_FilterConfig4.FilterMaskIdLow =0x0000;
	Can_FilterConfig4.FilterMode= CAN_FILTERMODE_IDLIST;
	Can_FilterConfig4.FilterScale = CAN_FILTERSCALE_32BIT;
	Can_FilterConfig4.SlaveStartFilterBank = 2;


	HAL_CAN_ConfigFilter(&hcan2,&Can_FilterConfig4 );

	/*_______________________for Testing___________	 */


	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO1_MSG_PENDING);

	/* USER CODE END CAN2_Init 2 */

}
/* CAN3 init function */
void MX_CAN3_Init(void)
{

	/* USER CODE BEGIN CAN3_Init 0 */

	hcan3.Instance = CAN3;
	hcan3.Init.Prescaler = 5;
	hcan3.Init.Mode = CAN_MODE_NORMAL;
	hcan3.Init.SyncJumpWidth = CAN_SJW_4TQ;
	hcan3.Init.TimeSeg1 = CAN_BS1_7TQ;
	hcan3.Init.TimeSeg2 = CAN_BS2_1TQ;
	/* USER CODE END CAN3_Init 0 */

	/* USER CODE BEGIN CAN3_Init 1 */

	/* USER CODE END CAN3_Init 1 */
	hcan3.Instance = CAN3;
	hcan3.Init.Prescaler = 18;
	hcan3.Init.Mode = CAN_MODE_NORMAL;
	hcan3.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan3.Init.TimeSeg1 = CAN_BS1_2TQ;
	hcan3.Init.TimeSeg2 = CAN_BS2_2TQ;
	hcan3.Init.TimeTriggeredMode = DISABLE;
	hcan3.Init.AutoBusOff = DISABLE;
	hcan3.Init.AutoWakeUp = DISABLE;
	hcan3.Init.AutoRetransmission = ENABLE;
	hcan3.Init.ReceiveFifoLocked = DISABLE;
	hcan3.Init.TransmitFifoPriority = DISABLE;
	if (HAL_CAN_Init(&hcan3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CAN3_Init 2 */
	CAN_FilterTypeDef Can_FilterConfig3;
	Can_FilterConfig3.FilterActivation = CAN_FILTER_ENABLE ;
	Can_FilterConfig3.FilterBank = 1;
	Can_FilterConfig3.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	Can_FilterConfig3.FilterIdHigh = 0x200<<5;
	Can_FilterConfig3.FilterIdLow=0x0000;
	Can_FilterConfig3.FilterMaskIdHigh=0x200<<5;
	Can_FilterConfig3.FilterMaskIdLow =0x0000;
	Can_FilterConfig3.FilterMode= CAN_FILTERMODE_IDMASK;
	Can_FilterConfig3.FilterScale = CAN_FILTERSCALE_32BIT;
	Can_FilterConfig3.SlaveStartFilterBank = 6;

	HAL_CAN_ConfigFilter(&hcan3,&Can_FilterConfig3 );
	//
	HAL_CAN_Start(&hcan3);
	HAL_CAN_ActivateNotification(&hcan3,CAN_IT_RX_FIFO0_MSG_PENDING);
	/* USER CODE END CAN3_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(canHandle->Instance==CAN2)
	{
		/* USER CODE BEGIN CAN2_MspInit 0 */

		/* USER CODE END CAN2_MspInit 0 */
		/* CAN2 clock enable */
		__HAL_RCC_CAN2_CLK_ENABLE();
		__HAL_RCC_CAN1_CLK_ENABLE();

		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**CAN2 GPIO Configuration
    PB5     ------> CAN2_RX
    PB6     ------> CAN2_TX
		 */
		GPIO_InitStruct.Pin = CAN2_RX_Pin|CAN2_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* CAN2 interrupt Init */
		HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 4, 0);
		HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
		HAL_NVIC_SetPriority(CAN2_RX1_IRQn, 4, 1);
		HAL_NVIC_EnableIRQ(CAN2_RX1_IRQn);
		/* USER CODE BEGIN CAN2_MspInit 1 */

		/* USER CODE END CAN2_MspInit 1 */
	}
	else if(canHandle->Instance==CAN3)
	{
		/* USER CODE BEGIN CAN3_MspInit 0 */

		/* USER CODE END CAN3_MspInit 0 */
		/* CAN3 clock enable */
		__HAL_RCC_CAN3_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**CAN3 GPIO Configuration
    PA8     ------> CAN3_RX
    PA15     ------> CAN3_TX
		 */
		GPIO_InitStruct.Pin = CAN3_RX_Pin|CAN3_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF11_CAN3;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* CAN3 interrupt Init */
		HAL_NVIC_SetPriority(CAN3_RX0_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN3_RX0_IRQn);
		HAL_NVIC_SetPriority(CAN3_RX1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(CAN3_RX1_IRQn);
		/* USER CODE BEGIN CAN3_MspInit 1 */

		/* USER CODE END CAN3_MspInit 1 */
	}
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

	if(canHandle->Instance==CAN2)
	{
		/* USER CODE BEGIN CAN2_MspDeInit 0 */

		/* USER CODE END CAN2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_CAN2_CLK_DISABLE();
		__HAL_RCC_CAN1_CLK_DISABLE();

		/**CAN2 GPIO Configuration
    PB5     ------> CAN2_RX
    PB6     ------> CAN2_TX
		 */
		HAL_GPIO_DeInit(GPIOB, CAN2_RX_Pin|CAN2_TX_Pin);

		/* CAN2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
		HAL_NVIC_DisableIRQ(CAN2_RX1_IRQn);
		/* USER CODE BEGIN CAN2_MspDeInit 1 */

		/* USER CODE END CAN2_MspDeInit 1 */
	}
	else if(canHandle->Instance==CAN3)
	{
		/* USER CODE BEGIN CAN3_MspDeInit 0 */

		/* USER CODE END CAN3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_CAN3_CLK_DISABLE();

		/**CAN3 GPIO Configuration
    PA8     ------> CAN3_RX
    PA15     ------> CAN3_TX
		 */
		HAL_GPIO_DeInit(GPIOA, CAN3_RX_Pin|CAN3_TX_Pin);

		/* CAN3 interrupt Deinit */
		HAL_NVIC_DisableIRQ(CAN3_RX0_IRQn);
		HAL_NVIC_DisableIRQ(CAN3_RX1_IRQn);
		/* USER CODE BEGIN CAN3_MspDeInit 1 */

		/* USER CODE END CAN3_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

void TransmitCanPkt(const enum enumCanId Ken_idetifier)
{
	void *pp_ptr;
	uint16_t Traverser = 0;
	uint16_t uc_size = 0;



	switch(EI_data.ProtocolType)
	{

	case HITACHI:
	{
		/* _____________________________ HITACHI ________________________________________ */
		if( (CHECKFLAG(8,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
		{
			printf("Stopping CAN Transmission\n");
			return;
		}

		switch((int)Ken_idetifier)
		{
		case EI_PROCESS_DATA:
			if(ReorderFlag == RESET)
			{
				vEI_Transver_EI_To_MIE.ActualDataBytes = (EI_data.Hitachi_st.PeersCnt * MAX_OUTPUTS_PER_PEER);
				pp_ptr = &vEI_Transver_EI_To_MIE;
				uc_size =(vEI_Transver_EI_To_MIE.ActualDataBytes + sizeof(vEI_Transver_EI_To_MIE.ActualDataBytes)); /* +2 is by Considering */
				//uc_size = sizeof(EI_VITAL_DATA_TO_VITAL_COMPUTER_t);
				sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
				if(Debug_flag == 1)
				{
					printf("SENT SIGNAL DATA TO MIE\n");
				}
			}
			else
			{
				HitachiEI_DataReArranged_st.ActualDataBytes = (EI_data.Hitachi_st.PeersCnt * MAX_OUTPUTS_PER_PEER);
				pp_ptr = &HitachiEI_DataReArranged_st;
				uc_size =(HitachiEI_DataReArranged_st.ActualDataBytes + sizeof(HitachiEI_DataReArranged_st.ActualDataBytes)); /* +2 is by Considering */
				//uc_size = sizeof(EI_VITAL_DATA_TO_VITAL_COMPUTER_t);
				sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
				if(Debug_flag == 1)
				{
					printf("SENT ReArranged SIGNAL DATA TO MIE\n");
				}
			}
			break;

		case EiPC_1_HEALTH:
		case EiPC_2_HEALTH:
			pp_ptr = &Hitachi_fault_logging_st;
			uc_size = sizeof(HITACHI_FAULT_LOGGING_t);
			if(Debug_flag == 1)
			{
				printf("\nHealth 2 MIE\n");
			}
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			break;

		case EI_1_SELF_SHARING:
		case EI_2_SELF_SHARING:
			pp_ptr = &Temp_Tx_Ei_self_sharing_st;
			uc_size = sizeof(EI_SELF_SHARING_t);
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			if(Debug_flag == 1)
			{
				printf("Sent2_O:%u\n",Temp_Tx_Ei_self_sharing_st.Peer_Index);
			}
			break;

		case SEND_VITAL_MSG_1:
		case SEND_VITAL_MSG_2:
			pp_ptr = &Tx_VITAL_MSG_CAN_st;
			uc_size = sizeof(SEND_VITAL_MSG_2_OTH_t);
			if(Debug_flag == 1)
			{
				printf("\nSending V-Message 2-O\n");
			}
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			break;

		case CAN_FAULT_INJECTION:
			pp_ptr = &Hitachi_fault_injection_st;
			uc_size = sizeof(HITACHI_FAULT_INJECTION_t);
			if(Debug_flag == 1)
			{
				printf("\nSending Fault Injection data 2-O\n");
			}
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			break;

		case EIPC1_RELAYS_2_OTHER:
		case EIPC2_RELAYS_2_OTHER:
			pp_ptr = &Tx_Temp_Main_Relays_st;
			uc_size = sizeof(RELAYS_t);
			if(Debug_flag == 1)
			{
				printf("Sending Relays-O\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case DAT_CRC_EXCHANGE_1:
		case DAT_CRC_EXCHANGE_2:
			pp_ptr = &DatCrcXchangeOther_st;
			uc_size = sizeof(DAT_CRC_XCHANGE_t);
			if(Debug_flag == 1)
			{
				printf("Sending App Data CRC to Other\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;


		default:
			break;

		} /* 	switch((int)Ken_idetifier) for HITACHI 	*/
		break; /* case HITACHI: break */
	}
	/* _____________________________ HITACHI ________________________________________ */

	case SIEMENS:
	{
		/* _____________________________ SIEMENS ________________________________________ */
		switch((int)Ken_idetifier)
		{

		case EI_PROCESS_DATA:
			pp_ptr = &VitalData2MIE_st;
			//				uc_size = sizeof(VITAL_DATA_t);
			uc_size =(VitalData2MIE_st.NoofDataBytes + sizeof(VitalData2MIE_st.NoofDataBytes)); /* +2 is by Considering */

			if(Debug_flag == 1)
			{
				printf("Sending Signals 2 MIE\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case EI_1_SELF_SHARING:
		case EI_2_SELF_SHARING:
			pp_ptr = &Tx_Siemens_self_sharing_st;
			uc_size = sizeof(SIEMENS_SELF_SHARING_t);
			if(Debug_flag == 1)
			{
				printf("Sending Signals 2 MIE\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case EIPC1_RELAYS_2_OTHER:
		case EIPC2_RELAYS_2_OTHER:
			pp_ptr = &SiemensMain_Relays_st;

			uc_size = (EI_data.Siemens_st.Booloutputs % 8);
			if (uc_size != 0)
			{
				uc_size = ((EI_data.Siemens_st.Booloutputs / 8)) + 1; //	including 0th buffer
			}
			else
			{
				uc_size = (EI_data.Siemens_st.Booloutputs / 8);
			}

			SiemensMain_Relays_st.NoofDataBytes = uc_size;
			//				printf("NoofDataBytes:%u\n",SiemensMain_Relays_st.NoofDataBytes);
			uc_size = uc_size + ( (sizeof(SiemensMain_Relays_st.NoofDataBytes)) + (sizeof(SiemensMain_Relays_st.VitalCRC)) );
			//				printf("Relays2Other:%u\n",uc_size);
			if(Debug_flag == 1)
			{
				printf("Sending Relays 2 Other\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case CAN_FAULT_INJECTION:
			pp_ptr = &Siemens_fault_injection_st;
			uc_size = sizeof(SIEMENS_FAULT_INJECTION_t);
			printf("\nSending Fault Injection data 2-O\n");
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			break;

		case DAT_CRC_EXCHANGE_1:
		case DAT_CRC_EXCHANGE_2:
			pp_ptr = &DatCrcXchangeOther_st;
			uc_size = sizeof(DAT_CRC_XCHANGE_t);
			if(Debug_flag == 1)
			{
				printf("Sending App Data CRC to Other\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		default:
			break;
		}
		break; /* case SIEMENS: break */
		/* _____________________________ SIEMENS ________________________________________ */
	}


	case MEDHA:
	{

		/* _____________________________ MEDHA ________________________________________ */
		switch((int)Ken_idetifier)
		{
		case EI_PROCESS_DATA:
			pp_ptr = &Medha_Relay_Sts_st;

			uint16_t Length =  EI_data.Medha_Prms_st.NoofRelays%8;

			if(Length != 0)
			{
				Length = ((EI_data.Medha_Prms_st.NoofRelays)/8) + 1;
			}
			else
			{
				Length = ((EI_data.Medha_Prms_st.NoofRelays)/8);
			}

			Medha_Relay_Sts_st.ActualDataBytes =  Length;
			uc_size =(Length + sizeof(Length)); /* +2 is by Considering */

			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			if(Debug_flag == 1)
			{
				printf("SENT MEDHA SIGNAL DATA TO MIE:%lu\n",HAL_GetTick());
			}
			break;

		case EI_1_SELF_SHARING:
		case EI_2_SELF_SHARING:
			pp_ptr = &Medha_ei_self_sharing_TX_st;
			uc_size = sizeof(MEDHA_EI_SELF_SHARING_t);
			if(Debug_flag == 1)
			{
				printf("\nSending_2_O\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case EiPC_1_HEALTH:
		case EiPC_2_HEALTH:
			pp_ptr = &Hitachi_fault_logging_st;
			uc_size = sizeof(HITACHI_FAULT_LOGGING_t);
			if(Debug_flag == 1)
			{
				printf("\nHealth 2 MIE\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case CAN_FAULT_INJECTION:
			pp_ptr = &Medha_Fault_Injection_st;
			uc_size = sizeof(MEDHA_FAULT_INJECTION_t);
			printf("\nSending Fault Injection data 2-O\n");
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case EIPC1_RELAYS_2_OTHER:
		case EIPC2_RELAYS_2_OTHER:
			pp_ptr = &Medha_Cur_Relay_Status_st;
			uc_size = sizeof(MEDHA_CUR_RELAY_STS_t);
			if(Debug_flag == 1)
			{
				printf("\nRelay Data 2-O\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		case DAT_CRC_EXCHANGE_1:
		case DAT_CRC_EXCHANGE_2:
			pp_ptr = &DatCrcXchangeOther_st;
			uc_size = sizeof(DAT_CRC_XCHANGE_t);
			if(Debug_flag == 1)
			{
				printf("Sending App Data CRC to Other\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		default:
			break;
		}

		break;
		/* _____________________________ MEDHA ________________________________________ */

	}
	case KYOSAN:
	{
		switch((int)Ken_idetifier)
		{
		case EI_PROCESS_DATA:
			memcpy(Kyosan_can_relays_st.Relays,Kyosan_FinalRelays.Relays,MAX_KYOSAN_RELAYS);
			Kyosan_can_relays_st.No_Of_CAN_Bytes = 60;
			pp_ptr = &Kyosan_can_relays_st;
			uc_size =(sizeof(KYOSAN_CAN_RELAYS_t));
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			if(Debug_flag == 1)
			{
				printf("SENT SIGNAL DATA TO MIE\n");
				for(uint8_t i=0;i<Kyosan_can_relays_st.No_Of_CAN_Bytes;++i)
				{
					printf("%02X ",Kyosan_can_relays_st.Relays[i]);
				}
				printf("\n");
			}
			break;

		case EIPC1_RELAYS_2_OTHER:
		case EIPC2_RELAYS_2_OTHER:
			pp_ptr = &Kyosan_SelfRelays;
			uc_size = sizeof(KYOSAN_RELAYS_t);
			printf("\nSending Relays 2-O\n");
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			break;

		case CAN_FAULT_INJECTION:
			pp_ptr = &KyosanFaultInjection_st;
			uc_size = sizeof(KYOSAN_FAULT_INJECTION_t);
			printf("\nSending Fault Injection data 2-O\n");
			sendCanPkts(pp_ptr,uc_size,Ken_idetifier);
			break;

		case DAT_CRC_EXCHANGE_1:
		case DAT_CRC_EXCHANGE_2:
			pp_ptr = &DatCrcXchangeOther_st;
			uc_size = sizeof(DAT_CRC_XCHANGE_t);
			if(Debug_flag == 1)
			{
				printf("Sending App Data CRC to Other\n");
			}
			sendCanPkts(pp_ptr, uc_size, Ken_idetifier);
			break;

		default:
			break;
		}
		break;
	}

	} /* 	switch(EI_data.ProtocolType) */

	UNUSED(Traverser);
}

void sendCanPkts(void *sptr, int size,enum enumCanId canId)
{
	int32_t j = size,seqNo=0;
	char *cptr = sptr;
	while(j > 0)
	{
		if(j < 7)
		{
			CAN_transmit(canId,cptr,j+1,seqNo);
		}
		else
		{
			CAN_transmit(canId,cptr,8,seqNo);
		}
		cptr += 7;
		j-=7;
		seqNo++;
	}
}

void CAN_transmit (int can_id ,char* datPtr,int len,int seqNo)
{
	char i=0;
	uint32_t CAN_TxWaitClk = HAL_GetTick();

	//	CAN_TxMsg.id = can_id;
	TxData[0] = seqNo;
	for (i = 0; i < len-1; i++)
	{
		TxData[i+1] = *(datPtr+i);
	}
	if(can_id < (0x200) )
	{
		while(len!=0)
		{
			/* transmit message to 3,4 nodes: CAN2*/
			TxHeader2.IDE = CAN_ID_STD;
			TxHeader2.StdId=can_id;
			TxHeader2.RTR=CAN_RTR_DATA;
			TxHeader2.DLC=len;
			uint8_t txbuff=0;

			//			if(can_id == EI_PROCESS_DATA)
			//			{
			//				printf("Data sending over CAN: ");
			//				for(uint8_t i=0;i<TxHeader2.DLC;++i)
			//				{
			//					printf("%02X ",TxData[i]);
			//				}
			//				printf("\n");
			//			}
			if((Error=HAL_CAN_AddTxMessage(&hcan2, &TxHeader2, TxData, &pTxMailbox2))!=HAL_OK)
			{
				printf("CAN2 Tx Fail\n");
				return;
				//				Error_Handler();
			}
			while( (txbuff=HAL_CAN_IsTxMessagePending(&hcan2, pTxMailbox2))>=1)
			{
				if(HAL_Elaps_Ticks(CAN_TxWaitClk) > 100)
				{
					printf("CAN2 Tx Fail\n");
					return;
				}
			}

			len=0;
		}
	}
	else
	{

		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
		{
			return;
		}

		while(len!=0)
		{
			/* transmit message to 3,4 nodes: CAN2*/
			TxHeader3.IDE = CAN_ID_STD;
			TxHeader3.StdId=can_id;
			TxHeader3.RTR=CAN_RTR_DATA;
			TxHeader3.DLC=len;
			uint8_t txbuff=0;

			//			if((can_id ==  EIPC1_RELAYS_2_OTHER) || (can_id ==  EIPC2_RELAYS_2_OTHER))
			//			{
			//				printf("Data sending over CAN: ");
			//				for(uint8_t i=0;i<TxHeader2.DLC;++i)
			//				{
			//					printf("%02X ",TxData[i]);
			//				}
			//				printf("\n");
			//			}

			//printf("TxHeader1.DLC:%u\n",TxHeader1.DLC);
			if((Error=HAL_CAN_AddTxMessage(&hcan3, &TxHeader3, TxData, &pTxMailbox3))!=HAL_OK)
			{
				printf("CAN3 Tx Fail\n");
				return;
				//				Error_Handler();
			}
			while( (txbuff=HAL_CAN_IsTxMessagePending(&hcan3, pTxMailbox3))>=1)
			{
				if(HAL_Elaps_Ticks(CAN_TxWaitClk) > 100)
				{
					printf("CAN3 Tx Fail\n");
					return;
				}
			}
			len=0;
		}
	}
}

/******************Function to be invoked when we get an interrupt on CAN1*****************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN3)
	{
		//		printf("^");
		if(CANBUFFSIZE==canRead.In)
		{
			canRead.In=0;
		}
		if (HAL_CAN_GetRxMessage(&hcan3,CAN_RX_FIFO0,&RxHeader2,RxData_can3) != HAL_OK)
		{
			Error_Handler();
		}
		(canRead.CAN_msg[canRead.In]).canPort=3;
		(canRead.CAN_msg[canRead.In]).Dlc=RxHeader2.DLC;
		(canRead.CAN_msg[canRead.In]).Ide=RxHeader2.IDE;
		(canRead.CAN_msg[canRead.In]).Rtr=RxHeader2.RTR;
		(canRead.CAN_msg[canRead.In]).StdId=RxHeader2.StdId;
		//printf("stdid in Int_Handler=%x\n",RxHeader1.StdId);
		if((canRead.CAN_msg[canRead.In]).Rtr==CAN_RTR_DATA)
		{
			memcpy((canRead.CAN_msg[canRead.In]).data,RxData_can3,8);
			canRead.In++;
		}
	}
}

/******************Function to be invoked when we get an interrupt on CAN2*****************/
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//printf("Interrupt invoked for CAN2\n");
	if(CANBUFFSIZE==canRead.In)
		canRead.In=0;
	if ((Error=HAL_CAN_GetRxMessage(&hcan2,CAN_RX_FIFO1,&RxHeader1,RxData_can2)) != HAL_OK)
	{
		printf("Error in CAN2 Reception Interrupt: %d\n",Error);
		Error_Handler();
	}
	//	printf("RxHeader1.stdid=%lx \n",RxHeader1.StdId);

	(canRead.CAN_msg[canRead.In]).canPort=2;
	(canRead.CAN_msg[canRead.In]).Dlc=RxHeader1.DLC;
	(canRead.CAN_msg[canRead.In]).Ide=RxHeader1.IDE;
	(canRead.CAN_msg[canRead.In]).Rtr=RxHeader1.RTR;
	(canRead.CAN_msg[canRead.In]).StdId=RxHeader1.StdId;

	if((canRead.CAN_msg[canRead.In]).Rtr==CAN_RTR_DATA){
		//	printf("copying in to buffer on can2\n");
		memcpy((canRead.CAN_msg[canRead.In]).data,RxData_can2,8);
		canRead.In++;
	}
}


unsigned char canDataReady()
{
	unsigned int ui_Read_CAN_index  = 0;

	if(canRead.In == canRead.Out)
	{
		return 0;
	}
	/* for removing duplicates in the buffer */
	for(ui_Read_CAN_index = (canRead.Out+1);ui_Read_CAN_index<canRead.In;ui_Read_CAN_index++)
	{
		if(0 == memcmp(&canRead.CAN_msg[canRead.Out],&canRead.CAN_msg[canRead.In],sizeof(CAN_msg)))
		{
			memset(&canRead.CAN_msg[canRead.Out],0,sizeof(CAN_msg));
			canRead.Out++;
			break;
		}
	}
	return 1;
}

void readCanToStructs(void)
{
	unsigned char uc_seqNo,uc_Loop_Index = 0,uc_index = 0,uc_LocIndex = 0;
	uint8_t *uc_pp_charPtr= 0;

	/* SIEMENS */

	static uint16_t DataLength = 0;
	static uint16_t DataTraverser = 0;


	switch(EI_data.ProtocolType)
	{
	case HITACHI:
	{
		/* ___________________ HITACHI _______________ */
		if( (CHECKFLAG(7,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
		{
			return;
		}
		while(canDataReady())
		{
			uc_index = canRead.CAN_msg[canRead.Out].canPort-1;
			switch(canRead.CAN_msg[canRead.Out].StdId)
			{

			case GPS_TIME_PKT:
				uc_pp_charPtr = (uint8_t *)&Temp_Gps_date_time_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][GPS_TIME_PKT].expSeqNo)
				{
					printf("GPS Packet Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][GPS_TIME_PKT].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(GPS_DATE_TIME_t))
				{
					memcpy(&Gps_date_time_st,&Temp_Gps_date_time_st,sizeof(GPS_DATE_TIME_t));
					if(Debug_flag == 1)
					{
						printf("GPS_PACKET_RECEIVED\n");
					}
					//				printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.Hour,Gps_date_time_st.Mins,Gps_date_time_st.Secs,Gps_date_time_st.Date,Gps_date_time_st.Mont,Gps_date_time_st.Year);
					Write_RTC();
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][GPS_TIME_PKT].invalid = 1;
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo++;
				}
				break;

			case GPS2_TIME_PKT:
				uc_pp_charPtr = (uint8_t *)&Temp_Gps_date_time_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][GPS2_TIME_PKT].expSeqNo)
				{
					printf("GPS2 Packet Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][GPS2_TIME_PKT].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(GPS_DATE_TIME_t))
				{
					memcpy(&Gps_date_time_st,&Temp_Gps_date_time_st,sizeof(GPS_DATE_TIME_t));
					if(Debug_flag == 1)
					{
						printf("GPS2_PACKET_RECEIVED\n");
					}
					//				printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.Hour,Gps_date_time_st.Mins,Gps_date_time_st.Secs,Gps_date_time_st.Date,Gps_date_time_st.Mont,Gps_date_time_st.Year);
					Write_RTC();
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][GPS2_TIME_PKT].invalid = 1;
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo++;
				}
				break;

			case EI_1_SELF_SHARING:
				uc_pp_charPtr = (uint8_t *)&Temp_Rx_Ei_self_sharing_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(EI_SELF_SHARING_t));
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo)
				{
					printf("EI_1_SELF_SHARING Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(EI_SELF_SHARING_t));
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(EI_SELF_SHARING_t))
				{
					memcpy(&Rx_Ei_self_sharing_st[Temp_Rx_Ei_self_sharing_st.Peer_Index],&Temp_Rx_Ei_self_sharing_st,sizeof(EI_SELF_SHARING_t));
					if(Debug_flag == 1)
					{
						printf("O_Info1_RCV:%u\n",Temp_Rx_Ei_self_sharing_st.Peer_Index);
					}
					Is_CAN_Exchange_happening = YES;
					RCV_SELF_XCHG_CLK = HAL_GetTick();
					Check_Other_EiPC_Packet(Temp_Rx_Ei_self_sharing_st.Peer_Index);
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EI_1_SELF_SHARING].invalid = 1;
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo++;
				}
				break;

			case EI_2_SELF_SHARING:
				uc_pp_charPtr = (uint8_t *)&Temp_Rx_Ei_self_sharing_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(EI_SELF_SHARING_t));

					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo)
				{
					printf("EI_2_SELF_SHARING Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(EI_SELF_SHARING_t));
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(EI_SELF_SHARING_t))
				{
					memcpy(&Rx_Ei_self_sharing_st[Temp_Rx_Ei_self_sharing_st.Peer_Index],&Temp_Rx_Ei_self_sharing_st,sizeof(EI_SELF_SHARING_t));
					if(Debug_flag == 1)
					{
						printf("O_Info2_RCV:%u\n",Temp_Rx_Ei_self_sharing_st.Peer_Index);
					}
					RCV_SELF_XCHG_CLK = HAL_GetTick();
					Is_CAN_Exchange_happening = YES;
					Check_Other_EiPC_Packet(Temp_Rx_Ei_self_sharing_st.Peer_Index);
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EI_2_SELF_SHARING].invalid = 1;
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo++;
				}
				break;

			case SEND_VITAL_MSG_1:
				uc_pp_charPtr = (uint8_t *)&Temp_Rx_VITAL_MSG_CAN_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
					canStatus[uc_index][SEND_VITAL_MSG_1].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][SEND_VITAL_MSG_1].expSeqNo)
				{
					printf("SEND_VITAL_MSG_1 Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
					canStatus[uc_index][SEND_VITAL_MSG_1].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][SEND_VITAL_MSG_1].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(SEND_VITAL_MSG_2_OTH_t))
				{
					if(Debug_flag == 1)
					{
						printf("RCV V-Message-1\n");
					}
					memset(&Rx_VITAL_MSG_CAN_st,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
					memcpy(&Rx_VITAL_MSG_CAN_st,&Temp_Rx_VITAL_MSG_CAN_st,sizeof(SEND_VITAL_MSG_2_OTH_t));
					Is_CAN_Exchange_happening = YES;
					if(Tx_Ei_self_sharing_st[Rx_VITAL_MSG_CAN_st.Source_Peer_Index].IS_PRIMARY != RECEIVED)
					{
						Vital_Msg_extr_from_CAN(Rx_VITAL_MSG_CAN_st.VITAL_MESSAGE,Rx_VITAL_MSG_CAN_st.Packet_Length,Rx_VITAL_MSG_CAN_st.Source_Peer_Index,Rx_VITAL_MSG_CAN_st.Dest_Peer_Index);
					}
					canStatus[uc_index][SEND_VITAL_MSG_1].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][SEND_VITAL_MSG_1].invalid = 1;
					canStatus[uc_index][SEND_VITAL_MSG_1].expSeqNo++;
				}
				break;
			case SEND_VITAL_MSG_2:
				uc_pp_charPtr = (uint8_t *)&Temp_Rx_VITAL_MSG_CAN_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
					canStatus[uc_index][SEND_VITAL_MSG_2].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][SEND_VITAL_MSG_2].expSeqNo)
				{
					printf("SEND_VITAL_MSG_2 Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
					canStatus[uc_index][SEND_VITAL_MSG_2].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][SEND_VITAL_MSG_2].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(SEND_VITAL_MSG_2_OTH_t))
				{
					if(Debug_flag == 1)
					{
						printf("RCV V-Message-2\n");
					}
					memset(&Rx_VITAL_MSG_CAN_st,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
					memcpy(&Rx_VITAL_MSG_CAN_st,&Temp_Rx_VITAL_MSG_CAN_st,sizeof(SEND_VITAL_MSG_2_OTH_t));
					Is_CAN_Exchange_happening = YES;
					if(Tx_Ei_self_sharing_st[Rx_VITAL_MSG_CAN_st.Source_Peer_Index].EI_Receiving_Status != 1)
					{
						Vital_Msg_extr_from_CAN(Rx_VITAL_MSG_CAN_st.VITAL_MESSAGE,Rx_VITAL_MSG_CAN_st.Packet_Length,Rx_VITAL_MSG_CAN_st.Source_Peer_Index,Rx_VITAL_MSG_CAN_st.Dest_Peer_Index);
					}
					canStatus[uc_index][SEND_VITAL_MSG_2].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][SEND_VITAL_MSG_2].invalid = 1;
					canStatus[uc_index][SEND_VITAL_MSG_2].expSeqNo++;
				}
				break;

			case CAN_FAULT_INJECTION:
				uc_pp_charPtr = (uint8_t *)&Hitachi_fault_injection_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(HITACHI_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo)
				{
					printf("FAULT_INJECTION Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(HITACHI_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(HITACHI_FAULT_INJECTION_t))
				{
					printf("Received Fault Injection Data\n");
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][CAN_FAULT_INJECTION].invalid = 1;
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo++;
				}
				break;

			case EIPC1_RELAYS_2_OTHER:
				uc_pp_charPtr = (uint8_t *)&Rx_Temp_Main_Relays_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(RELAYS_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo)
				{
					printf("EIPC1RELAYS Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(RELAYS_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(RELAYS_t))
				{
					uint16_t CRC16 = 0;
					uint8_t Buff[40] = {0,};
					memcpy(Buff,&Rx_Temp_Main_Relays_st,sizeof(Rx_Temp_Main_Relays_st));
					CRC16 = GetCRC16(0,Buff, sizeof(Rx_Temp_Main_Relays_st));
					if(CRC16 == 0)
					{
						canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
						//					printf("\nCRC is 0\n");
					}
					else
					{
						if(Debug_flag == 1)
						{
							printf("Rcv Relays1-O:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Rx_Temp_Main_Relays_st.Peer_Index]);
						}
						memcpy(&OtherCard_Relays_st[Rx_Temp_Main_Relays_st.Peer_Index],&Rx_Temp_Main_Relays_st,sizeof(RELAYS_t));
						Is_CAN_Exchange_happening = YES;
						canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
					}
				}
				else
				{
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].invalid = 1;
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo++;
				}
				break;

			case EIPC2_RELAYS_2_OTHER:
				uc_pp_charPtr = (uint8_t *)&Rx_Temp_Main_Relays_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(RELAYS_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo)
				{
					printf("EIPC2 RELAYS Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(RELAYS_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(RELAYS_t))
				{
					uint16_t CRC16 = 0;
					uint8_t Buff[40] = {0,};
					memcpy(Buff,&Rx_Temp_Main_Relays_st,sizeof(Rx_Temp_Main_Relays_st));
					CRC16 = GetCRC16(0,Buff, sizeof(Rx_Temp_Main_Relays_st));
					if(CRC16 == 0)
					{
						canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
						//					printf("\nCRC is 0\n");
					}
					else
					{
						if(Debug_flag == 1)
						{
							printf("Rcv Relays2-O:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Rx_Temp_Main_Relays_st.Peer_Index]);
						}
						memcpy(&OtherCard_Relays_st[Rx_Temp_Main_Relays_st.Peer_Index],&Rx_Temp_Main_Relays_st,sizeof(RELAYS_t));
						Is_CAN_Exchange_happening = YES;
						canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
					}
				}
				else
				{
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].invalid = 1;
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo++;
				}
				break;


			case DAT_CRC_EXCHANGE_1:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo)
				{
					printf("EIPC1 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-1 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_2:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo)
				{
					printf("EIPC2 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-2 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo++;
				}
				break;

			default:
				break;
			}
			memset(&canRead.CAN_msg[canRead.Out],0,sizeof(CAN_msg));
			canRead.Out++;
			if(CANBUFFSIZE == canRead.Out)
				canRead.Out = 0;

		}
		/* ___________________ HITACHI _______________ */
	}
	break;


	case SIEMENS:
	{
		/* __________ SIEMENS _________ */
		while(canDataReady())
		{
			uc_index = canRead.CAN_msg[canRead.Out].canPort-1;
			switch(canRead.CAN_msg[canRead.Out].StdId)
			{
			case GPS_TIME_PKT:
				uc_pp_charPtr = (uint8_t *)&Temp_Gps_date_time_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][GPS_TIME_PKT].expSeqNo)
				{
					printf("GPS Packet Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][GPS_TIME_PKT].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(GPS_DATE_TIME_t))
				{
					memcpy(&Gps_date_time_st,&Temp_Gps_date_time_st,sizeof(GPS_DATE_TIME_t));
					if(Debug_flag == 1)
					{
						printf("GPS_PACKET_RECEIVED\n");
					}
					//				printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.Hour,Gps_date_time_st.Mins,Gps_date_time_st.Secs,Gps_date_time_st.Date,Gps_date_time_st.Mont,Gps_date_time_st.Year);
					Write_RTC();
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][GPS_TIME_PKT].invalid = 1;
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo++;
				}
				break;

			case GPS2_TIME_PKT:
				uc_pp_charPtr = (uint8_t *)&Temp_Gps_date_time_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][GPS2_TIME_PKT].expSeqNo)
				{
					printf("GPS2 Packet Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][GPS2_TIME_PKT].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(GPS_DATE_TIME_t))
				{
					memcpy(&Gps_date_time_st,&Temp_Gps_date_time_st,sizeof(GPS_DATE_TIME_t));
					if(Debug_flag == 1)
					{
						printf("GPS2_PACKET_RECEIVED\n");
					}
					//				printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.Hour,Gps_date_time_st.Mins,Gps_date_time_st.Secs,Gps_date_time_st.Date,Gps_date_time_st.Mont,Gps_date_time_st.Year);
					Write_RTC();
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][GPS2_TIME_PKT].invalid = 1;
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo++;
				}
				break;

			case EI_1_SELF_SHARING:
				uc_pp_charPtr = (uint8_t *)&Temp_Siemens_self_sharing_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(SIEMENS_SELF_SHARING_t));
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo)
				{
					printf("EI_1_SELF_SHARING Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(SIEMENS_SELF_SHARING_t));
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(SIEMENS_SELF_SHARING_t))
				{
					memcpy(&Rx_Siemens_self_sharing_st,&Temp_Siemens_self_sharing_st,sizeof(SIEMENS_SELF_SHARING_t));
					if(Debug_flag == 1)
					{
						printf("O_Info1_RCV\n");
					}
					SiemensClocks_st.SiemensCanXchangeHappening = YES;
					SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK = HAL_GetTick();
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EI_1_SELF_SHARING].invalid = 1;
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo++;
				}
				break;

			case EI_2_SELF_SHARING:
				uc_pp_charPtr = (uint8_t *)&Temp_Siemens_self_sharing_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(SIEMENS_SELF_SHARING_t));

					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo)
				{
					printf("EI_2_SELF_SHARING Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(SIEMENS_SELF_SHARING_t));
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(SIEMENS_SELF_SHARING_t))
				{
					memcpy(&Rx_Siemens_self_sharing_st,&Temp_Siemens_self_sharing_st,sizeof(SIEMENS_SELF_SHARING_t));
					if(Debug_flag == 1)
					{
						printf("O_Info2_RCV\n");
					}
					SiemensClocks_st.SiemensCanXchangeHappening = YES;
					SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK = HAL_GetTick();
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EI_2_SELF_SHARING].invalid = 1;
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo++;
				}
				break;

			case EIPC1_RELAYS_2_OTHER:
				uc_pp_charPtr = (uint8_t *)&TempSiemensRx_OtherRelays_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo != 0)
				{
					uc_pp_charPtr = uc_pp_charPtr + DataTraverser;
				}
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(VITAL_DATA_SHARING_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
					DataTraverser = 0;
					DataLength= 0;
				}
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
					DataTraverser++;
				}

				if(uc_seqNo != canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo)
				{
					printf("EIPC1_RELAYS_2_OTHER Seq no MisMatch:%u:%u\n",uc_seqNo,canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo);
					memset(uc_pp_charPtr,0,sizeof(VITAL_DATA_SHARING_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
					DataTraverser = 0;
					break;
				}
				else
				{
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo++;
				}

				//				uc_pp_charPtr += (7 * uc_seqNo);
				DataLength = ( (TempSiemensRx_OtherRelays_st.NoofDataBytes) + ((sizeof(TempSiemensRx_OtherRelays_st.NoofDataBytes))+ (sizeof(TempSiemensRx_OtherRelays_st.VitalCRC))));
				//				printf("Data Length1:%u:%u\n",DataLength,TempSiemensRx_OtherRelays_st.NoofDataBytes);

				if(DataTraverser >= DataLength)
				{
					if(Debug_flag == 1)
					{
						printf("EI1 Relay Info Received\n");
					}
					memcpy(&Siemens_OtherRelays_st,&TempSiemensRx_OtherRelays_st,sizeof(TempSiemensRx_OtherRelays_st));
					DataTraverser = 0;
					DataLength = 0;
					SiemensClocks_st.SiemensCanXchangeHappening = YES;
					SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK = HAL_GetTick();
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
				}
				break;

			case EIPC2_RELAYS_2_OTHER:
				uc_pp_charPtr = (uint8_t *)&TempSiemensRx_OtherRelays_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo != 0)
				{
					uc_pp_charPtr = uc_pp_charPtr + DataTraverser;
				}
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(VITAL_DATA_SHARING_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
					DataTraverser = 0;
					DataLength= 0;
				}
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
					DataTraverser++;
				}

				if(uc_seqNo != canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo)
				{
					printf("EIPC2_RELAYS_2_OTHER Seq no MisMatch:%u:%u\n",uc_seqNo,canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo);
					memset(uc_pp_charPtr,0,sizeof(VITAL_DATA_SHARING_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
					DataTraverser = 0;
					break;
				}
				else
				{
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo++;
				}

				//				uc_pp_charPtr += (7 * uc_seqNo);
				DataLength = ( (TempSiemensRx_OtherRelays_st.NoofDataBytes) + ((sizeof(TempSiemensRx_OtherRelays_st.NoofDataBytes))+ (sizeof(TempSiemensRx_OtherRelays_st.VitalCRC))));
				//				printf("Data Length1:%u:%u\n",DataLength,TempSiemensRx_OtherRelays_st.NoofDataBytes);

				if(DataTraverser >= DataLength)
				{
					if(Debug_flag == 1)
					{
						printf("EI2 Relay Info Received\n");
					}
					memcpy(&Siemens_OtherRelays_st,&TempSiemensRx_OtherRelays_st,sizeof(TempSiemensRx_OtherRelays_st));
					DataTraverser = 0;
					DataLength = 0;
					SiemensClocks_st.SiemensCanXchangeHappening = YES;
					SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK = HAL_GetTick();
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
				}
				break;

			case CAN_FAULT_INJECTION:
				uc_pp_charPtr = (uint8_t *)&Siemens_fault_injection_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(SIEMENS_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo)
				{
					printf("FAULT_INJECTION Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(SIEMENS_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(SIEMENS_FAULT_INJECTION_t))
				{
					printf("Received Fault Injection Data\n");
					SiemensClocks_st.SiemensCanXchangeHappening = YES;
					SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK = HAL_GetTick();
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][CAN_FAULT_INJECTION].invalid = 1;
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_1:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo)
				{
					printf("EIPC1 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-1 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_2:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo)
				{
					printf("EIPC2 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-2 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo++;
				}
				break;

			default:
				break;
			}
			memset(&canRead.CAN_msg[canRead.Out],0,sizeof(CAN_msg));
			canRead.Out++;
			if(CANBUFFSIZE == canRead.Out)
			{
				canRead.Out = 0;
			}

		}
		/* __________ SIEMENS _________ */
	}
	break;

	case MEDHA:
	{
		/* __________ MEDHA _________ */
		while(canDataReady())
		{
			uc_index = canRead.CAN_msg[canRead.Out].canPort-1;
			switch(canRead.CAN_msg[canRead.Out].StdId)
			{
			case GPS_TIME_PKT:
				uc_pp_charPtr = (uint8_t *)&Temp_Gps_date_time_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][GPS_TIME_PKT].expSeqNo)
				{
					printf("GPS Packet Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][GPS_TIME_PKT].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(GPS_DATE_TIME_t))
				{
					memcpy(&Gps_date_time_st,&Temp_Gps_date_time_st,sizeof(GPS_DATE_TIME_t));
					if(Debug_flag == 1)
					{
						printf("GPS_PACKET_RECEIVED\n");
					}
					//				printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.Hour,Gps_date_time_st.Mins,Gps_date_time_st.Secs,Gps_date_time_st.Date,Gps_date_time_st.Mont,Gps_date_time_st.Year);
					Write_RTC();
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][GPS_TIME_PKT].invalid = 1;
					canStatus[uc_index][GPS_TIME_PKT].expSeqNo++;
				}
				break;

			case GPS2_TIME_PKT:
				uc_pp_charPtr = (uint8_t *)&Temp_Gps_date_time_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][GPS2_TIME_PKT].expSeqNo)
				{
					printf("GPS2 Packet Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(GPS_DATE_TIME_t));
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][GPS2_TIME_PKT].expSeqNo) * 7)
						+ (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(GPS_DATE_TIME_t))
				{
					memcpy(&Gps_date_time_st,&Temp_Gps_date_time_st,sizeof(GPS_DATE_TIME_t));
					if(Debug_flag == 1)
					{
						printf("GPS2_PACKET_RECEIVED\n");
					}
					//				printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.Hour,Gps_date_time_st.Mins,Gps_date_time_st.Secs,Gps_date_time_st.Date,Gps_date_time_st.Mont,Gps_date_time_st.Year);
					Write_RTC();
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][GPS2_TIME_PKT].invalid = 1;
					canStatus[uc_index][GPS2_TIME_PKT].expSeqNo++;
				}
				break;

			case EI_1_SELF_SHARING:

				uc_pp_charPtr = (uint8_t *)&Medha_ei_self_sharing_Temp_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if (uc_seqNo == 0)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_EI_SELF_SHARING_t));
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
				}
				if (uc_seqNo != canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo)
				{
					printf("Seq No Mismatch for EI_1_SELF_SHARING\n");
					printf("Rcv:%u|Exp:%u\n",uc_seqNo,canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo);
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_EI_SELF_SHARING_t));
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if ((((canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(MEDHA_EI_SELF_SHARING_t))
				{
					if(Debug_flag == 1)
					{
						printf("\nRCV OTH_INFO1\n");
					}
					memcpy(&Medha_ei_self_sharing_RX_st, &Medha_ei_self_sharing_Temp_st, sizeof(MEDHA_EI_SELF_SHARING_t));
					Medha_CAN_Xchanging_Happening = SET;
					Last_Received_Medha_Self_sharing = HAL_GetTick();
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EI_1_SELF_SHARING].invalid = 1;
					canStatus[uc_index][EI_1_SELF_SHARING].expSeqNo++;
				}
				break;

			case EI_2_SELF_SHARING:

				uc_pp_charPtr = (uint8_t *)&Medha_ei_self_sharing_Temp_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if (uc_seqNo == 0)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_EI_SELF_SHARING_t));
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
				}
				if (uc_seqNo != canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo)
				{
					printf("Seq No Mismatch for EI_2_SELF_SHARING\n");
					printf("Rcv:%u|Exp:%u\n",uc_seqNo,canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo);
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_EI_SELF_SHARING_t));
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if ((((canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(MEDHA_EI_SELF_SHARING_t))
				{
					if(Debug_flag == 1)
					{
						printf("\nRCV OTH_INFO2\n");
					}
					memcpy(&Medha_ei_self_sharing_RX_st, &Medha_ei_self_sharing_Temp_st, sizeof(MEDHA_EI_SELF_SHARING_t));
					Medha_CAN_Xchanging_Happening = SET;
					Last_Received_Medha_Self_sharing = HAL_GetTick();
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EI_2_SELF_SHARING].invalid = 1;
					canStatus[uc_index][EI_2_SELF_SHARING].expSeqNo++;
				}
				break;

			case CAN_FAULT_INJECTION:

				uc_pp_charPtr = (uint8_t *)&Medha_Fault_Injection_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if (uc_seqNo == 0)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				if (uc_seqNo != canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if ((((canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(MEDHA_FAULT_INJECTION_t))
				{
					printf("Received CAN_FAULT_INJECTION\n");
					Last_Received_Medha_Self_sharing = HAL_GetTick();
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][CAN_FAULT_INJECTION].invalid = 1;
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo++;
				}
				break;

			case EIPC1_RELAYS_2_OTHER:

				uc_pp_charPtr = (uint8_t *)&Medha_CAN_Relay_Status_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if (uc_seqNo == 0)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_CUR_RELAY_STS_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
				}
				if (uc_seqNo != canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_CUR_RELAY_STS_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if ((((canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(MEDHA_CUR_RELAY_STS_t))
				{
					if(Debug_flag == 1)
					{
						printf("\nRCV EIPC1_RELAYS_2_OTHER\n");
					}
					Received_CAN_Relay_Status = YES;
					Medha_CAN_Xchanging_Happening = SET;
					Last_Received_Medha_Self_sharing = HAL_GetTick();
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].invalid = 1;
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo++;
				}
				break;

			case EIPC2_RELAYS_2_OTHER:

				uc_pp_charPtr = (uint8_t *)&Medha_CAN_Relay_Status_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if (uc_seqNo == 0)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_CUR_RELAY_STS_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
				}
				if (uc_seqNo != canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo)
				{
					memset(uc_pp_charPtr, 0, sizeof(MEDHA_CUR_RELAY_STS_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if ((((canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(MEDHA_CUR_RELAY_STS_t))
				{
					if(Debug_flag == 1)
					{
						printf("\nRCV RELAY_DATA_EI2\n");
					}
					Medha_CAN_Xchanging_Happening = SET;
					Last_Received_Medha_Self_sharing = HAL_GetTick();
					Received_CAN_Relay_Status = YES;
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].invalid = 1;
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_1:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo)
				{
					printf("EIPC1 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-1 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_2:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo)
				{
					printf("EIPC2 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-2 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo++;
				}
				break;

			default:
				break;
			}
			memset(&canRead.CAN_msg[canRead.Out],0,sizeof(CAN_msg));
			canRead.Out++;
			if(CANBUFFSIZE == canRead.Out)
				canRead.Out = 0;
		}
		/* __________ MEDHA _________ */
	}
	break;

	case KYOSAN:
	{
		/* __________ KYOSAN _________ */
		while(canDataReady())
		{
			uc_index = canRead.CAN_msg[canRead.Out].canPort-1;
			switch(canRead.CAN_msg[canRead.Out].StdId)
			{

			case EIPC1_RELAYS_2_OTHER:
				uc_pp_charPtr = (uint8_t *)&Kyosan_CANRelays;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(KYOSAN_RELAYS_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo)
				{
					printf("EIPC1RELAYS Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(KYOSAN_RELAYS_t));
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(KYOSAN_RELAYS_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv Relays1-O:%u\n",Kyosan_CANRelays.Index);
					}
					memcpy(&Kyosan_OtherRelays,&Kyosan_CANRelays,sizeof(KYOSAN_RELAYS_t));
					LastCANReceived_Clk_Kyosan = HAL_GetTick();
					Is_CAN_Exchange_happening = SET;
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].invalid = 1;
					canStatus[uc_index][EIPC1_RELAYS_2_OTHER].expSeqNo++;
				}
				break;

			case EIPC2_RELAYS_2_OTHER:
				uc_pp_charPtr = (uint8_t *)&Kyosan_CANRelays;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(KYOSAN_RELAYS_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo)
				{
					printf("EIPC2 RELAYS Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(KYOSAN_RELAYS_t));
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(KYOSAN_RELAYS_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv Relays2-O:%u\n",Kyosan_CANRelays.Index);
					}
					memcpy(&Kyosan_OtherRelays,&Kyosan_CANRelays,sizeof(KYOSAN_RELAYS_t));
					LastCANReceived_Clk_Kyosan = HAL_GetTick();
					Is_CAN_Exchange_happening = SET;
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].invalid = 1;
					canStatus[uc_index][EIPC2_RELAYS_2_OTHER].expSeqNo++;
				}
				break;

			case CAN_FAULT_INJECTION:

				uc_pp_charPtr = (uint8_t *)&KyosanFaultInjection_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if (uc_seqNo == 0)
				{
					memset(uc_pp_charPtr, 0, sizeof(KYOSAN_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				if (uc_seqNo != canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo)
				{
					memset(uc_pp_charPtr, 0, sizeof(KYOSAN_FAULT_INJECTION_t));
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if ((((canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(KYOSAN_FAULT_INJECTION_t))
				{
					printf("Received CAN_FAULT_INJECTION\n");
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][CAN_FAULT_INJECTION].invalid = 1;
					canStatus[uc_index][CAN_FAULT_INJECTION].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_1:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo)
				{
					printf("EIPC1 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-1 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_1].expSeqNo++;
				}
				break;

			case DAT_CRC_EXCHANGE_2:
				uc_pp_charPtr = (uint8_t *)&DatCrcXchangeOther_st;
				uc_seqNo = canRead.CAN_msg[canRead.Out].data[0];
				if(uc_seqNo==0)
				{
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				if(uc_seqNo != canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo)
				{
					printf("EIPC2 DAT_CRC_EXCHANGE Seq no MisMatch\n");
					memset(uc_pp_charPtr,0,sizeof(DAT_CRC_XCHANGE_t));
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
					break;
				}
				uc_pp_charPtr += (7 * uc_seqNo);
				for (uc_Loop_Index = 1; uc_Loop_Index < canRead.CAN_msg[canRead.Out].Dlc; uc_Loop_Index++)
				{
					*uc_pp_charPtr = canRead.CAN_msg[canRead.Out].data[uc_Loop_Index];
					uc_pp_charPtr++;
				}
				if((((canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo) * 7) + (canRead.CAN_msg[canRead.Out].Dlc - 1)) >= sizeof(DAT_CRC_XCHANGE_t))
				{
					if(Debug_flag == 1)
					{
						printf("Rcv-2 AppCRC:%04X\n",DatCrcXchangeOther_st.App_CRC);
					}
					Is_CAN_Exchange_happening = YES;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo = 0;
				}
				else
				{
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].invalid = 1;
					canStatus[uc_index][DAT_CRC_EXCHANGE_2].expSeqNo++;
				}
				break;

			default:
				break;
			}

			memset(&canRead.CAN_msg[canRead.Out],0,sizeof(CAN_msg));
			canRead.Out++;
			if(CANBUFFSIZE == canRead.Out)
				canRead.Out = 0;
			/* Protocol Type */
		}
	}
	default:
		break;
		/* Protocol Type */
	}
	UNUSED(uc_Loop_Index);
	UNUSED(uc_LocIndex);
}


/* USER CODE END 1 */
