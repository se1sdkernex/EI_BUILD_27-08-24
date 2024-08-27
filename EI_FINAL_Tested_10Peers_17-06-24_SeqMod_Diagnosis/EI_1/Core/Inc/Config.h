/*
 * Config.h
 *
 *  Created on: May 10, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#include "Headers.h"


extern uint8_t Slot;
extern uint8_t Dats_Request_Flag;
extern uint8_t Debug_flag;


#pragma pack(4)



typedef struct
{
	uint8_t ProtocolType;
	uint8_t STN_NAME[30];
	HITACHI_t Hitachi_st;
	SIEMENS_t Siemens_st;
	MEDHA_PRMS_t Medha_Prms_st;
	KYOSAN_t Kyosan_st;

}EI_t;

typedef struct
{
	uint16_t App_CRC;
}DAT_CRC_XCHANGE_t;


extern EI_t	*EI_Config_ptr;
extern EI_t EI_data;
extern EI_t EI_BeforeReorderData;

extern DAT_CRC_XCHANGE_t DatCrcXchangeSelf_st,DatCrcXchangeOther_st;

extern uint8_t Config_Buff[1024];
extern uint8_t gucFlagUpdate;
extern uint8_t gucUpdateFlag;
extern uint8_t UpdateFlag;
extern uint32_t gucReqstCurTim;


void Get_Slot(void);
void Check_for_cmd(void);
void Update_HitachiCommands(void);
uint8_t Is_Command_Valid(uint8_t *Buffer,uint8_t Length);
void CheckEIPC_DAT_CRC(void);
#endif /* INC_CONFIG_H_ */
