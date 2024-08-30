
#ifndef INC_KYOSAN_H_
#define INC_KYOSAN_H_


#include "Headers.h"


#define GETPIN_Kyosan(x)	(Kyosan_FinalRelays.Relays[((x-1)/8)] & (1 << ((x-1)%8)))

#pragma pack(4)


#define KYOSAN_CRC_POLYNOMIAL 	0x1021
#define KYOSAN_CRC_INIT_VAL 	0x0000

#define ACTUAL_CONTROL_DATA	0x01
#define COMPLIMENT_CONTROL_DATA 0xFE

#define FRAME_SIZE 				230     // Size of each frame
#define MAX_FRAMES 				4       // Maximum number of frames
//#define CONTROL_DATA			60
#define MAIN_DATA_BYTES			4
#define MAINTENANCE_BYTES		2
#define POLLING_CRC_BYTE		64
#define MAX_CRC_FAIL_CNT 		10
#define POS_DATA_MISMATCH	1
#define NEG_DATA_MISMATCH	2
#define KYOSAN_BAUD	31200
#define KYOSAN_PORT	3

#define MAX_KYOSAN_RELAYS	60
#define MAX_RELAY_MISMATCH_CNT	10

typedef enum
{
  CONTROL_DATA = 1U,
  POLLING_DATA
} KYOSAN_DATA;

typedef enum
{
	CRC_FAIL_CONTROL_DATA_POS,
	CRC_FAIL_CONTROL_DATA_NEG,
	CRC_FAIL_POLLING_DATA_POS,
	CRC_FAIL_POLLING_DATA_NEG,
	COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA,
}KYOSAN_FAULT_CMDS;

typedef struct
{
	uint8_t Index;
	uint8_t No_ofFrames;
	uint16_t MaxMisMatchCnt;
	uint16_t Bool_outputs;
	uint32_t Baudrate;
	uint32_t MIE_SEND_CLK;
	uint32_t Comm_Fail_Time;
	uint32_t CAN_Fail_Clk;
}KYOSAN_t;

typedef struct
{
	uint8_t Index;
	uint8_t Message_Receiving;
	uint8_t Relays[MAX_KYOSAN_RELAYS];
	uint16_t Relay_CRC;
}KYOSAN_RELAYS_t;


typedef struct
{
	uint16_t No_Of_CAN_Bytes;
	uint8_t Relays[MAX_KYOSAN_RELAYS];

}KYOSAN_CAN_RELAYS_t;

typedef struct
{
	uint32_t MessageReceivedCLK;
	uint32_t ControlDataPosRecvCLK;
	uint32_t ControlDataNegRecvCLK;
	uint32_t PollingDataPosRecvCLK;
	uint32_t PollingDataNegRecvCLK;

}KYOSAN_CLK_t;


typedef struct
{
	uint32_t FAULT_INJECTION_BITS;
	uint8_t EnableDisableFlag;
}KYOSAN_FAULT_INJECTION_t;

extern KYOSAN_FAULT_INJECTION_t KyosanFaultInjection_st;
extern KYOSAN_CLK_t KyosanClocks;
extern KYOSAN_RELAYS_t Kyosan_SelfRelays,Kyosan_OtherRelays,Kyosan_CANRelays,Kyosan_FinalRelays;
extern KYOSAN_CAN_RELAYS_t Kyosan_can_relays_st;

extern uint32_t MIE_SignalsSending_Clk_Kyosan;
extern uint32_t LastCANReceived_Clk_Kyosan;

#ifdef EI_1
extern uint8_t SendThisCardRelays;
#endif


/*___________Declarations for Kyosan EI_____________*/

void Process_Kyosan_EI(void);
void Get_Kyosan_EI_Message(uint8_t);
void Process_Send_Signals_2_MIE_Kyosan(void);
void DetectCanXchangeFailure(void);
void Process_ComFailTimeout(void);
void SendKyosanRelays2Other(void);
uint16_t Calc_Kysoan_CRC(uint8_t *, size_t);

void Update_KyosanCommands(void);
uint8_t Is_KyosanCommand_Valid(uint8_t *Buffer,uint8_t Length);
void Check_forKyosanCMD(void);

void Update_KyosanStructure(void);
void UpdateSIGNALSNamesKyosan(void);
void PrintSignalsKyosan(void);

/*___________Declarations for Kyosan EI_____________*/


#endif /* INC_KYOSAN_H_ */
