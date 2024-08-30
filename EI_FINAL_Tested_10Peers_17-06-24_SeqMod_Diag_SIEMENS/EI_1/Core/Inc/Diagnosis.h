/*
 * Diagnosis.h
 *
 *  Created on: 27-Jun-2024
 *      Author: Software10
 */

#ifndef INC_DIAGNOSIS_H_
#define INC_DIAGNOSIS_H_



#include "Headers.h"

#define FUN_CODE 0x03
#define DBYT_CNT 0x1C


typedef struct
{
	unsigned char Online_Diag_Buf[8];
}Online_Diag_CAN;


extern unsigned int sui_failure_determine_clock ;
extern unsigned char Online_diag_flag[6];
extern Online_Diag_CAN Online_Diag_var;
extern unsigned char Online_diag_flag[6]; //31



void Check_OnlineDiag_OFC_Modem(void);
void Check_Online_Dignostic_RIU(void);
void process_to_comReceived(void);
unsigned short int CommReadAndFormMessage_Cmd(unsigned short int us_Port,
        unsigned char *puc_ReadMsg,unsigned short int us_MaxSize);
unsigned char Cal_Crc_fun(unsigned char * Buffer, unsigned short Length);
unsigned short ModBus_CRC16( unsigned char * Buffer, unsigned short Length );

#endif /* INC_DIAGNOSIS_H_ */
