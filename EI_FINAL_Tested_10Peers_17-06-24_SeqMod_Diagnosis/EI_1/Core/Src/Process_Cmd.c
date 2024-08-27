/*
 * Process_Cmd.c
 *
 *  Created on: May 10, 2023
 *      Author: Ravi Teja P
 */


#include "Headers.h"


unsigned char CheckForRqst(unsigned char uc_port)
{
	unsigned char buffer[10] = "\0";//index=0,data=0;
	if(ReadAndFormMsg(uc_port, buffer,sizeof(buffer)) > 0)
	{
		if( (buffer[1] == ((DATLOADING>>8)&0XFF)) && (buffer[2] ==(DATLOADING & 0XFF)) && (buffer[3] == DATREQUEST) )
		{
			printf("\nUPLOADING REQUEST OK\n");
			SendAck(uc_port,DATLOADING, ACKOK);
			gucReqstCurTim = HAL_GetTick();
			return 0x01;
		}
		else if( (buffer[1] == ((DATSENDING>>8)&0XFF)) && (buffer[2] ==(DATSENDING & 0XFF)) && (buffer[3] == DATREQUEST) )
		{
			printf("\nDOWNLOADING REQUEST OK\n");
			SendAck(uc_port,DATSENDING, ACKOK);
			gucReqstCurTim = HAL_GetTick();
			return 0x02;
		}
		else
			printf("\nREQUEST NOT OK\n");
	}
	return 0x00;
}



short int ReadAndFormMsg(unsigned short int us_Port,unsigned char *puc_ReadMsg,unsigned short int us_MaxSize)
{
	static unsigned char suc_RcvStatus = ONE;
	unsigned char suc_RByte = ZERO;
	static unsigned short int j = ZERO;
	short int sus_CRC = ZERO;
	unsigned short int us_ReturnBytes = ZERO;
	//	static unsigned long updateClk = 0;
	//static unsigned short int index = 0;

	/* If data available on port read and store data */
	//	if(SpoiElapsTICs(updateClk) > 5000 )
	//		*UpdateFlag = 0;


	while(Com_DataReady(us_Port))
	{
		gucUpdateFlag = 1;
		// updateClk = SpoiGetSysTICs();

		HAL_IWDG_Refresh(&hiwdg);
		suc_RByte = Com_Get_Byte(us_Port);

		//Delay_SRAM(1000);

		//printf("%x|* ",suc_RByte);
		Enable_UART_Interrupts();
		//#ifdef PRINTF_DEBUG
		//printf("%d:%x ",index++,suc_RByte);
		//SpoiPause(1);
		//index++	   ;
		//#endif /*PRINTF_DEBUG*/

		switch(suc_RcvStatus)
		{
		case ONE: /* Receive Start of Message */
			if(START_OF_MESSAGE == suc_RByte)
			{
				puc_ReadMsg[ZERO] = suc_RByte;
				j = ONE;
				suc_RcvStatus = TWO;
			}
			break;
		case TWO: /* Receive body of message or end of message */
			switch(suc_RByte)
			{
			case END_OF_MESSAGE:

				sus_CRC = j-2;
				us_ReturnBytes = j;
				j = ZERO;

				if (sus_CRC < 0)
					return 0;

				suc_RcvStatus = ONE;

				//printf(" SDM Check sum IRDMI: [%d]%x [%d]%x \n",siNoBytes,ucIrdmiBuff[siNoBytes],(siNoBytes+1), ucIrdmiBuff[siNoBytes + 1]);

				printf("\n SDM Check sum IRDMI: [%d]%x [%d]%x",sus_CRC,
						puc_ReadMsg[sus_CRC],
						(sus_CRC + 1),
						puc_ReadMsg[sus_CRC+1]);

				// if (CommCheckCRC(puc_ReadMsg,sus_CRC))
				//if(CheckIntegrity(puc_ReadMsg,sus_CRC, CRCHK))
				if(CheckCRC(puc_ReadMsg, sus_CRC))
				{
					//printf("Actual: %s ",puc_ReadMsg);
					printf("CRC OK [%x] [%x]\n",puc_ReadMsg[sus_CRC], puc_ReadMsg[sus_CRC+1]);
					//puc_ReadMsg[i+1] = END_OF_MESSAGE;

					return us_ReturnBytes;
				}
				else
				{
					printf("\nCRC NOK ");
					printf("[%x] [%x]\n",puc_ReadMsg[sus_CRC], puc_ReadMsg[sus_CRC+1]);
					//printf("CRC Failed [%d]\n", iNrepBytes);
					return 0;
					//return us_ReturnBytes;
				}
#if 0
				if (CheckCS)
				{
					printf("CRC OK\n");
					return us_ReturnBytes;
				}
				else
				{
					printf("CRC NOK\n");
					return ZERO;
				}
#endif	/* #if 0*/

			case START_OF_MESSAGE:
				puc_ReadMsg[ZERO] = suc_RByte;
				j = ONE;
				suc_RcvStatus = TWO;
				break;

			case DLE:
				suc_RcvStatus = THREE;
				break;

			default:
				puc_ReadMsg[j] = suc_RByte;
				j++;
				break;
			}
			break;

			case THREE:

				/* Receive body of message specially if $ or & are part of body
				 *  i.e characters escaped with DLE */

				if((START_OF_MESSAGE == suc_RByte)
						|| (END_OF_MESSAGE == suc_RByte)
						|| (DLE == suc_RByte))
				{
					puc_ReadMsg[j] = suc_RByte;
					j++;
				}
				suc_RcvStatus = TWO;
				break;

			default:
				break;
		}    /* End of switch*/

		/* If receiving invalid message, ZERO message buffer */

		if( j >= us_MaxSize)
		{
			suc_RcvStatus = ONE;
			j = ZERO;
		}
	}	    /* End of While data ready */

	return ZERO;
}


void SendAck(unsigned char uc_port, unsigned char Opcode, unsigned char ACK)
{
	uint32_t AckTim=0;
	unsigned char puc_Message[10] = {0,};

	puc_Message[0]= '$';
	puc_Message[1]=((unsigned char ) (Opcode >> 8) & 0xff);
	puc_Message[2]= (unsigned char ) (Opcode & 0xff);
	puc_Message[3]= ACK;

	ComputeCRC(puc_Message, 4);
	puc_Message[6]= '&';
	printf("\nSending Ack CRC [%x][%x]\n",puc_Message[4],puc_Message[5]);

	AckTim=HAL_GetTick();

	while( HAL_Elaps_Ticks(AckTim) < 1500)
	{
		HAL_IWDG_Refresh(&hiwdg);
	}

	CommTransmitMessage(uc_port,puc_Message, 6);

	AckTim = 0;
}

short int ReadAndFormConfig(unsigned short int us_Port,
		unsigned char*puc_ReadMsg,unsigned short int us_MaxSize)
{
	static unsigned char suc_RcvStatus = ONE;
	unsigned char suc_RByte = ZERO;
	static unsigned short int i = ZERO;
	short int sus_CRC = ZERO;
	unsigned short int us_ReturnBytes = ZERO;
	//	unsigned short us_CrcStart = 2;

	//static unsigned short int index = 0;

	/* If data available on port read and store data */
	//	if(SpoiElapsTICs(updateClk) > 5000 )
	//		*UpdateFlag = 0;


	while(Com_DataReady(us_Port))
	{
		// *UpdateFlag = 1;
		// updateClk = SpoiGetSysTICs();


		suc_RByte = Com_Get_Byte(us_Port);
		Enable_UART_Interrupts();

		HAL_IWDG_Refresh(&hiwdg);
		//	Delay_SRAM(1000);
		//printf("DaTA\n");
		//printf("%x ",suc_RByte);
		switch(suc_RcvStatus)
		{
		case ONE: /* Receive Start of Message */
			if(START_OF_MESSAGE == suc_RByte)
			{
				puc_ReadMsg[ZERO] = suc_RByte;
				i = ONE;
				suc_RcvStatus = TWO;
			}
			break;
		case TWO: /* Receive body of message or end of message */
			switch(suc_RByte)
			{
			case END_OF_MESSAGE:
				sus_CRC = i-2;
				us_ReturnBytes = i;
				//printf("us_ReturnBytes: %d\n",us_ReturnBytes);
				i = ZERO;

				if (sus_CRC < 0)
					return 0;

				suc_RcvStatus = ONE;

				//printf(" SDM Check sum IRDMI: [%d]%x [%d]%x \n",siNoBytes,ucIrdmiBuff[siNoBytes],(siNoBytes+1), ucIrdmiBuff[siNoBytes + 1]);

				//printf("\n SDM Check sum IRDMI: [%d]%x [%d]%x",sus_CRC,
				//													puc_ReadMsg[sus_CRC],
				//													(sus_CRC + 1),
				//													puc_ReadMsg[sus_CRC+1]);

				// if (CommCheckCRC(puc_ReadMsg,sus_CRC))
				//if(CheckIntegrity(puc_ReadMsg,sus_CRC, CRCHK))
				if(CheckCRC(puc_ReadMsg, sus_CRC))
				{
					//printf("Actual: %s ",puc_ReadMsg);
					printf("CRC OK [%x][%x]\n",puc_ReadMsg[sus_CRC], puc_ReadMsg[sus_CRC+1]);
					return us_ReturnBytes;
				}
				else
				{
					printf("\nCRC NOK ");
					printf("[%x][%x]\n",puc_ReadMsg[sus_CRC], puc_ReadMsg[sus_CRC+1]);
					//printf("CRC Failed [%d]\n", iNrepBytes);
					return 0;
					//return us_ReturnBytes;
				}

			case START_OF_MESSAGE:
				puc_ReadMsg[ZERO] = suc_RByte;
				i = ONE;
				suc_RcvStatus = TWO;
				break;

			case DLE:
				suc_RcvStatus = THREE;
				break;

			default:
				puc_ReadMsg[i] = suc_RByte;
				i++;
				break;
			}
			break;

			case THREE:

				/* Receive body of message specially if $ or & are part of body
				 *  i.e characters escaped with DLE */

				if((START_OF_MESSAGE == suc_RByte)
						|| (END_OF_MESSAGE == suc_RByte)
						|| (DLE == suc_RByte))
				{
					puc_ReadMsg[i] = suc_RByte;
					i++;
				}
				suc_RcvStatus = TWO;
				break;

			default:
				break;
		}    /* End of switch*/

		/* If receiving invalid message, ZERO message buffer */

		if( i >= us_MaxSize)
		{
			suc_RcvStatus = ONE;
			i = ZERO;
		}
	}	    /* End of While data ready */

	return ZERO;
}


unsigned char CheckDocklightForRqst(unsigned char uc_port)
{

	unsigned char RqstBuff[10]="\0";//index=0,data=0;
	if(CommReadAndFormMessage(uc_port, RqstBuff,10)>0)
	{
		switch (RqstBuff[1])
		{
		case ONE:
		case TWO:
			if(RqstBuff[1] == 1)
			{
				printf("\nPlease Load \"EIPC DATS\"\n");
				return 0x01;
			}
			break;

		default:
			printf("\n\t\t Received:%d Please send VALID Command from ISP port",RqstBuff[1]);
			return 0;
		}
		return RqstBuff[1];
	}
	return 0x00;
}


unsigned short int CommReadAndFormMessage(unsigned short int us_Port,
		unsigned char *puc_ReadMsg,unsigned short int us_MaxSize)
{
	static unsigned char suc_RcvStatus = ONE;
	static unsigned char suc_RByte = ZERO;
	static unsigned short int i = ZERO;
	static unsigned short int sus_CRC = ZERO;
	unsigned short int us_ReturnBytes = ZERO;

	/* If data available on port read and store data */

	while(Com_DataReady(us_Port))
	{
		suc_RByte = Com_Get_Byte(us_Port);
		//printf(" 0x%02x ",suc_RByte);
		switch(suc_RcvStatus)
		{
		case ONE: /* Receive Start of Message */
			if(START_OF_MESSAGE == suc_RByte)
			{
				puc_ReadMsg[ZERO] = suc_RByte;
				i = ONE;
				suc_RcvStatus = TWO;
			}
			break;
		case TWO: /* Receive body of message or end of message */
			switch(suc_RByte)
			{
			case END_OF_MESSAGE:
				sus_CRC = i-TWO;
				us_ReturnBytes = i;
				i = ZERO;
				suc_RcvStatus = ONE;
				if(us_ReturnBytes <= FOUR)
				{
					return ZERO;
				}
				if (CommCheckCRC(puc_ReadMsg,sus_CRC))
				{
					//printf("CRC OK\n");
					return us_ReturnBytes;
				}
				else
				{
					//printf("CRC NOK\n");
					return ZERO;
				}

			case START_OF_MESSAGE:
				puc_ReadMsg[ZERO] = suc_RByte;
				i = ONE;
				suc_RcvStatus = TWO;
				break;

			case DLE:
				suc_RcvStatus = THREE;
				break;

			default:
				puc_ReadMsg[i] = suc_RByte;
				i++;
				break;
			}
			break;

			case THREE:

				/* Receive body of message specially if $ or & are part of body
				 *  i.e characters escaped with DLE */

				if(START_OF_MESSAGE == suc_RByte || END_OF_MESSAGE == suc_RByte
						|| DLE == suc_RByte)
				{
					puc_ReadMsg[i] = suc_RByte;
					i++;
				}
				suc_RcvStatus = TWO;
				break;

			default:
				break;
		}    /* End of switch*/

		/* If receiving invalid message, ZERO message buffer */

		if( i >= us_MaxSize)
		{
			suc_RcvStatus = ONE;
			i = ZERO;
		}
	}	    /* End of While data ready */

	return ZERO;
}	/* End of CommReadAndFormMessage */


