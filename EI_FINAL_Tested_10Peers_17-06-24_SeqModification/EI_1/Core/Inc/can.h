/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.h
 * @brief   This file contains all the function prototypes for
 *          the can.c file
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
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "Headers.h"
/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan2;

extern CAN_HandleTypeDef hcan3;

/* USER CODE BEGIN Private defines */
#define CANBUFFSIZE		1024


typedef struct
{
	uint32_t StdId;       /* 11  bit identifier */
	uint8_t  data[8];     /* Data field */
	uint8_t  Dlc;         /* Length of data field in bytes */
	uint8_t  Ide;         /* STANDARD, or EXTENDED IDENTIFIER */
	uint8_t  Rtr;         /* DATA FRAME, or - REMOTE FRAME */
	uint8_t canPort;
} CAN_msg;

enum enumCanId
{
	GPS_TIME_PKT=0x103, 		/* FROM MIE3 */
	GPS2_TIME_PKT=0x109,		/* FROM MIE4 */
	EI_PROCESS_DATA=0x119, 		/* TO MIE 1 & 2*/
	EiPC_1_HEALTH=0x11A, 		/* TO MIE 1 & 2*/
	EiPC_2_HEALTH=0x11B, 		/* TO MIE 1 & 2*/
	EI_1_SELF_SHARING=0x201,	/* EIPC SHARING */
	EI_2_SELF_SHARING=0x202,	/* EIPC SHARING */
	SEND_VITAL_MSG_1=0x203,		/* EIPC SHARING */
	SEND_VITAL_MSG_2=0x204,		/* EIPC SHARING */
	CAN_FAULT_INJECTION=0x205,	/* EIPC SHARING */
	LAST_RCV_MSGS_DIFF_2=0x206,	/* EIPC SHARING */
	EIPC1_RELAYS_2_OTHER=0x207,	/* EIPC SHARING */
	EIPC2_RELAYS_2_OTHER=0x208,	/* EIPC SHARING */
	DAT_CRC_EXCHANGE_1 = 0x209,	/* EIPC SHARING */
	DAT_CRC_EXCHANGE_2 = 0x20A,	/* EIPC SHARING */
	MAXCANID
};




typedef struct
{
	unsigned char invalid;
	unsigned char expSeqNo;
	//	unsigned char recStatus;

}canSynch;


typedef struct
{
	CAN_msg CAN_msg[CANBUFFSIZE];
	unsigned short In;
	unsigned short Out;
}canReadStruct;


typedef struct
{
	unsigned char canId;
	unsigned char canData[8];
}canBuf;


extern CAN_msg       CAN_TxMsg;      /* CAN messge for sending */
extern canSynch canStatus[2][MAXCANID];
extern canReadStruct canRead;
extern CAN_TxHeaderTypeDef TxHeader2,TxHeader3;
extern uint32_t pTxMailbox2,pTxMailbox3;
extern CAN_RxHeaderTypeDef RxHeader0,RxHeader1;
extern uint8_t RxData_can1[8];
extern uint8_t RxData_can2[8];
extern uint8_t RxData_can3[8];
extern uint8_t TxData[8];
extern uint8_t Error;







/* USER CODE END Private defines */

void MX_CAN2_Init(void);
void MX_CAN3_Init(void);

/* USER CODE BEGIN Prototypes */

unsigned char canDataReady(void);
void readCanToStructs(void);
void TransmitCanPkt(enum enumCanId);

void sendCanPkts(void * ,int, enum enumCanId );
void CAN_transmit (int can_id ,char* datPtr,int len,int seqNo);
void * getCanData(enum enumCanId idetifier);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

