/*
 * Medha.h
 *
 *  Created on: Oct 6, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_MEDHA_H_
#define INC_MEDHA_H_

#include "Headers.h"


#define LAB_TESTING 0

#define BCD(x)				((x/10)*16)+(x%10)
#define ROTATE_BYTE(c)		((uint8_t)(c<<1)|(c>>7))

#define MISMATCH_CNT_FAIL		0
#define CAN_EXCHANGE_FAIL		1



#define FAULT_INJECTION_ENABLED	0





/* FAULT INJECTION BITS #DEFINES */
#define DEST_ADDR_FI		0
#define SOURCE_ADDR_FI		1
#define NO_OF_RELAYS_FI		2
#define CHANNEL_TYPE_FI		3
#define CONTROL_BYTE_FI		4
#define EI_HEALTH_FAIL_FI	5
#define	EIPC_RX_FAIL_FI		6
#define	CMPLMT_FAIL_FI		7
#define VITAL_CRC_FAIL_FI	8
#define MSG_CRC_FAIL_FI		9
/* FAULT INJECTION BITS #DEFINES */



#define OK							1
#define NOT_OK						0
#define RECEIVED					1
#define NOT_RECEIVED				0


#define SELF_NRCV_OTHER_NRCV		0b00 	/* Both Cards Not Receiving */
#define SELF_RCV_OTHER_NRCV			0b01	/* Self Receiving-Other Not Receiving */
#define SELF_NRCV_OTHER_RCV			0b10	/* Self Not Receiving-Other Receiving */
#define SELF_RCV_OTHER_RCV			0b11	/* Self Receiving-Other Receiving */


#define SOM_MEDHA					0x53
#define CRC_FOUR      				0x04
#define CONTROL_BYTE_RX 			0xE1
#define CONTROL_BYTE_TX 			0xE2
#define HEALTH_OK					0x11
#define CHANNEL_A					0xAA
#define CHANNEL_B					0xBB

#pragma pack(4)

typedef struct
{
	uint8_t  Comport;
	uint8_t	 Max_Mismatch_value;
	uint16_t Source_Addr;
	uint16_t Dest_Addr;
	uint16_t Packet_Timeout_Count;
	uint16_t NoofRelays;
	uint32_t Packet_Interval_ms;
	uint32_t BaudRate;
	uint32_t SendSignal2MIE_CLK;
	uint32_t CAN_Xchange_Time;
	uint32_t CAN_Xchange_Tout;

}MEDHA_PRMS_t;


typedef struct
{
	uint8_t			SOM;
	uint16_t  		Source_Addr;
	uint16_t 		Dest_Addr;
	uint8_t 		Channel_Type;
	uint8_t			Control_Byte;
	uint16_t 		Length;
	uint8_t 		Packet_Number; //Sequence Number.
	uint8_t 		Reserved1[7];
	uint8_t			Health_Status;
	uint16_t 		NoofRelays;
	uint8_t 		RelayStatus[128];
	uint8_t 		Comp_RelayStatus[128];
	uint32_t		Vital_CRC;
	uint8_t 		Reserved2[2];
	uint32_t		Message_CRC;

}MEDHA_EI_PKT_t;

typedef struct
{
	uint16_t ActualDataBytes;
	uint8_t Relay_Sts[48];

}MEDHA_RELAY_STS_t;


typedef struct
{
	uint8_t  Clear_Relay_Statuses;
	uint32_t Pkt_Rcv_clk;
	uint32_t Relay_sending_clk;


}MEDHA_FAULT_HANDLING_t;

typedef struct
{
	uint32_t FaultBits;

}MEDHA_FAULT_INJECTION_t;

typedef struct
{
	uint8_t	 Receiving_Status;
	uint8_t MisMatch_CNT;
	uint32_t Vital_CRC;
}MEDHA_EI_SELF_SHARING_t;

typedef struct
{
	uint8_t 	Relay_Status[128];
	uint32_t	Vital_CRC;
}MEDHA_CUR_RELAY_STS_t;


extern uint8_t 					EIPC_1_2_STATUS;
extern uint8_t					Medha_CAN_Xchanging_Happening;
extern uint8_t					Received_CAN_Relay_Status;
//extern uint16_t					EI_Dat_CRC;
//extern uint16_t					Build_CRC;
extern uint32_t					Last_Received_Medha_Self_sharing;

extern uint8_t					CAN_Relay_Status[128];
extern uint8_t 					Current_Relay_Status[128];


extern MEDHA_EI_PKT_t			Medha_EI_Rx_Pkt_st;
extern MEDHA_RELAY_STS_t 		Medha_Relay_Sts_st;
extern MEDHA_FAULT_HANDLING_t 	Medha_Fault_Handler_st;
extern MEDHA_FAULT_INJECTION_t	Medha_Fault_Injection_st;
extern MEDHA_EI_SELF_SHARING_t	Medha_ei_self_sharing_TX_st,Medha_ei_self_sharing_RX_st,Medha_ei_self_sharing_Temp_st;
extern MEDHA_CUR_RELAY_STS_t	Medha_Cur_Relay_Status_st,Medha_CAN_Relay_Status_st,Medha_Temp_relay_sts_t;


void Process_Medha_EI (void);
void Medha_EI_Analyser(void);
uint16_t Get_Medha_EI_Message(uint8_t Comport,uint8_t *Buffer,uint16_t Buffer_Size);
void Send_Medha_EI_Reply(uint16_t Outputs);


void Send_Relay_Statuses_2_MIE(void);
void Process_Clear_Relays(void);
void Print_Medha_Self_Details(void);
void Print_Medha_Self_Details(void);
#if FAULT_INJECTION_ENABLED
void Get_Command(void);
#endif

void Send_Data_to_Other(void);
void Process_Data_from_Other(void);
void CheckforMedhaCMD(void);
void Update_MedhaCommands(void);
uint8_t Is_MedhaCommand_Valid(uint8_t *Buffer,uint8_t Length);




// ___________________________________ CRC _____________________________________
uint32_t GetCRC32(uint16_t us_Start,uint8_t *ucPtr,uint16_t us_End);
uint32_t CheckCRC32(uint16_t us_Start,uint8_t *ucPtr,uint16_t us_End);
// ___________________________________ CRC _____________________________________


#endif /* INC_MEDHA_H_ */
