/*
 * Kyosan.c
 *
 *  Created on: Apr 15, 2024
 *      Author: Ravi Teja P
 */

#include "Headers.h"

KYOSAN_FAULT_INJECTION_t KyosanFaultInjection_st;
KYOSAN_CLK_t KyosanClocks;
KYOSAN_RELAYS_t Kyosan_SelfRelays,Kyosan_OtherRelays,Kyosan_CANRelays,Kyosan_FinalRelays;
KYOSAN_CAN_RELAYS_t Kyosan_can_relays_st;


uint16_t TotalSigCountKyosan =0;
uint8_t SignalsKyosan[200][100] = {0,};
uint32_t MIE_SignalsSending_Clk_Kyosan = 0;
uint32_t LastCANReceived_Clk_Kyosan = 0;
uint8_t MisMatchFlagKyosan = 0;

uint8_t KyosanCommands_List[20][50] = {{0,},};
uint8_t KyosanCommands_Count = 0;

#ifdef EI_1
uint8_t SendThisCardRelays = 0;
#endif

void Process_Kyosan_EI(void)
{

	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);

		Feed_wdt();

		Get_Kyosan_EI_Message(KYOSAN_PORT);

		Process_Send_Signals_2_MIE_Kyosan();

		DetectCanXchangeFailure();

		Process_ComFailTimeout();

		readCanToStructs();

		Check_forKyosanCMD();

		SendKyosanRelays2Other();

		if(Dats_Request_Flag > 0)
		{
			gucFlagUpdate = 3;
			break;
		}
	}

	while(1)
	{
		HAL_IWDG_Refresh(&hiwdg);
		Data_Processing();
	}
}

void Process_ComFailTimeout(void)
{
	if((HAL_Elaps_Ticks(KyosanClocks.ControlDataPosRecvCLK) > EI_data.Kyosan_st.Comm_Fail_Time)
			||(HAL_Elaps_Ticks(KyosanClocks.ControlDataNegRecvCLK)> EI_data.Kyosan_st.Comm_Fail_Time)
			||(HAL_Elaps_Ticks(KyosanClocks.MessageReceivedCLK)> EI_data.Kyosan_st.Comm_Fail_Time)
			||(HAL_Elaps_Ticks(KyosanClocks.PollingDataPosRecvCLK)> EI_data.Kyosan_st.Comm_Fail_Time)
			||(HAL_Elaps_Ticks(KyosanClocks.PollingDataNegRecvCLK)> EI_data.Kyosan_st.Comm_Fail_Time))
	{

		if(strcmp((char *)EI_data.STN_NAME,"LAB_TESTING") == 0)
		{
			return;
		}



		if(Kyosan_SelfRelays.Message_Receiving == SET)
		{
			printf("Time out for Message Packets Reached\n");
			memset(&Kyosan_SelfRelays,0,sizeof(KYOSAN_RELAYS_t));
		}
	}
	else if((HAL_Elaps_Ticks(KyosanClocks.ControlDataPosRecvCLK) < EI_data.Kyosan_st.Comm_Fail_Time)
			&& (HAL_Elaps_Ticks(KyosanClocks.ControlDataNegRecvCLK)< EI_data.Kyosan_st.Comm_Fail_Time)
			&& (HAL_Elaps_Ticks(KyosanClocks.MessageReceivedCLK)< EI_data.Kyosan_st.Comm_Fail_Time)
			&& (HAL_Elaps_Ticks(KyosanClocks.PollingDataPosRecvCLK)< EI_data.Kyosan_st.Comm_Fail_Time)
			&& (HAL_Elaps_Ticks(KyosanClocks.PollingDataNegRecvCLK)< EI_data.Kyosan_st.Comm_Fail_Time))

	{
		Kyosan_SelfRelays.Message_Receiving = SET;
	}
}

void Process_CheckOtherData(void)
{
	static uint8_t RelayMisMatchCnt = 0;

	if((Kyosan_OtherRelays.Message_Receiving == SET) && (Kyosan_SelfRelays.Relay_CRC != Kyosan_OtherRelays.Relay_CRC))
	{
		++RelayMisMatchCnt;
		if(RelayMisMatchCnt > MAX_RELAY_MISMATCH_CNT)
		{
			RelayMisMatchCnt = MAX_RELAY_MISMATCH_CNT+1;
			MisMatchFlagKyosan = SET;
		}
	}
	else
	{
		MisMatchFlagKyosan = RESET;
		RelayMisMatchCnt = RESET;
	}
}

void SendKyosanRelays2Other(void)
{
	static uint32_t RelaySendingClk = 0;
	if(HAL_Elaps_Ticks(RelaySendingClk) > 3000)
	{
#ifdef EI_1
		TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
		TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif
		RelaySendingClk = HAL_GetTick();
	}
}

void DetectCanXchangeFailure(void)
{
	static uint32_t S2_BlinkClk = 0;

	if( (HAL_Elaps_Ticks(LastCANReceived_Clk_Kyosan) > EI_data.Kyosan_st.CAN_Fail_Clk) && (Is_CAN_Exchange_happening != RESET ))
	{
		if(strcmp((char *)EI_data.STN_NAME,"LAB_TESTING") == 0)
		{
			Is_CAN_Exchange_happening = SET;
			return;
		}

		Is_CAN_Exchange_happening = RESET;
	}
	else
	{
		if(HAL_Elaps_Ticks(S2_BlinkClk) > 300)
		{
			S2_BlinkClk = HAL_GetTick();
			HAL_GPIO_TogglePin(S2_LED_GPIO_Port, S2_LED_Pin);
		}

	}
}

void Process_Send_Signals_2_MIE_Kyosan(void)
{
	uint8_t Choice =0;

	if(HAL_Elaps_Ticks(MIE_SignalsSending_Clk_Kyosan) > EI_data.Kyosan_st.MIE_SEND_CLK)
	{
#ifdef EI_1
		if((Is_CAN_Exchange_happening == SET) && (MisMatchFlagKyosan == RESET) )
		{

			Choice = 0;
			(Kyosan_OtherRelays.Message_Receiving == SET)?SET_BIT_USER(0,Choice):CLEAR_BIT_USER(0,Choice);
			(Kyosan_SelfRelays.Message_Receiving == SET)?SET_BIT_USER(1,Choice):CLEAR_BIT_USER(1,Choice);

			switch(Choice)
			{
			case ONE:
				//				printf("\nOtherCard SET\n");
				memcpy(&Kyosan_FinalRelays.Relays,Kyosan_OtherRelays.Relays,MAX_KYOSAN_RELAYS);
#ifdef EI_1
				SendThisCardRelays = RESET;
#endif
				break;
			case TWO:
				//				printf("\nBothCard SET2\n");
			case THREE:
				//				printf("\nBothCard SET3\n");
				memcpy(&Kyosan_FinalRelays.Relays,Kyosan_SelfRelays.Relays,MAX_KYOSAN_RELAYS);
#ifdef EI_1
				SendThisCardRelays = SET;
#endif
				break;

			default:
				memset(&Kyosan_FinalRelays,0,sizeof(KYOSAN_RELAYS_t));
#ifdef EI_1
				SendThisCardRelays = RESET;
#endif
			}

			printf("Data Sending 2 MIE:\n");
			for(uint8_t i=0;i<60;++i)
			{
				printf("%02X ",Kyosan_FinalRelays.Relays[i]);

				if(((i+1)%10)==0)
				{
					printf("\n");
				}
			}
			printf("\n");
//			PrintSignalsKyosan();


			TransmitCanPkt(EI_PROCESS_DATA);
		}
		else
		{
			memset(&Kyosan_FinalRelays,0,sizeof(KYOSAN_RELAYS_t));
			TransmitCanPkt(EI_PROCESS_DATA);
		}
#endif

#ifdef EI_2

		if(Is_CAN_Exchange_happening == SET)
		{
			Choice = 0;
			(Kyosan_OtherRelays.Message_Receiving == SET)?SET_BIT_USER(0,Choice):CLEAR_BIT_USER(0,Choice);
			(Kyosan_SelfRelays.Message_Receiving == SET)?SET_BIT_USER(1,Choice):CLEAR_BIT_USER(1,Choice);

			switch(Choice)
			{
			case ONE:
				//				printf("\nOtherCard SET\n");
				memcpy(&Kyosan_FinalRelays.Relays,Kyosan_OtherRelays.Relays,MAX_KYOSAN_RELAYS);
				break;
			case TWO:
				//				printf("\nBothCard SET2\n");
			case THREE:
				//				printf("\nBothCard SET3\n");
				memcpy(&Kyosan_FinalRelays.Relays,Kyosan_SelfRelays.Relays,MAX_KYOSAN_RELAYS);
				break;

			default:
				memset(&Kyosan_FinalRelays,0,sizeof(KYOSAN_RELAYS_t));
			}
		}
		else
		{
			memset(&Kyosan_FinalRelays,0,sizeof(KYOSAN_RELAYS_t));
			TransmitCanPkt(EI_PROCESS_DATA);
		}
#endif
		MIE_SignalsSending_Clk_Kyosan = HAL_GetTick();
	}
}

void Get_Kyosan_EI_Message(uint8_t ComPort)
{
	uint8_t Data = 0;
	static uint16_t Max_Receive_Length = 0;
	static uint16_t Cur_Receive_Length = 0;
	static uint8_t Buffer[256] = {0,};
	static uint8_t Curnt_ProcessingDataType = 0;
	static uint8_t Control_DataReceived = RESET;
	static uint32_t Last_ValidControlInfoClk = 0;
	uint16_t C_CRC = 0;
	uint16_t R_CRC = 0;
#ifdef EI_1
	static uint16_t PrevRelayCRC = 0;
#endif
	static uint8_t ControlData_Positive[256] = {0,};
	static uint8_t ControlData_Negative[256] = {0,};
	uint8_t Pos_NegDataMisMatch = RESET;
	static uint8_t Pos_NegDataMisMatch_Count = RESET;
	uint8_t TypeofDataMisMatch = RESET;
	uint8_t ControlID = 0;



	if((HAL_Elaps_Ticks(Last_ValidControlInfoClk) >= EI_data.Kyosan_st.Comm_Fail_Time) && (Control_DataReceived == SET))
	{
		Control_DataReceived = RESET;
	}

	while(Com_DataReady(ComPort))
	{
		Data = Com_Get_Byte(ComPort);
		//		printf("%02X ",Data);

		if((Cur_Receive_Length == Max_Receive_Length) || (Cur_Receive_Length == RESET))
		{
			memset(Buffer,0,sizeof(Buffer));
			Cur_Receive_Length = RESET;

			switch(Data)
			{
			case ACTUAL_CONTROL_DATA:
			case COMPLIMENT_CONTROL_DATA:
				Max_Receive_Length = (	EI_data.Kyosan_st.No_ofFrames * 64);
				if(EI_data.Kyosan_st.No_ofFrames == 4)
				{
					Max_Receive_Length = 192;
				}
				Curnt_ProcessingDataType = CONTROL_DATA;
				//				printf("Received Ctrl Data|");
				break;

			default:

				/* Polling Data Range (0x11-0x7f for Positive data) (0x80-0xEE for Negative data)*/
				/* Control_DataReceived checking is added to Avoid Control information data to be processed as Polling Information */
				if( (Control_DataReceived == SET) && (((Data >= 0X11 ) && (Data <= 0X7F)) || ((Data >= 0X80 ) && (Data <= 0XEE))) )
				{
					/* Consider the data as Polling Data */
					//					printf("Received Polling Data:%u|%02X\n",Control_DataReceived,Data);
					Curnt_ProcessingDataType = POLLING_DATA;
					Max_Receive_Length = 3;
				}
			}
		}

		switch(Curnt_ProcessingDataType)
		{
		case CONTROL_DATA:
			Buffer[Cur_Receive_Length++] = Data;
			if(Cur_Receive_Length == Max_Receive_Length)
			{
				C_CRC = Calc_Kysoan_CRC(Buffer, Cur_Receive_Length-2);
				R_CRC = ExtractBits(0, 16, &Buffer[Cur_Receive_Length-2]);


				if( (  (CHECKFLAG(CRC_FAIL_CONTROL_DATA_POS,KyosanFaultInjection_st.FAULT_INJECTION_BITS)) == SET) && (CHECKFLAG(7,Buffer[0]) == RESET))
				{
					printf("\nInjecting Positive Control Data CRC Failure\n");
					R_CRC = (HAL_RNG_GetRandomNumber(&hrng)%0XFFFF);
					if(R_CRC ==  C_CRC)
					{
						R_CRC = R_CRC-(HAL_RNG_GetRandomNumber(&hrng)%0XFF);
					}
				}

				if( ((CHECKFLAG(CRC_FAIL_CONTROL_DATA_NEG,KyosanFaultInjection_st.FAULT_INJECTION_BITS)) == SET) && (CHECKFLAG(7,Buffer[0]) == SET))
				{
					printf("\nInjecting Negative Control Data CRC Failure\n");
					R_CRC = (HAL_RNG_GetRandomNumber(&hrng)%0XFFFF);
					if(R_CRC ==  C_CRC)
					{
						R_CRC = R_CRC-(HAL_RNG_GetRandomNumber(&hrng)%0XFF);
					}
				}

				if(R_CRC == C_CRC)
				{

					HAL_GPIO_TogglePin(S1_LED_GPIO_Port, S1_LED_Pin);


					Last_ValidControlInfoClk = HAL_GetTick();
					Control_DataReceived = SET;
					KyosanClocks.MessageReceivedCLK = HAL_GetTick();

					//					printf("CTRL-Data CRC OK\n");
					if(CHECKFLAG(7,Buffer[0]) == RESET)
					{
						printf("+ Ctrl Data|");
						memcpy(&ControlData_Positive,&Buffer,Cur_Receive_Length);

						if( ((CHECKFLAG(COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA,KyosanFaultInjection_st.FAULT_INJECTION_BITS)) == SET) && (CHECKFLAG(7,Buffer[0]) == SET))
						{
							printf("\nInjecting COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA Failure\n");
							ControlData_Positive[(HAL_RNG_GetRandomNumber(&hrng)%0xf)+1] = HAL_RNG_GetRandomNumber(&hrng)%0XFF;
						}

						ControlID = (uint8_t)((uint8_t)ControlData_Positive[0]&(uint8_t)0x0f);
						//						printf("%u|",ControlID);
						for(uint16_t  i=0;i<(Cur_Receive_Length-2);++i) /*(Cur_Receive_Length-2) is for Not checking CRC Location */
						{
							if(ControlData_Negative[i] != (uint8_t)(~ControlData_Positive[i]))
							{
								Pos_NegDataMisMatch = SET;
								TypeofDataMisMatch = POS_DATA_MISMATCH;
								//								printf("Positive & Negative data not Matching\n");
								//								printf("P:%X-N:%X -> %X\n",ControlData_Positive[i],ControlData_Negative[i],(uint8_t)(~ControlData_Positive[i]));
								break;
							}
						}

						if(Pos_NegDataMisMatch == RESET)
						{
							printf("\nR:");
							for(uint8_t i=1;i<61;++i)
							{
								printf("%02X ",ControlData_Positive[i]);
							}
							printf("\n");

							memcpy(Kyosan_SelfRelays.Relays,&ControlData_Positive[1],60);
							Kyosan_SelfRelays.Relay_CRC = Calc_Kysoan_CRC(Kyosan_SelfRelays.Relays, 60);

#ifdef EI_1
							if((SendThisCardRelays == SET)&&(PrevRelayCRC != Kyosan_SelfRelays.Relay_CRC))
							{
								PrevRelayCRC = Kyosan_SelfRelays.Relay_CRC;
								TransmitCanPkt(EI_PROCESS_DATA);
//								PrintSignalsKyosan();
							}
#endif

						}

					}
					else
					{
						printf("- Ctrl Data|");
						memcpy(&ControlData_Negative,&Buffer,Cur_Receive_Length);
						ControlID = (uint8_t)(~ControlData_Negative[0]);

						if( ((CHECKFLAG(COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA,KyosanFaultInjection_st.FAULT_INJECTION_BITS)) == SET) && (CHECKFLAG(7,Buffer[0]) == SET))
						{
							printf("\nInjecting COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA Failure\n");
							ControlData_Negative[(HAL_RNG_GetRandomNumber(&hrng)%0xf)+1] = HAL_RNG_GetRandomNumber(&hrng)%0XFF;
						}


						for(uint16_t i=0;i<(Cur_Receive_Length-2);++i) /*(Cur_Receive_Length-2) is for Not checking CRC Location */
						{
							if(ControlData_Positive[i] != (uint8_t)(~ControlData_Negative[i]))
							{
								Pos_NegDataMisMatch = SET;
								TypeofDataMisMatch = NEG_DATA_MISMATCH;
								//								printf("Negative & Positive data not Matching\n");
								//								printf("N:%02X-P:%02X -> %X\n",ControlData_Negative[i],ControlData_Positive[i],(uint8_t)(~ControlData_Negative[i]));
								break;
							}
						}
						if(Pos_NegDataMisMatch == RESET)
						{
							//							printf("\nR:");
							//							for(uint8_t i=1;i<61;++i)
							//							{
							//								printf("%02X ",ControlData_Negative[i]);
							//							}
							//							printf("\n");
						}
					}

					if(Pos_NegDataMisMatch == RESET)
					{
						//						printf("+ & -:DATA:OK\n");
						if(CHECKFLAG(7,Buffer[0]) == RESET)
						{
							KyosanClocks.ControlDataPosRecvCLK = HAL_GetTick();
						}
						else
						{
							KyosanClocks.ControlDataNegRecvCLK = HAL_GetTick();
						}
						Pos_NegDataMisMatch_Count = RESET;
						TypeofDataMisMatch = RESET;
						Pos_NegDataMisMatch = RESET;
					}
					else
					{
						++Pos_NegDataMisMatch_Count;

						if(Pos_NegDataMisMatch_Count == 2)
						{
							switch(TypeofDataMisMatch)
							{
							case POS_DATA_MISMATCH:
								printf("P:");
								for(uint16_t i=0;i<Cur_Receive_Length;++i)
								{
									printf("%02X ",ControlData_Positive[i]);
								}
								printf("\nN:");
								for(uint16_t i=0;i<Cur_Receive_Length;++i)
								{
									printf("%02X ",ControlData_Negative[i]);
								}
								printf("\n");
								break;
							case NEG_DATA_MISMATCH:
								printf("N:");
								for(uint16_t i=0;i<Cur_Receive_Length;++i)
								{
									printf("%02X ",ControlData_Negative[i]);
								}
								printf("\nP:");
								for(uint16_t i=0;i<Cur_Receive_Length;++i)
								{
									printf("%02X ",ControlData_Positive[i]);
								}
								printf("\n");
								break;
							default:
								;

							}
						}

						if(Pos_NegDataMisMatch_Count >= EI_data.Kyosan_st.MaxMisMatchCnt)
						{
							printf("\n+ & - Data Mismatch count Reached:%u\n",Pos_NegDataMisMatch_Count);
							Pos_NegDataMisMatch_Count = EI_data.Kyosan_st.MaxMisMatchCnt; /* To avoid Overflow */
						}
					}
				}
				else
				{
					printf("\n%c Control Data Packet CRC Failed C:%04x-R:%04x\n",((CHECKFLAG(7,Buffer[0]) == RESET)?'+':'-'),C_CRC,R_CRC);

				}
				Cur_Receive_Length = RESET;
				Max_Receive_Length = RESET;
			}
			break;
		case POLLING_DATA:
			Buffer[Cur_Receive_Length++] = Data;
			if(Cur_Receive_Length == Max_Receive_Length)
			{
				C_CRC = Calc_Kysoan_CRC(Buffer, Cur_Receive_Length-2);
				R_CRC = ExtractBits(0, 16, &Buffer[Cur_Receive_Length-2]);

				if( (  (CHECKFLAG(CRC_FAIL_POLLING_DATA_POS,KyosanFaultInjection_st.FAULT_INJECTION_BITS)) == SET) && (CHECKFLAG(7,Buffer[0]) == RESET))
				{
					printf("\nInjecting Positive Polling Data CRC Failure\n");
					R_CRC = (HAL_RNG_GetRandomNumber(&hrng)%0XFFFF);
					if(R_CRC ==  C_CRC)
					{
						R_CRC = R_CRC-(HAL_RNG_GetRandomNumber(&hrng)%0XFF);
					}
				}

				if( ((CHECKFLAG(CRC_FAIL_POLLING_DATA_NEG,KyosanFaultInjection_st.FAULT_INJECTION_BITS)) == SET) && (CHECKFLAG(7,Buffer[0]) == SET))
				{
					printf("\nInjecting Negative Polling Data CRC Failure\n");
					R_CRC = (HAL_RNG_GetRandomNumber(&hrng)%0XFFFF);
					if(R_CRC ==  C_CRC)
					{
						R_CRC = R_CRC-(HAL_RNG_GetRandomNumber(&hrng)%0XFF);
					}
				}

				if(C_CRC == R_CRC)
				{
					KyosanClocks.MessageReceivedCLK = HAL_GetTick();
					HAL_GPIO_TogglePin(S1_LED_GPIO_Port, S1_LED_Pin);

					if(CHECKFLAG(7,Buffer[0]) == RESET)
					{
						KyosanClocks.PollingDataPosRecvCLK = HAL_GetTick();
					}
					else
					{
						KyosanClocks.PollingDataNegRecvCLK = HAL_GetTick();
					}
					//CRC_FailCnt = RESET;
				}
				else
				{
					printf("Polling Data CRC Failed\nC:%04x-R:%04x\n",C_CRC,R_CRC);
					printf("\n%c Polling Data CRC Failed C:%04x-R:%04x\n",((CHECKFLAG(7,Buffer[0]) == RESET)?'+':'-'),C_CRC,R_CRC);

				}
				Cur_Receive_Length = RESET;
				Max_Receive_Length = RESET;
			}
			break;
		default:
			Cur_Receive_Length = RESET;

		}
	}
	UNUSED(ControlID);
}


uint16_t Calc_Kysoan_CRC(uint8_t *data, size_t length)
{
	uint16_t crc = KYOSAN_CRC_INIT_VAL;

	for (size_t i = 0; i < length; i++)
	{
		crc ^= (uint16_t)data[i] << 8; // XOR with next byte shifted to left
		for (int j = 0; j < 8; j++)
		{
			if (crc & 0x8000)
			{
				crc = (crc << 1) ^ KYOSAN_CRC_POLYNOMIAL;
			}
			else
			{
				crc <<= 1;
			}
		}
	}
	return crc;
}



void Update_KyosanStructure(void)
{
	EI_data.Kyosan_st.Baudrate = 31200;
	EI_data.Kyosan_st.Bool_outputs = 40;
	EI_data.Kyosan_st.Comm_Fail_Time = 2000;
	EI_data.Kyosan_st.MIE_SEND_CLK = 2000;
	EI_data.Kyosan_st.MaxMisMatchCnt = 30;
	EI_data.Kyosan_st.No_ofFrames = 1;
	EI_data.Kyosan_st.CAN_Fail_Clk = 5000;
}


void UpdateSIGNALSNamesKyosan(void)
{
	uint16_t Signals_Cnt_Kyosan = 0;
	Signals_Cnt_Kyosan = 0;
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"CLTPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"DMTPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"UMTPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"ULTPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"1ATPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"30ATPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"3_4TPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"6TPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"26_28TPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"25TPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1DDECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1DHHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1DHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1|DDECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1|DHHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1|DHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1DECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1UECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S1AHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S3DECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S3HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S3RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S4HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S4RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S6DECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S6RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S25DECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S25RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S26HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S26RECR");

	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S27HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S27RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S28DECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S28HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S28RECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30DECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30HECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30RECR");

	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30UECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30AHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30|DDECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30|DHHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30|DHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30DDECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30DHHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S30DHECR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"11NWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"11RWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"12NWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"12RWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"13NWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"13RWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"18NWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"18RWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"19NWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"19RWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"20NWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"20RWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"15KLNWKR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"22LXPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S6LCPR");
	strcpy((char *)SignalsKyosan[Signals_Cnt_Kyosan++], (const char *)"S25LCPR");

	TotalSigCountKyosan = Signals_Cnt_Kyosan;


}


void PrintSignalsKyosan(void)
{
	//	memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);
	//	static uint8_t PrevRelays[1024] = {0,};
	uint8_t EnablePrint = 0;


	//if(memcmp(&PrevRelays,&vEI_Transver_EI_To_MIE.Inputs,(EI_data.Hitachi_st.PeersCnt*MAX_OUTPUTS_PER_PEER))!= 0)
	{
		EnablePrint = 1;
	}

	if(EnablePrint == 1)
	{
		Get_time();
		printf("Time:%02u:%02u:%02u\n",RTC_Date_time_Dec_st.Hour,RTC_Date_time_Dec_st.Mins,RTC_Date_time_Dec_st.Secs);
		for(uint16_t Traverser = 0;Traverser< 64;++Traverser)
		{

			if(GETPIN_Kyosan(Traverser+1) > 0)
			{
				printf("%15s:PICKUP   ",SignalsKyosan[Traverser]);
			}
			else
			{
				printf("%15s:DROP      ",SignalsKyosan[Traverser]);
			}

			if( (Traverser >0) &&(Traverser%4) == 0)
			{
				printf("\n");
			}
		}
	}
}


void Check_forKyosanCMD(void)
{
	//	unsigned char ch[20] = {0,},i=0;
	uint8_t IS_COMMAND_RECEIVED = NO;
	uint8_t Index = 0;
	uint8_t Byte = 0;
	uint8_t Buffer[100] = {0,};
	if(HAL_Elaps_Ticks(Isp_Intr_Clk) < 10)
	{
		return;
	}
	else
	{
		while(Com_DataReady(ISP_PORT))
		{
			Byte = Com_Get_Byte(ISP_PORT);
			Buffer[Index++] = Byte;
		}
		if(Index>0)
		{
			Buffer[Index] = 0;
			printf("Received from ISP:%s\n",Buffer);
			switch(Is_KyosanCommand_Valid(Buffer,Index))
			{
			case ZERO:
				Dats_Request_Flag = 1;
				printf("\n*************PLEASE LOAD EI DATS USING APPLICATION************\n");
				break;
			case ONE:
				printf("Received \"%s\":%u\n",KyosanCommands_List[ONE],(Buffer[Index-1])-'0');
				if( (((Buffer[Index-1])-'0') >= 0) && ((Buffer[Index-1])-'0') < 2)
				{
					Debug_flag = (Buffer[Index-1])-'0';
				}
				else
				{
					printf("Please Send Valid Command\n");
				}
				IS_COMMAND_RECEIVED = YES;
				break;

			case TWO:
				printf("Received %s\n",KyosanCommands_List[TWO]);
				printf("CRC_FAIL_CONTROL_DATA_POS\n");
				SET_BIT_USER(CRC_FAIL_CONTROL_DATA_POS,KyosanFaultInjection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case THREE:
				printf("Received %s\n",KyosanCommands_List[THREE]);
				printf("CRC_FAIL_CONTROL_DATA_NEG\n");
				KyosanFaultInjection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(CRC_FAIL_CONTROL_DATA_NEG,KyosanFaultInjection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case FOUR:
				printf("Received %s\n",KyosanCommands_List[FOUR]);
				printf("CRC_FAIL_POLLING_DATA_POS\n");
				KyosanFaultInjection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(CRC_FAIL_POLLING_DATA_POS,KyosanFaultInjection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case FIVE:
				printf("Received %s\n",KyosanCommands_List[FIVE]);
				printf("CRC_FAIL_POLLING_DATA_NEG\n");
				KyosanFaultInjection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(CRC_FAIL_POLLING_DATA_NEG,KyosanFaultInjection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case SIX:
				printf("Received %s\n",KyosanCommands_List[SIX]);
				printf("COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA\n");
				KyosanFaultInjection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA,KyosanFaultInjection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case SEVEN:
				printf("Received %s\n",KyosanCommands_List[SEVEN]);
				printf("CLEAR ALL\n");
				KyosanFaultInjection_st.FAULT_INJECTION_BITS = 0;
				IS_COMMAND_RECEIVED = YES;
				break;
			default:
				printf("Please Send Valid Command\n");
				for(uint8_t i=0;i<KyosanCommands_Count;++i)
				{
					printf("%s\n",KyosanCommands_List[i]);
				}
			}
		}

		if(IS_COMMAND_RECEIVED == YES)
		{
			TransmitCanPkt(CAN_FAULT_INJECTION);
		}
	}
}


void Update_KyosanCommands(void)
{
	uint8_t CmdCnt = 0;

	KyosanCommands_List[CmdCnt][0] = 0X24;
	KyosanCommands_List[CmdCnt][1] = 0X01;
	KyosanCommands_List[CmdCnt][2] = 0X30;
	KyosanCommands_List[CmdCnt][3] = 0X40;
	KyosanCommands_List[CmdCnt][4] = 0XC0;
	KyosanCommands_List[CmdCnt][5] = 0X4E;
	KyosanCommands_List[CmdCnt][6] = 0X26;
	++CmdCnt;

	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"DEBUG_MODE");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"CRC_FAIL_CONTROL_DATA_POS");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"CRC_FAIL_CONTROL_DATA_NEG");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"CRC_FAIL_POLLING_DATA_POS");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"CRC_FAIL_POLLING_DATA_NEG");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"COMPLIMENT_DATA_MISMATCH_FOR_CONTROL_DATA");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"CLEAR_ALL");
	strcpy( (char *)KyosanCommands_List[CmdCnt++], (const char *)"HELP");
	KyosanCommands_Count =  CmdCnt;
}

uint8_t Is_KyosanCommand_Valid(uint8_t *Buffer,uint8_t Length)
{
	for(uint8_t Traverser = 0; Traverser < KyosanCommands_Count; ++Traverser)
	{
		//		if(strstr((char *)Buffer,(char *)KyosanCommands_List[Traverser]) != NULL)
		if(memcmp(Buffer,KyosanCommands_List[Traverser],strlen((char *)KyosanCommands_List[Traverser])) == 0)
		{
			return Traverser;
		}
	}
	return 255;
}
