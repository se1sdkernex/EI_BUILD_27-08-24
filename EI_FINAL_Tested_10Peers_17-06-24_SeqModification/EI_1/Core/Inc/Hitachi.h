/*
 * Hitachi.h
 *
 *  Created on: May 15, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_HITACHI_H_
#define INC_HITACHI_H_

#include "Headers.h"

#pragma pack(4)

#define ENABLE_FAULT_INJECTION			1
#define ENABLE_SIGNAL_PRINT				0


#define MAX_LAST_ACT_COUNT				4

enum INTERFACE_TYPES
{
	ETHERNET =1,
	SERIAL=2
};

enum CONNECTION_STATE
{
	BOTHCARD_LINK_NOK,
	OTHERCARD_LINK_OK,
	THISCARD_LINK_OK,
	BOTHCARD_LINK_OK,
};

//#define ETHERNET						1
//#define SERIAL							2

#if ENABLE_SIGNAL_PRINT
#define GETPINHITACHI(x)	(vEI_Transver_EI_To_MIE.Inputs[((x-1)/8)] & (1 << ((x-1)%8)))
#endif

#define HITACHI_SOM						0XF4
#define	HITACHI_EOM						0XF6
#define MAX_PEER_CONNECTIONS			10
#define MAX_OUTPUTS_PER_PEER			16
#define SELF_PEER_ADDRESS				411
#define SELF_PEER_ADDRESS_2				410
#define INIT_MSG_SIZE					0x17
#define EIPC_PCOL_VER					0x0001
#define EIPC_EXE_VER_1					0x33e232af
#define EIPC_EXE_VER_2					0x0000
#define EIPC_APP_COMP_STAMP				0X0000
#define EIPC_EXE_IDEN_CRC				0X0000
#define EIPC_APP_IDEN_CRC				0X0000
#define V_NV_FLAG						0X01
#define SIZE_BOOL_VAL_DATA				0X01
#define SIZE_NUMER_VAL_DATA				0X00
#define VAL_NUM_OUTPUTS					0X00

#define RECEIVED						1
#define NOT_RECEIVED					0

#define PRIMARY							1
#define SECONDARY						0

#define	HITACHI_MAX_PAYLOAD				128
#define MAX_VDATA_LENGTH				64

#define XMT								0
#define RCV								1

#define AVAILABLE						1
#define NOT_AVAILABLE					0

#define TX_SIZE_BOOL_OPS				0x01
#define TX_NUM_VAL_OPS					0X0
#define	TX_BOOL_VAL_STATE				0X01


// ------------------- MESSAGE TYPES ------------------------
#define INIT_MSG 							1
#define VDATAMSG 							2
#define NVDATAMSG 						3
#define STATUSMSG 						4
#define SERVICEMSG 						6

// ------------------- MESSAGE TYPES ------------------------


// ------------------- MESSAGE FLAGS ------------------------

#define ACK_MESSAGE						0
#define NACK_MESSAGE					1
#define ACK_REQUIRED					2
#define LINK_ESTABLISHED			3
#define BROADCAST							4
#define TIME_STAMP						5
#define TIME_STAMP_RESET			6
#define INIT_REQUEST					7
#define OPTIONAL_HEADER				8


// ------------------- MESSAGE FLAGS ------------------------

//-------------------- FAULT_LOGGING_BIT POSTION ------------------------

#define	Link_Status								0
#define	Receiving_From_A						1
#define	Receiving_From_B						2
#define	App_CRC_MisMatch						3
#define	EiPC_Primary							4
#define LAST_RECV_LOGGING_TIMEOUT_ELAPSED		5
#define INIT_RECEIVED							6
#define VITAL_MSG_RECEIVED						7
#define INIT_REQ_SENT							8

//________________________	TIMEOUTS	_________________________

//#define CAN_EXCHANGE_TIME					FIVE_HUNDRED_MS
//#define CAN_EXCHANGE_TIME_OUT 				FIVE_SEC
//#define COMM_FAILURE_TIME_OUT				TWENTY_SEC // FIFTEEN_SEC//EIGHT_SEC //FIFTEEN_SEC
//#define MIE_SIGNAL_SEND_CLK					TWO_SEC
//#define RCV_STATUS_FAIL_TIME_OUT			TWENTY_SEC //EIGHT_SEC //(FIVE_SEC + FIVE_HUNDRED_MS) //FIFTEEN_SEC
//#define HEALTH_PACKET_TIME					TWENTY_SEC
//#define STATUS_MSG_TIMEOUT					TEN_SEC
//#define LAST_RECV_LOGGING_TIMEOUT		 	SIX_SEC
//#define STALE_DATA_TIME_OUT					(TEN_SEC + ONE_SEC)

//________________________	TIMEOUTS	_________________________

#define SELF_ID 0x1234
#define B2D(x) (x - 6 * (x >> 4)) /*TO CONVERT BCD TO DECIMAL */
#define D2B(x) (((x / 10) << 4) | (x % 10))

//{
//	uint8_t PeersCnt;
//	uint8_t No_of_IP_s;
//	uint8_t SerialPortStartIndex;
//	uint8_t ClockMaster[MAX_PEER_CONNECTIONS];
//	uint8_t Interface_Type[MAX_PEER_CONNECTIONS];
//	uint8_t Self_IP[EIPC_CARDS][IP_LENGTH];
//	uint8_t Self_MAC[EIPC_CARDS][MAC_LENGTH];
//	uint8_t Source_IP[EIPC_CARDS][MAX_NO_IP][IP_LENGTH];
//	uint8_t Bool_outputs[MAX_PEER_CONNECTIONS];
//	uint16_t Self_Listening_Port[EIPC_CARDS];
//	uint16_t PORTS[EIPC_CARDS][MAX_NO_IP][PORTS_CNT_PER_IP];
//	uint32_t SOURCE_ADDRESSES[MAX_PEER_CONNECTIONS];
//	uint32_t DEST_ADDRESSES[MAX_PEER_CONNECTIONS];
//	uint16_t App_Identity_CRC[MAX_PEER_CONNECTIONS];
//	uint32_t Baudrate;
//	uint32_t CAN_Xchange_Time;
//	uint32_t CAN_EXCHANGE_TIME_OUT;
//	uint32_t MIE_SIGNAL_SEND_CLK;
//	uint32_t HEALTH_PACKET_TIME;
//	uint32_t COMM_FAILURE_TIME_OUT[MAX_PEER_CONNECTIONS]; /* Communication Failure Means No Data Received for Certain Time */
//	uint32_t STALE_DATA_TIME_OUT[MAX_PEER_CONNECTIONS]; /*	This clock will be used to check for Sending Init Message if no Packet reception */
//	uint32_t LinkEstablishTimeout[MAX_PEER_CONNECTIONS]; /* This clock will be checked for Status Messages Elapsed or Not. */

typedef struct
{
	uint8_t PeersCnt;
	uint8_t No_of_IP_s;
	uint8_t EnableSerialPort;
	uint8_t ClockMaster[MAX_PEER_CONNECTIONS];
	uint8_t Interface_Type[MAX_PEER_CONNECTIONS];
	uint8_t Self_IP[EIPC_CARDS][IP_LENGTH];
	uint8_t Self_MAC[EIPC_CARDS][MAC_LENGTH];
	uint8_t Source_IP[EIPC_CARDS][MAX_NO_IP][IP_LENGTH];
	uint8_t Bool_outputs[MAX_PEER_CONNECTIONS];
	uint16_t Self_Listening_Port[EIPC_CARDS];
	uint16_t PORTS[EIPC_CARDS][MAX_NO_IP][PORTS_CNT_PER_IP];
	uint32_t SOURCE_ADDRESSES[MAX_PEER_CONNECTIONS];
	uint32_t DEST_ADDRESSES[MAX_PEER_CONNECTIONS];
	uint16_t App_Identity_CRC[EIPC_CARDS][MAX_PEER_CONNECTIONS];
	uint32_t BaudRate;
	uint32_t CAN_Xchange_Time;
	uint32_t CAN_EXCHANGE_TIME_OUT;
	uint32_t MIE_SIGNAL_SEND_CLK;
	uint32_t HEALTH_PACKET_TIME;
	uint32_t COMM_FAILURE_TIME_OUT[MAX_PEER_CONNECTIONS]; /* Communication Failure Means No Data Received for Certain Time */
	uint32_t STALE_DATA_TIME_OUT[MAX_PEER_CONNECTIONS]; /*	This clock will be used to check for Sending Init Message if no Packet reception */
	uint32_t LinkEstablishTimeout[MAX_PEER_CONNECTIONS]; /* This clock will be checked for Status Messages Elapsed or Not. */
}HITACHI_t;

typedef struct
{
	uint8_t  Peer_Index;
	uint8_t  Msg_Rcv_Flag;
	uint8_t  LinkEstablishFlag;
	uint8_t  EI_Receiving_Status;
	uint8_t  IS_PRIMARY;
	uint8_t	 Last_Recv_Msg_Type;
	uint16_t VITAL_DATA_CRC;
	uint32_t Rx_Vital_Init_Val;
	uint32_t Tx_Vital_Init_Val;
	uint32_t Last_RCV_Clk_diff;
	uint32_t Last_Recv_CRC;
}EI_SELF_SHARING_t;


typedef struct
{
	uint16_t Source_Peer_Index;
	uint16_t Dest_Peer_Index;
	uint16_t Packet_Length;
	uint8_t VITAL_MESSAGE[MAX_VDATA_LENGTH];
}SEND_VITAL_MSG_2_OTH_t;

typedef struct
{
	uint8_t hh, mm, ss, DD, MM, YY;
} HITACHI_TIME_t;

typedef struct
{
	uint8_t Seq_Init_Value;
	uint8_t Tx_Seq;
	uint8_t Rx_Seq;
	uint8_t Size_of_VITAL_DATA;
	uint16_t Msg_Status_Flag;
	uint16_t Tx_Msg_Status_Flag;
	uint32_t Rx_Vital_CRC_Init_Value;
	uint32_t Tx_Vital_CRC_Init_Value;
} HITACHI_PARAMETERS_t;

typedef struct
{
	uint8_t Tx_Seq;
	uint8_t Rx_Seq;
	uint8_t Msg_Type;
	uint8_t Init_Size;
	uint8_t Exe_Ver[6];
	uint8_t V_NV_Flag;
	uint8_t Sz_Num_Val_Data;
	uint8_t Val_Bool_Ops;
	uint8_t Val_Num_Ops;
	uint16_t Src_Addr;
	uint16_t Des_Addr;
	uint16_t Protocol_Ver;
	uint16_t App_comp;
	uint16_t Exe_Idty_CRC;
	uint16_t App_Idty_CRC;
	uint32_t Init_CRC;
} HITACHI_INIT_t;

typedef struct
{
	uint8_t Tx_Seq;
	uint8_t Rx_Seq;
	uint8_t Msg_Type;
	uint16_t Src_Addr;
	uint16_t Des_Addr;
	uint32_t Service_CRC;
} HITACHI_SERVICE_t;

typedef struct
{
	uint8_t Tx_Seq;
	uint8_t Rx_Seq;
	uint8_t Msg_Type;
	uint16_t Src_Addr;
	uint16_t Des_Addr;
	uint32_t Status_CRC;
} HITACHI_STATUS_t;

typedef struct
{
	uint8_t Tx_Seq;
	uint8_t Rx_Seq;
	uint8_t Msg_Type;
	uint8_t No_Outputs;
	uint8_t Size_Num_val;
	uint8_t Outputs[MAX_OUTPUTS_PER_PEER];
	uint8_t Num_Outputs[MAX_OUTPUTS_PER_PEER];
	uint16_t Src_Addr;
	uint16_t Des_Addr;
	uint32_t VITAL_CRC;
} HITACHI_VITAL_t;

typedef struct // EI_packet_Rx_struct
{
	uint32_t FAULT_INJECTION_BITS;
	uint8_t FOR_WHICH_PEER_INDEX;
}HITACHI_FAULT_INJECTION_t;

typedef struct
{
	uint16_t ActualDataBytes;
	uint8_t Inputs[MAX_OUTPUTS_PER_PEER * MAX_PEER_CONNECTIONS];
}EI_VITAL_DATA_TO_VITAL_COMPUTER_t;

typedef struct
{
	uint8_t No_of_Peers;
	uint8_t CAN_Receiving;
	uint16_t Fault_Bits[MAX_PEER_CONNECTIONS];
}HITACHI_FAULT_LOGGING_t;


typedef struct
{
	uint8_t Last_Rcv_vdata[MAX_VDATA_LENGTH];
	uint16_t Length;
}LAST_VDATA_t;

typedef struct
{
	//This structure is just used to print Received data after
	//Replying to that Packet.
	uint8_t	Print_Buff[HITACHI_MAX_PAYLOAD];
	uint8_t Size;
}RX_Print_t;


typedef struct
{
	uint8_t flag[MAX_PEER_CONNECTIONS];
	uint32_t Clk[MAX_PEER_CONNECTIONS];
}Send_Vital_MSG_t;

typedef struct
{
	uint8_t Peer_Index;
	uint8_t Link_EstablishFlag;
	uint32_t Link_EstablishClk;
	uint32_t Link_FailClk;
}HITACHI_LINK_CLKS_FLAG_t;

typedef struct
{
	uint8_t Peer_Index;
	uint8_t RelayIPS_Buff[MAX_OUTPUTS_PER_PEER];
	uint8_t No_ofIps; /* No of Inputs in Bytes */
	uint16_t RelayIPS_CRC;
}RELAYS_t;



extern uint8_t 	Self_Seq_Init_Value[MAX_PEER_CONNECTIONS];
extern uint32_t Last_Recevied_Clock[MAX_PEER_CONNECTIONS];
extern uint32_t Last_CAN_Rcv_CLK_For_VDATA[MAX_PEER_CONNECTIONS];
extern uint32_t Link_Establish_Clock[MAX_PEER_CONNECTIONS];
extern uint32_t OtherCardLink_Establish_Clock[MAX_PEER_CONNECTIONS];
extern uint8_t	SelfLinkFlag[MAX_PEER_CONNECTIONS];
extern uint8_t	OtherCardLink_Flag[MAX_PEER_CONNECTIONS];
extern uint8_t	App_Identity_CRC_Fail[MAX_PEER_CONNECTIONS];
extern uint8_t	IP_Source_Peer[MAX_PEER_CONNECTIONS];
extern uint32_t RCV_SELF_XCHG_CLK;
extern uint32_t Interrupt_clock;
extern uint8_t 	Is_CAN_Exchange_happening;
extern uint8_t 	Rx_Seq_failure;
extern uint32_t FAULT_INJECTION_BITS;
extern uint8_t 	FOR_WHICH_PEER_INDEX;
extern ip_addr_t Recv_IP_List[MAX_PEER_CONNECTIONS];
extern uint8_t	Recv_IP_Index[MAX_PEER_CONNECTIONS];
extern uint8_t	SEND_FORCE_REPLY[MAX_PEER_CONNECTIONS];
extern uint8_t	Change_MLK_TIME[MAX_PEER_CONNECTIONS];
extern uint8_t 	Is_GPS_Packet_Received;
extern uint8_t	Tx_Peer_Index_To_SignalReq;
extern uint8_t 	Rx_Peer_Index_To_SignalReq;

extern uint8_t ReorderFlag;

extern EI_VITAL_DATA_TO_VITAL_COMPUTER_t 	vEI_Transver_EI_To_MIE;
extern EI_VITAL_DATA_TO_VITAL_COMPUTER_t 	HitachiEI_DataReArranged_st;
extern HITACHI_TIME_t 						HITACHI_time_Stamp_st[MAX_PEER_CONNECTIONS];
extern HITACHI_PARAMETERS_t 				Hitachi_prms_st[MAX_PEER_CONNECTIONS];
extern HITACHI_INIT_t 						Hitachi_Init_st[MAX_PEER_CONNECTIONS];
extern HITACHI_VITAL_t						Hitachi_Vital_st[MAX_PEER_CONNECTIONS];
extern HITACHI_SERVICE_t					Hitachi_Service_st[MAX_PEER_CONNECTIONS];
extern HITACHI_STATUS_t						Hitachi_Status_st[MAX_PEER_CONNECTIONS];
//extern EI_SELF_SHARING_t					Tx_Ei_self_sharing_st,Rx_Ei_self_sharing_st,Temp_Rx_Ei_self_sharing_st;
extern EI_SELF_SHARING_t					Tx_Ei_self_sharing_st[MAX_PEER_CONNECTIONS];
extern EI_SELF_SHARING_t					Rx_Ei_self_sharing_st[MAX_PEER_CONNECTIONS];
extern EI_SELF_SHARING_t 					Temp_Tx_Ei_self_sharing_st;
extern EI_SELF_SHARING_t 					Temp_Rx_Ei_self_sharing_st;
extern SEND_VITAL_MSG_2_OTH_t				Tx_VITAL_MSG_CAN_st,Rx_VITAL_MSG_CAN_st,Temp_Rx_VITAL_MSG_CAN_st;
extern HITACHI_FAULT_INJECTION_t			Hitachi_fault_injection_st;
extern HITACHI_FAULT_LOGGING_t				Hitachi_fault_logging_st,Prev_Hitachi_fault_logging_st;
extern LAST_VDATA_t							Last_Rcv_Vdata_st[MAX_PEER_CONNECTIONS];
extern RX_Print_t							Rx_Print_st;
extern Send_Vital_MSG_t						Send_Vital_MSG_st;
extern HITACHI_LINK_CLKS_FLAG_t				Hitachi_clk_flags_st[MAX_PEER_CONNECTIONS];
extern HITACHI_LINK_CLKS_FLAG_t				Other_Hitachi_clk_flags_st[MAX_PEER_CONNECTIONS];
extern HITACHI_LINK_CLKS_FLAG_t				Tx_Temp_clk_flags_st;
extern HITACHI_LINK_CLKS_FLAG_t				Rx_Temp_clk_flags_st;
extern RELAYS_t								Relays_st[MAX_PEER_CONNECTIONS];
extern RELAYS_t								Main_Relays_st[MAX_PEER_CONNECTIONS];
extern RELAYS_t								OtherCard_Relays_st[MAX_PEER_CONNECTIONS];
extern RELAYS_t								Tx_Temp_Main_Relays_st,Rx_Temp_Main_Relays_st;



void EI_Data_Analyzer(unsigned char *, unsigned short int);
void Init_Msg_extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index);
void Vital_Msg_extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index);
void Vital_Msg_extr_from_CAN(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index);
void Status_Msg_Extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index);
void Service_Msg_Extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index);

void Send_Service_Signal_Message(uint8_t Source_Index,uint8_t Des_Index);
void Send_Vital_Data_Message(uint8_t Source_Index,uint8_t Des_Index);
void Send_Status_Message(uint8_t Source_Index,uint8_t Des_Index);
void Process_Sending_Reply(uint8_t *Transmit_Buff, uint8_t Length,uint8_t Source_Index);
void Debug_Status_Flag(uint16_t Msg_sts_flag,uint8_t Message_Type);
void Send_Init_Msg(uint8_t Source_Index,uint8_t Des_Index);

void Check_Other_EiPC_Packet(uint8_t Source_Index);
void Send_Self_Sharing(void);
void CheckLinkEstablishTimeout(void);
void Detect_CAN_Xchange_Failure(void);
void Check_Other_EI_Packet(void);
void Process_Send_Signals_2_MIE(void);
void Send_Health_Status(void);
void Print_EI_DATA(uint8_t *Msg,uint8_t Msg_Status,uint8_t Length);
void Display(char *Data);
void print_self_Details_Hitachi(void);
void Check_Primary_r_Secondary(uint8_t Source_Index);
void Check_Vital_CRC(uint8_t Source_Index);
void Send_Init_After_Timeout(void);
void Send_Vital_Msg_After_Rx_VITAL(void);
void Check_All_Peers(void);
void CheckOtherLinkStatus(uint8_t SourceIndex);

void Convert_Data_format(uint8_t *Data,uint8_t Length);
void Process_Hitachi_Packets_Eth(void);
void Process_Hitachi_Packets_Com(void);
void Check_Recv_Status_fail(void);
void SendRelay2_Other(void);

uint8_t IS_MLK_EIPC_TIME_MATCHING(uint8_t Source_Index);
void Check_for_RelayMisMatch(void);


void Process_HitachiEI(void);

#if ENABLE_SIGNAL_PRINT
void UpdateSIGNALSNamesHitachi(void);
void PrintSignalsHitachi(void);
#endif


void ReAssignData(void);
void ReArrangedVitalData(void);
#endif /* INC_HITACHI_H_ */
