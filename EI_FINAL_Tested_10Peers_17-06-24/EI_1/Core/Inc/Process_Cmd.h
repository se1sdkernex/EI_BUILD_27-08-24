/*
 * Process_Cmd.h
 *
 *  Created on: May 10, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_PROCESS_CMD_H_
#define INC_PROCESS_CMD_H_

#include "Headers.h"


unsigned char CheckForRqst(unsigned char uc_port);
short int ReadAndFormMsg(unsigned short int us_Port,unsigned char *puc_ReadMsg,unsigned short int us_MaxSize);
void SendAck(unsigned char uc_port, unsigned char Opcode, unsigned char ACK);

short int ReadAndFormConfig(unsigned short int us_Port,
                    unsigned char*puc_ReadMsg,unsigned short int us_MaxSize);
unsigned char CheckDocklightForRqst(unsigned char uc_port);
unsigned short int CommReadAndFormMessage(unsigned short int us_Port,
                    unsigned char *puc_ReadMsg,unsigned short int us_MaxSize);


#endif /* INC_PROCESS_CMD_H_ */
