/*
 * Diagnosis.c
 *
 *  Created on: 27-Jun-2024
 *      Author: Software10
 */


#include "Headers.h"

Online_Diag_CAN Online_Diag_var;
unsigned int ui_Online_DigChkTics;
unsigned char suc_ReceivedMessage[4000],Failure_check_flag = 0;
unsigned int sui_failure_determine_clock = 0;
unsigned int ofc_flag1 =0;
unsigned char Online_diag_flag[6]; //31


unsigned char CRC_Table_Hi[] = {
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
		0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
		0x40
};
/*CRC table for calculate with polynom 0xA001 with init value 0xFFFF, Low half word*/
unsigned char CRC_Table_Lo[] = {
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
		0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
		0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
		0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
		0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
		0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
		0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
		0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
		0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
		0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
		0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
		0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
		0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
		0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
		0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
		0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
		0x40
};


void Check_OnlineDiag_OFC_Modem(void)
{
	static unsigned int ui_Online_DigChkTics;
	if(HAL_Elaps_Ticks(ui_Online_DigChkTics) > (SEVEN*ONE_SEC))           // wait for 30 Sec
	{
		Check_Online_Dignostic_RIU();                                //Checking the online dignostic status of network of OFC modem
		ui_Online_DigChkTics = HAL_GetTick();
	}
}

void Check_Online_Dignostic_RIU(void)
{
	//static unsigned char CAN_Trans_flag,temp1,temp2,temp3,Failure_check_flag,Flag = 4,suc_Hold_Flag =1;
	static unsigned char CAN_Trans_flag,temp1,temp2,temp3,Flag = 4,suc_Hold_Flag =1;
	unsigned char index,insert_index = 0,check_index = 0;
	for(index = 0;index< 31; index++)
	{
		if(Online_diag_flag[index] == 0)
		{
			InsertBits(insert_index,2, &Online_Diag_var.Online_Diag_Buf[check_index],0);
		}
		if(Online_diag_flag[index] == 1)
		{
			InsertBits(insert_index,2, &Online_Diag_var.Online_Diag_Buf[check_index],1);
		}
		if(Online_diag_flag[index] == 2)
		{
			InsertBits(insert_index,2, &Online_Diag_var.Online_Diag_Buf[check_index],2);
		}
		if(Online_diag_flag[index] == 3)
		{
			InsertBits(insert_index,2, &Online_Diag_var.Online_Diag_Buf[check_index],3);

		}
		if(HAL_Elaps_Ticks(sui_failure_determine_clock) > (FIFTEEN*ONE_SEC))
		{
			if(Online_diag_flag[index] > 0)
			{
				InsertBits(insert_index,2, &Online_Diag_var.Online_Diag_Buf[check_index],3);
				ofc_flag1 = 1;
				//	ofc_flag1 = 1;
				printf("##############################################Both fiber link failure ocuired$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

			}
		}
		insert_index =	insert_index + 2;
		if(insert_index == 8)
		{
			insert_index = 0;
			check_index++;
		}
		if((check_index == 7)&&(insert_index == 6))
		{
			break;
		}
	}
	for(index = 0;index< 31; index++)
	{
		if((Online_diag_flag[index] > 0)&&(Online_diag_flag[index] < 4))
		{
			temp1 = index;
			break;
		}
	}
	temp2 = temp1;
	temp3 = index;

	if(temp2 != temp3)
	{
		printf("\n***********************FIBER LINK OK******************************\n");

		//	ofc_flag2 =0;
		suc_Hold_Flag = 1;
		Failure_check_flag++;
		if(ofc_flag1 > 4)
		{
			ofc_flag1 =0;
			Failure_check_flag =4;
			printf("\n****************************ASSIGNED***********************\n");
			/*
			switch(FIVE)
			{
			case	FIVE:
				transCanPkt(ONDIG_TO_MIE5_LPM_1_2);
				break;
			case	SIX:
				transCanPkt(ONDIG_TO_MIE6_LPM_1_2);
				break;
			case	SEVEN:
				transCanPkt(ONDIG_TO_MIE7_LPM_1_2);
				break;
			case	EIGHT:
				transCanPkt(ONDIG_TO_MIE8_LPM_1_2);
				break;
			case	NINE:
				transCanPkt(ONDIG_TO_MIE9_LPM_1_2);
				break;
			case	TEN:
				transCanPkt(ONDIG_TO_MIE10_LPM_1_2);
				break;
			case	ELEVEN:
				transCanPkt(ONDIG_TO_MIE11_LPM_1_2);
				break;
			case	TWELVE:
				transCanPkt(ONDIG_TO_MIE12_LPM_1_2);
				break;
			case	THIRTEEN:
				transCanPkt(ONDIG_TO_MIE13_LPM_1_2);
				break;
			case FOURTEEN:
				transCanPkt(ONDIG_TO_MIE14_LPM_1_2);
				break;
			case	FIFTEEN:
				transCanPkt(ONDIG_TO_MIE15_LPM_1_2);
				break;
			case	SIXTEEN:
				transCanPkt(ONDIG_TO_MIE16_LPM_1_2);
				break;
			default:
				printf("ERROR                  \t");

			}
			 */
		}

	}
	else
		Failure_check_flag = 0;
	if(Failure_check_flag == 250)
	{
		Failure_check_flag = 6;
	}

	if(temp2 == temp3)
	{
		CAN_Trans_flag++;
		printf("\n************************FIBER LINK FAIL*****************************\n");
		//		Online_diag_flag[index] = 3; //raviteja

		if((4 == Flag)&&(1 == suc_Hold_Flag))
		{
			if(HAL_Elaps_Ticks(sui_failure_determine_clock) > (12*ONE_SEC))
			{
				suc_Hold_Flag = 0;
				printf("\n Sending the packet to MIE 1 and MIE 2");

				//					for(i=0; i<8; i++)
				//			{
				//
				//			printf("\n11111111111111Online_Diag_var.Online_Diag_Buf[%d]=%d",i,Online_Diag_var.Online_Diag_Buf[i]);
				//			}
				/*
				switch(FIVE)
				{
				case	FIVE:
					transCanPkt(ONDIG_TO_MIE5_LPM_1_2);
					break;
				case	SIX:
					transCanPkt(ONDIG_TO_MIE6_LPM_1_2);
					break;
				case	SEVEN:
					transCanPkt(ONDIG_TO_MIE7_LPM_1_2);
					break;
				case	EIGHT:
					transCanPkt(ONDIG_TO_MIE8_LPM_1_2);
					break;
				case	NINE:
					transCanPkt(ONDIG_TO_MIE9_LPM_1_2);
					break;
				case	TEN:
					transCanPkt(ONDIG_TO_MIE10_LPM_1_2);
					break;
				case	ELEVEN:
					transCanPkt(ONDIG_TO_MIE11_LPM_1_2);
					break;
				case	TWELVE:
					transCanPkt(ONDIG_TO_MIE12_LPM_1_2);
					break;
				case	THIRTEEN:
					transCanPkt(ONDIG_TO_MIE13_LPM_1_2);
					break;
				case FOURTEEN:
					transCanPkt(ONDIG_TO_MIE14_LPM_1_2);
					break;
				case	FIFTEEN:
					transCanPkt(ONDIG_TO_MIE15_LPM_1_2);
					break;
				case	SIXTEEN:
					transCanPkt(ONDIG_TO_MIE16_LPM_1_2);
					break;
				default:
					printf("ERROR                  \t");

				}
				 */


			}
		}
	}
	else
		CAN_Trans_flag = 0;
	if(CAN_Trans_flag == 250)
	{
		CAN_Trans_flag = 6;
	}
	if((CAN_Trans_flag > 3) && (CAN_Trans_flag < 6) && (Flag == 2))
	{
		//			Online_diag_flag[index] = 2;   //vrt
		//			InsertBits(insert_index,2, &Online_Diag_var.Online_Diag_Buf[check_index],3);
		//			printf("\n*****************************SLAVE_%d FB1_&FB2_LINK_FAIL****************\n",check_index);
		//
		//			for(i=0; i<8; i++)
		//			{
		//
		//			printf("\n222222Online_Diag_var.Online_Diag_Buf[%d]=%d",i,Online_Diag_var.Online_Diag_Buf[i]);
		//			}

		printf("\nSENT ONLINE DIAG CAN PACKT 2 TIMES WHEN FAILURE\n");
		/*
		switch(FIVE)
		{
		case	FIVE:
			transCanPkt(ONDIG_TO_MIE5_LPM_1_2);
			break;
		case	SIX:
			transCanPkt(ONDIG_TO_MIE6_LPM_1_2);
			break;
		case	SEVEN:
			transCanPkt(ONDIG_TO_MIE7_LPM_1_2);
			break;
		case	EIGHT:
			transCanPkt(ONDIG_TO_MIE8_LPM_1_2);
			break;
		case	NINE:
			transCanPkt(ONDIG_TO_MIE9_LPM_1_2);
			break;
		case	TEN:
			transCanPkt(ONDIG_TO_MIE10_LPM_1_2);
			break;
		case	ELEVEN:
			transCanPkt(ONDIG_TO_MIE11_LPM_1_2);
			break;
		case	TWELVE:
			transCanPkt(ONDIG_TO_MIE12_LPM_1_2);
			break;
		case	THIRTEEN:
			transCanPkt(ONDIG_TO_MIE13_LPM_1_2);
			break;
		case FOURTEEN:
			transCanPkt(ONDIG_TO_MIE14_LPM_1_2);
			break;
		case	FIFTEEN:
			transCanPkt(ONDIG_TO_MIE15_LPM_1_2);
			break;
		case	SIXTEEN:
			transCanPkt(ONDIG_TO_MIE16_LPM_1_2);
			break;
		default:
			printf("ERROR                  \t");
		}
		 */

		Failure_check_flag = 0;
		Flag = 4;
		memset(Online_diag_flag,'\0',sizeof(Online_diag_flag));
		//printf("\n111******************FIBER LINK FAIL*****************************=%d\n",Online_diag_flag[index]);
	}
	if(((Failure_check_flag > 3) && (Failure_check_flag < 6)&&(Flag == 4))
			||(ofc_flag1 > 1))
	{
		if(ofc_flag1 >1)
		{
			ofc_flag1++;
		}
		{
			//			for(i=0; i<8; i++)
			//			{
			//
			//			printf("\n3333333Online_Diag_var.Online_Diag_Buf[%d]=%d",i,Online_Diag_var.Online_Diag_Buf[i]);
			//			}
		}
		printf("\nSENT ONLINE DIAG CAN PACKT 2 TIMES WHEN OK\n");
		/*
		switch(FIVE)
		{
		case	FIVE:
			transCanPkt(ONDIG_TO_MIE5_LPM_1_2);
			break;
		case	SIX:
			transCanPkt(ONDIG_TO_MIE6_LPM_1_2);
			break;
		case	SEVEN:
			transCanPkt(ONDIG_TO_MIE7_LPM_1_2);
			break;
		case	EIGHT:
			transCanPkt(ONDIG_TO_MIE8_LPM_1_2);
			break;
		case	NINE:
			transCanPkt(ONDIG_TO_MIE9_LPM_1_2);
			break;
		case	TEN:
			transCanPkt(ONDIG_TO_MIE10_LPM_1_2);
			break;
		case	ELEVEN:
			transCanPkt(ONDIG_TO_MIE11_LPM_1_2);
			break;
		case	TWELVE:
			transCanPkt(ONDIG_TO_MIE12_LPM_1_2);
			break;
		case	THIRTEEN:
			transCanPkt(ONDIG_TO_MIE13_LPM_1_2);
			break;
		case FOURTEEN:
			transCanPkt(ONDIG_TO_MIE14_LPM_1_2);
			break;
		case	FIFTEEN:
			transCanPkt(ONDIG_TO_MIE15_LPM_1_2);
			break;
		case	SIXTEEN:
			transCanPkt(ONDIG_TO_MIE16_LPM_1_2);
			break;
		default:
			printf("ERROR                  \t");
		}
		 */
		CAN_Trans_flag = 0;
		Flag = 2;
		memset(Online_diag_flag,'\0',sizeof(Online_diag_flag));
		//	printf("\n************************FIBER LINK FAIL*****************************=%d\n",Online_diag_flag[index]);
	}
	memset(suc_ReceivedMessage,'\0',sizeof(suc_ReceivedMessage));
	temp2 = 0;
	temp3 = 0;
	temp1 = 0;
}

void process_to_comReceived(void)
{


	unsigned int NoofBuffBytes = 0,Dat_Check;
	//	static unsigned int ui_riucmdTxTics;
	static unsigned char EI_Buffer[300];
	NoofBuffBytes= CommReadAndFormMessage_Cmd(TWO_PORT,EI_Buffer,sizeof(EI_Buffer));

	if(NoofBuffBytes >0)
	{
		if((EI_Buffer[0]>=0x01) &&(EI_Buffer[0] <=0x1F))
		{
			Dat_Check = (int)(EI_Buffer[0]);
			if(EI_Buffer[22] == 0xFF)
			{
				Online_diag_flag[Dat_Check] =0;
				printf("\n*******************************SLAVE_%d OK*****************************\n",Dat_Check);
				//ofc_flag1 =0;
			}
			if(EI_Buffer[22] == 0xBF)
			{
				Online_diag_flag[Dat_Check] = 1;
				printf("\n***************************SLAVE_%d FB1_LINK_FAIL*********************\n",Dat_Check);
				//	ofc_flag2 =1;
				if(1 == ofc_flag1)
				{
					ofc_flag1 = 2;
					//Failure_check_flag = 4;

				}
			}
			if(EI_Buffer[22] == 0x7F)
			{
				Online_diag_flag[Dat_Check] = 2;
				printf("\n********************************SLAVE_%d FB2_LINK_FAIL*******************\n",Dat_Check);
				//ofc_flag2 =1;
				if(1 == ofc_flag1)
				{
					ofc_flag1 = 2;
					//Failure_check_flag = 4;

				}
			}
			if(EI_Buffer[22] == 0x3F)
			{
				Online_diag_flag[Dat_Check] = 3;
				printf("\n*****************************SLAVE_%d FB1_&FB2_LINK_FAIL****************\n",Dat_Check);
			}
		}
		sui_failure_determine_clock = HAL_GetTick();
	}
}

unsigned short int CommReadAndFormMessage_Cmd(unsigned short int us_Port,
		unsigned char *puc_ReadMsg,unsigned short int us_MaxSize)
{
	static  char sign1 = 0,sign2 = 0,sign3 = 0,case_check=0;
	static unsigned long int store_index ;
	unsigned char suc_RByte = 0x00;
	static unsigned char suc_RcvStatus = 11;
	while(Com_DataReady(us_Port))
	{
		suc_RByte = Com_Get_Byte(us_Port);
			printf("|%02X|",suc_RByte);

		switch(suc_RcvStatus)
		{
		case 11:
			if((suc_RByte >= 0x01) && (suc_RByte <= 0x1F) && (suc_RByte != 0x0E))
			{
				sign1 = suc_RByte;
				//	printf("sign11 = %X\n",sign1);
				store_index++;
				suc_RcvStatus = 12;
			}
			break;

		case 12:
			if (suc_RByte == FUN_CODE)
			{
				sign2 = suc_RByte;
				store_index++;
				//	printf("sign12= %X\n ",sign2);
				suc_RcvStatus = 13;
			}
			break;

		case 13:
			if(suc_RByte == DBYT_CNT)
			{

				sign3 = suc_RByte;
				store_index++;
				//	printf("sign13= %X\n ",sign3);
				suc_RcvStatus = 14;
				case_check =1;
			}
			else
			{
				store_index = 0;
				suc_RcvStatus = 11;
			}

			break;
		case 14:
			puc_ReadMsg[0] = sign1;
			// printf("BUFF = %X\n", puc_ReadMsg[0]);
			puc_ReadMsg[1] = sign2;
			puc_ReadMsg[2] = sign3;
			puc_ReadMsg[store_index] = suc_RByte;
			if(store_index < 33)
			{
				store_index++;
			}
			if(store_index == 33)
			{

				if(Cal_Crc_fun(puc_ReadMsg,(store_index-2)))
				{
					//	printf("CRC OK\n");
					case_check = store_index;
					suc_RcvStatus = 11;
					store_index = 0;
					return case_check;
				}
				else
				{
					printf("OFC_CRC NOK\n");
					suc_RcvStatus = 11;
					store_index = 0;
					return 0;
				}

			}

			break;
		default:

			printf("In default");
			break;


		}
	}
	return 0;
}

unsigned char Cal_Crc_fun(unsigned char * Buffer, unsigned short Length)
{
	unsigned short Rec_Crc,Cal_Crc;
	//	for(i=0;i<Length;i++)
	//	printf("RECV = %X ",Buffer[i]);
	Cal_Crc = ModBus_CRC16(Buffer,(Length));
	Rec_Crc = (unsigned short int ) (Buffer[Length+1] & 0xff);
	Rec_Crc = (unsigned short int ) (Rec_Crc<<8);
	Rec_Crc |= Buffer[Length] & 0xff;
	//	printf("Cal_Crc = %X Rec_Crc = %X\n",Cal_Crc,Rec_Crc);
	if(Cal_Crc	== Rec_Crc)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

unsigned short ModBus_CRC16( unsigned char * Buffer, unsigned short Length )
{
	unsigned char CRCHi = 0xFF;
	unsigned char CRCLo = 0xFF;
	int Index;
	unsigned short ret;
	//	printf("Length=%d",Length);
	//for(Index = 0;Index<Length;Index++)
	//printf("CAL = %X ",Buffer[Index]);
	while( Length--)
	{
		Index = CRCLo ^ *Buffer++ ;
		CRCLo = CRCHi ^ CRC_Table_Hi[Index];
		CRCHi = CRC_Table_Lo[Index];
	}
	//	CRCHi = CRCHi + 0x17;
	//	printf("LOW Byte = %X HIGHER BYTE = %X\n",CRCLo,CRCHi);
	ret=((unsigned short)CRCHi << 8);
	ret|= (unsigned short)CRCLo;
	return ret;
}

