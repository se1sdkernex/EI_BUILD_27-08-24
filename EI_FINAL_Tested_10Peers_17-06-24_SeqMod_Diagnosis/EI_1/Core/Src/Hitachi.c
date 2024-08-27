/*
 * Hitachi.c
 *
 *  Created on: May 15, 2023
 *      Author: Ravi Teja P
 */


#include "Headers.h"
#include "lwip.h"

uint8_t 	Is_CAN_Exchange_happening = YES;
uint8_t 	Rx_Seq_failure = 0;
uint32_t 	Interrupt_clock = 0;
uint32_t 	RCV_SELF_XCHG_CLK = 0;
uint32_t 	Self_Sharing_CAN_Clk[MAX_PEER_CONNECTIONS] = {0,};
uint8_t		Is_GPS_Packet_Received = 0;
uint8_t		Tx_Peer_Index_To_SignalReq = 0XFF;
uint8_t 	Rx_Peer_Index_To_SignalReq = 0xff;
uint32_t 	MIE_Signal_Send_CLK = 0;


/* NEW VARIABLES */
/* This variable will be used to detect that any Message received from the Peer */
uint32_t Last_ReceptionClk[MAX_PEER_CONNECTIONS];
uint32_t OtherCardReceptionClk[MAX_PEER_CONNECTIONS];
uint8_t Act_InactiveFlag[MAX_PEER_CONNECTIONS] = {0,};
uint8_t LastActCount[MAX_PEER_CONNECTIONS] = {0,};
uint8_t ReorderFlag = 0;

#if ENABLE_SIGNAL_PRINT

uint8_t SignalsHitachi[1536][40] = {{0,},};
uint16_t TotalSigCountHitachi;
#endif

/* NEW VARIABLES */





uint16_t	MisMatchFlag[MAX_PEER_CONNECTIONS]					= {0,};
//uint16_t	MisMatchClk[]
uint32_t	Last_Vital_Received_Clk[MAX_PEER_CONNECTIONS]		= {0,};
uint16_t 	Prev_Relay_CRC[MAX_PEER_CONNECTIONS]				= {0,};
uint32_t 	Link_Establish_Clock[MAX_PEER_CONNECTIONS]			= {0,};
uint32_t 	OtherCardLink_Establish_Clock[MAX_PEER_CONNECTIONS] = {0,};
uint8_t		SelfLinkFlag[MAX_PEER_CONNECTIONS]						= {0,};
uint8_t		OtherCardLink_Flag[MAX_PEER_CONNECTIONS]			= {0,};
uint32_t 	Prev_IP_Source[MAX_PEER_CONNECTIONS] 				= {0,};
uint8_t 	Self_Seq_Init_Value[MAX_PEER_CONNECTIONS] 			= {12,21,15};
uint32_t 	Last_Recevied_Clock[MAX_PEER_CONNECTIONS] 			= {0,};
uint32_t 	Last_Recevied_Clock_ValidMsgs[MAX_PEER_CONNECTIONS] = {0,};
uint8_t 	INIT_Received[MAX_PEER_CONNECTIONS]					= {0,};
uint8_t		App_Identity_CRC_Fail[MAX_PEER_CONNECTIONS]			= {0,};
uint8_t		Received_Init[MAX_PEER_CONNECTIONS]					= {0,};
uint8_t		Received_Vital[MAX_PEER_CONNECTIONS]				= {0,};
uint8_t		IP_Source_Peer[MAX_PEER_CONNECTIONS]				= {0,};
uint32_t	Prev_INTR_Rcv_Clk[MAX_PEER_CONNECTIONS]				= {0,};
ip_addr_t	Recv_IP_List[MAX_PEER_CONNECTIONS]					= {0,};
uint8_t		Recv_IP_Index[MAX_PEER_CONNECTIONS]					= {0,};
uint8_t		Init_Msg_Sent_After_Timeout[MAX_PEER_CONNECTIONS] 	= {0,};
uint32_t	Last_Received_Init_Clk[MAX_PEER_CONNECTIONS]		= {0,};
uint8_t		INIT_REQ_SENT_Flag[MAX_PEER_CONNECTIONS] 			= {0,};
uint8_t		SEND_FORCE_REPLY[MAX_PEER_CONNECTIONS]				= {0,};
uint8_t		Change_MLK_TIME[MAX_PEER_CONNECTIONS]				= {0,};

EI_VITAL_DATA_TO_VITAL_COMPUTER_t 	vEI_Transver_EI_To_MIE;
EI_VITAL_DATA_TO_VITAL_COMPUTER_t 	HitachiEI_DataReArranged_st;
HITACHI_TIME_t 						HITACHI_time_Stamp_st[MAX_PEER_CONNECTIONS];
HITACHI_PARAMETERS_t 				Hitachi_prms_st[MAX_PEER_CONNECTIONS];
HITACHI_INIT_t 						Hitachi_Init_st[MAX_PEER_CONNECTIONS];
HITACHI_VITAL_t						Hitachi_Vital_st[MAX_PEER_CONNECTIONS];
HITACHI_SERVICE_t					Hitachi_Service_st[MAX_PEER_CONNECTIONS];
HITACHI_STATUS_t					Hitachi_Status_st[MAX_PEER_CONNECTIONS];

EI_SELF_SHARING_t					Tx_Ei_self_sharing_st[MAX_PEER_CONNECTIONS];
EI_SELF_SHARING_t					Rx_Ei_self_sharing_st[MAX_PEER_CONNECTIONS];
EI_SELF_SHARING_t 					Temp_Tx_Ei_self_sharing_st;
EI_SELF_SHARING_t 					Temp_Rx_Ei_self_sharing_st;

SEND_VITAL_MSG_2_OTH_t				Tx_VITAL_MSG_CAN_st,Rx_VITAL_MSG_CAN_st,Temp_Rx_VITAL_MSG_CAN_st;
HITACHI_FAULT_INJECTION_t			Hitachi_fault_injection_st;
HITACHI_FAULT_LOGGING_t				Hitachi_fault_logging_st,Prev_Hitachi_fault_logging_st;
LAST_VDATA_t						Last_Rcv_Vdata_st[MAX_PEER_CONNECTIONS];
RX_Print_t							Rx_Print_st;
Send_Vital_MSG_t					Send_Vital_MSG_st;
HITACHI_LINK_CLKS_FLAG_t			Hitachi_clk_flags_st[MAX_PEER_CONNECTIONS];
HITACHI_LINK_CLKS_FLAG_t			Other_Hitachi_clk_flags_st[MAX_PEER_CONNECTIONS];

RELAYS_t							Relays_st[MAX_PEER_CONNECTIONS];
RELAYS_t							Main_Relays_st[MAX_PEER_CONNECTIONS];
RELAYS_t							OtherCard_Relays_st[MAX_PEER_CONNECTIONS];
RELAYS_t							Tx_Temp_Main_Relays_st,Rx_Temp_Main_Relays_st;

void Process_Hitachi_Packets_Eth(void)
{
	uint8_t Data = 0;
	uint8_t Temp_array[HITACHI_MAX_PAYLOAD] = {0,};
	uint8_t Traverser = 0,uc_Found = 0;

	while(Is_There_EI_Data())
	{
		Data = Get_Eth_Byte();
		switch(Data)
		{
		case HITACHI_SOM:
			Temp_array[Traverser++] = Data;
			break;
		case HITACHI_EOM:
			Temp_array[Traverser++] = Data;
			uc_Found = Traverser;
			break;
		default:
			if(Traverser >0)
			{
				Temp_array[Traverser++] = Data;
			}
		}
		if(uc_Found > 0)
		{
			break;
		}
	}
	if(uc_Found > 0)
	{
		if(Traverser>15)
		{
			Convert_Data_format(Temp_array,Traverser);
		}
		Traverser = 0;
	}
}

void Process_Hitachi_Packets_Com(void)
{
	uint8_t Data = 0;
	uint8_t Temp_array[HITACHI_MAX_PAYLOAD] = {0,};
	uint8_t Traverser = 0,uc_Found = 0;

	if(EI_data.Hitachi_st.EnableSerialPort == 0)
	{
		return;
	}


	/* Waiting Until the Full Packet is Received this Variable will be updated in UART4 Interrupt*/
	if(HAL_Elaps_Ticks(UART4_Intr_Clk) < 20)
	{
		return;
	}

	while(Com_DataReady(THREE_PORT))
	{
		Data = Com_Get_Byte(THREE_PORT);
		printf("*%02X*",Data);
		if(Traverser > (HITACHI_MAX_PAYLOAD-2))
		{
			printf("Received data %u Bytes exceeded %u Bytes",Traverser,HITACHI_MAX_PAYLOAD);
			Traverser = 0;
			break;
		}
		//printf("%02X ",Data);
		switch(Data)
		{
		case HITACHI_SOM:
			Temp_array[Traverser++] = Data;
			break;
		case HITACHI_EOM:
			Temp_array[Traverser++] = Data;
			uc_Found = Traverser;
			break;
		default:
			if(Traverser >0)
			{
				Temp_array[Traverser++] = Data;
			}
		}
		if(uc_Found > 0)
		{
			break;
		}
	}
	if(uc_Found > 0)
	{
		if(Traverser>15)
		{
			Convert_Data_format(Temp_array,Traverser);
		}
		Traverser = 0;
		memset(&Temp_array,0,sizeof(Temp_array));
	}
}


void Convert_Data_format(uint8_t *Data,uint8_t Length)
{
	uint8_t Temp_Buff[128] = {0,};
	uint16_t des_index=0,src_index=0;

	for(des_index=0,src_index=0; src_index < Length; ++des_index,++src_index)
	{
		if(Data[src_index] == (uint8_t)0xF0)
		{
			Temp_Buff[des_index] = Data[src_index] | Data[src_index+1];
			++src_index;
		}
		else
		{
			Temp_Buff[des_index] = Data[src_index];
		}
	}
	EI_Data_Analyzer(Temp_Buff,des_index);
}

void EI_Data_Analyzer(unsigned char *Buff, unsigned short int Size)
{
	uint8_t Message_Type = 0;
	uint8_t Source_Index = 0,Dest_Index = 0;
	uint32_t Destin_Address = 0;
	uint32_t Source_Address = 0;
	uint32_t Cur_Ticks = HAL_GetTick();

	if( (Is_CAN_Exchange_happening == NO) && (HAL_GetTick() >= TEN_SEC) )
	{
		printf("\n Not Processing Any EI Packet As CAN Packets are Not receiving\n");
		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0)
		{
			//Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = 0x123456;
			return;
		}		//		return;
	}

	Source_Address  = (uint16_t)ExtractBits(0, 16, (unsigned char *)&Buff[3]);
	for(; Source_Index < EI_data.Hitachi_st.PeersCnt; ++Source_Index)
	{
		if(Source_Address == EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index])
		{
			break;
		}
	}
	if(Source_Index>= EI_data.Hitachi_st.PeersCnt)
	{
		if(Debug_flag == 1)
		{
			printf("Rejected Source Address:%lu which is Not Available in List\n",Source_Address);
		}
		return;
	}

	Destin_Address = (uint16_t)ExtractBits(0, 16, (unsigned char *)&Buff[1]);

	if(Destin_Address != EI_data.Hitachi_st.DEST_ADDRESSES[Source_Index] )
	{
		printf("Packet Indexes Not matching\n");
		printf("Received Destination Address:%lu\n",Destin_Address);
		printf("Actual Destination Address for Source Address[%lu]:%lu\n",Source_Address,EI_data.Hitachi_st.DEST_ADDRESSES[Source_Index]);
		return;
	}
	else
	{
		Dest_Index = Source_Index;
	}
	if( (CHECKFLAG(4,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		if(Dest_Index == Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX)
		{
			printf("\nInjected Invalid Destin_Address\n");
			printf("Received:%lu\n",Destin_Address);
			Destin_Address = 333;
			Dest_Index = EI_data.Hitachi_st.PeersCnt;
			printf("Injected:%lu\n",Destin_Address);
		}
	}
	if(Dest_Index>=EI_data.Hitachi_st.PeersCnt)
	{
		printf("Destin_Address is not in the List of DEST_ADDRESSES\n");
		printf("Received Destin_Address:%lu\n",Destin_Address);
		return;
	}

	if(Debug_flag == 1)
	{
		switch(Recv_IP_Index[Source_Index])
		{
		case ONE:
			printf("RCV from MLK-A\n");
			break;
		case TWO:
			printf("RCV from MLK-B\n");
			break;
		default:
			if(EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL)
			{
				printf("Received from Serial\n");
			}
			else
			{
				printf("Received from Unknown Source:%u\n",Recv_IP_Index[Source_Index]);
			}
		}
	}
	if(Prev_IP_Source[Source_Index] != Recv_IP_Index[Source_Index]) //This if condition is to Avoid data missing when MLK Primary to Secondary shifting.
	{
		Last_Recevied_Clock[Source_Index] = Cur_Ticks;
		Last_Recevied_Clock_ValidMsgs[Source_Index] = Cur_Ticks;
		Prev_IP_Source[Source_Index] = Recv_IP_Index[Source_Index];
		printf("MLK Shifting Detected\n");
	}
	IP_Source_Peer[Source_Index] = Recv_IP_Index[Source_Index];

	if( HAL_Elaps_Ticks(Prev_INTR_Rcv_Clk[Source_Index]) >= EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Source_Index])
	{
		Prev_INTR_Rcv_Clk[Source_Index] =Cur_Ticks;
		Last_Recevied_Clock[Source_Index] = Cur_Ticks;
		Last_Recevied_Clock_ValidMsgs[Source_Index] = Cur_Ticks;
		printf("EIPC Shifting Detected:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
	}
	Prev_INTR_Rcv_Clk[Source_Index] = Cur_Ticks;

	memset(&Rx_Print_st,0,sizeof(RX_Print_t));
	memcpy(Rx_Print_st.Print_Buff,Buff,Size);
	Rx_Print_st.Size = Size;
	//Print_EI_DATA(Buff,RCV,Size); // To Print in Hitachi Format.

	Message_Type = (uint8_t)ExtractBits(0, 8, (unsigned char *)&Buff[7]);
	Tx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status = RECEIVED;


	Last_ReceptionClk[Source_Index] = Cur_Ticks;
	Check_Other_EiPC_Packet(Source_Index);
	switch (Message_Type)
	{
	case INIT_MSG:
		//printf("\nReceived Init Message\n");
		Init_Msg_extr(Buff, Size, Source_Index, Dest_Index);
		break;

	case VDATAMSG:
		//printf("\nReceived VITAL DATA Message\n");
		Vital_Msg_extr(Buff, Size, Source_Index, Dest_Index);
		break;

	case NVDATAMSG:
		printf("\nReceived NON-VITAL DATA Message\n");
		break;

	case STATUSMSG:
		//printf("\nReceived STATUS Message\n");
		Status_Msg_Extr(Buff, Size, Source_Index, Dest_Index);
		break;

	case SERVICEMSG:
		//printf("\nReceived SERVICE Message\n");
		Service_Msg_Extr(Buff, Size, Source_Index, Dest_Index);
		break;

	default:
		printf("Received \"%d\" Message type\n", Message_Type);
		break;
	} /* Switch */

} /*  EI_Data_Analyzer */

void Init_Msg_extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index)
{
	// unsigned int Traverser=0;
	uint16_t Recv_App_CRC = 0;
	uint8_t Recv_Inputs = 0;
	uint32_t Calc_CRC = 0;
	uint32_t Recv_CRC = 0;
	uint32_t Cur_Ticks = HAL_GetTick();
	BITSTUFFINIT(0, 0);

	Calc_CRC = CalculateDirectReverseCrc(Buff, Length - 5);
	Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length-5]);

	if(Recv_CRC != Calc_CRC)
	{
		printf("Init Message CRC fail:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
		printf("R-CRC:0x%08lX\n",Recv_CRC);
		printf("C-CRC:0x%08lX\n",Calc_CRC);
		return;
	}
	else
	{
		//printf("Received CRC       :0x%06lX\n",Recv_CRC);
		INIT_Received[Source_Index] = RECEIVED;
		Received_Init[Source_Index] = 1;
	}

	Recv_App_CRC = ExtractBits(ucBit, 16, (unsigned char *)&Buff[29]);


	if(Recv_App_CRC != EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][Source_Index])
	{
		App_Identity_CRC_Fail[Source_Index] = 1;
		printf("Received App Identity CRC is not Matching for Peer:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
		printf("Received App Identity CRC:0X%04X\n",Recv_App_CRC);
		printf("Expected App Identity CRC:0X%04X\n",EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][Source_Index]);

		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0)
		{
			return;
		}
	}
	else
	{
		App_Identity_CRC_Fail[Source_Index] = 0;
	}

	Recv_Inputs = ExtractBits(ucBit, 8, (unsigned char *)&Buff[36]);
	if(Recv_Inputs != EI_data.Hitachi_st.Bool_outputs[Source_Index])
	{
		printf("Vital Inputs Not Matching for Peer:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
		printf("Expected Inputs:%u\n",EI_data.Hitachi_st.Bool_outputs[Source_Index]);
		printf("Received Inputs:%u\n",Recv_Inputs);

		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0)
		{
			//Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = 0x123456;
			return;
		}
	}


	Last_Recevied_Clock[Source_Index] = Cur_Ticks;
	//Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = HAL_Elaps_Ticks(Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff); // To detected EiPC shifting Faster
	Received_Init[Source_Index] = SET;
	Init_Msg_Sent_After_Timeout[Source_Index] = 0;
	Last_Received_Init_Clk[Source_Index] = Cur_Ticks;
	//	Recv_Init_Msg_cnt[Source_Index]++;
	//	if(Recv_Init_Msg_cnt[Source_Index] == 1)
	//	{
	//		Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = 0;
	//		if(Cur_Ticks > TEN_SEC)
	//		{
	//			Link_Establish_Clock[Source_Index] = (Cur_Ticks- FOUR_SEC);
	//
	//		}
	//		else
	//		{
	//			Link_Establish_Clock[Source_Index] = TWO_SEC;
	//		}
	//	}

	BITSTUFFINC(8); // Incrementing to escape SOM

	Hitachi_Init_st[Source_Index].Des_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	printf("DA:%u ",Hitachi_Init_st[Source_Index].Des_Addr);
	BITSTUFFINC(16);


	Hitachi_Init_st[Source_Index].Src_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	printf("SA:%u ",Hitachi_Init_st[Source_Index].Src_Addr);
	BITSTUFFINC(16);

	Hitachi_Init_st[Source_Index].Tx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Tx_seq             :%u\n",Hitachi_Init_st[Source_Index].Tx_Seq);
	BITSTUFFINC(8);


	Hitachi_Init_st[Source_Index].Rx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Rx_seq             :%u\n", Hitachi_Init_st[Source_Index].Rx_Seq);
	BITSTUFFINC(8);


	Hitachi_Init_st[Source_Index].Msg_Type = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_Type           :%u\n", Hitachi_Init_st[Source_Index].Msg_Type);
	BITSTUFFINC(8);


	Hitachi_prms_st[Source_Index].Msg_Status_Flag = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	BITSTUFFINC(16);
	//printf("Msg_sts_flag       :0x%04X\n", Hitachi_prms_st[Source_Index].Msg_Status_Flag);

	//Debug_Status_Flag(Hitachi_prms_st[Source_Index].Msg_Status_Flag,Hitachi_Init_st[Source_Index].Msg_Type);

	if ( CHECKFLAG(TIME_STAMP,Hitachi_prms_st[Source_Index].Msg_Status_Flag)== SET) // Time-stamp Extraction
	{
		// printf("Time Included in the Data field\n");
		HITACHI_time_Stamp_st[Source_Index].YY = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("YEAR               :0x%02X\n",HITACHI_time_Stamp_st[Source_Index].YY);

		HITACHI_time_Stamp_st[Source_Index].MM = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Month              :0x%02X\n",HITACHI_time_Stamp_st[Source_Index].YY);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].DD = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Day                :0x%02X\n",HITACHI_time_Stamp_st[Source_Index].DD);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].hh = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("HOURS              :0x%02X\n",HITACHI_time_Stamp_st[Source_Index].hh);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].mm = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("MIN                :0x%02X\n",HITACHI_time_Stamp_st[Source_Index].mm);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].ss = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("SEC                :0x%02X\n",HITACHI_time_Stamp_st[Source_Index].ss);
		BITSTUFFINC(8);

		(IS_MLK_EIPC_TIME_MATCHING(Source_Index))?(Change_MLK_TIME[Source_Index]=0):(Change_MLK_TIME[Source_Index]=1);
	}

	Hitachi_Init_st[Source_Index].Init_Size = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("sizofInitData      :0x%02X\n", Hitachi_Init_st[Source_Index].Init_Size);
	BITSTUFFINC(8);


	Hitachi_Init_st[Source_Index].Protocol_Ver = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("Protocol_Ver       :0x%04X\n",Hitachi_Init_st[Source_Index].Protocol_Ver);
	BITSTUFFINC(16);

	memcpy(Hitachi_Init_st[Source_Index].Exe_Ver,&Buff[ucByte],6);
	//	printf("Executive Version  :0x%02X%02X%02X%02X%02X%02X\n",Hitachi_Init_st[Source_Index].Exe_Ver[0],Hitachi_Init_st[Source_Index].Exe_Ver[1],
	//			Hitachi_Init_st[Source_Index].Exe_Ver[2],Hitachi_Init_st[Source_Index].Exe_Ver[3],
	//			Hitachi_Init_st[Source_Index].Exe_Ver[4],Hitachi_Init_st[Source_Index].Exe_Ver[5]);

	BITSTUFFINC(16);
	BITSTUFFINC(16);
	BITSTUFFINC(16);
	// printf("Skip Exe_ver\n");

	Hitachi_Init_st[Source_Index].App_comp = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("Application_comp   :0x%04X\n",Hitachi_Init_st[Source_Index].App_comp);
	BITSTUFFINC(16);

	Hitachi_Init_st[Source_Index].Exe_Idty_CRC = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//	printf("Executive_Idty_CRC :0x%04X\n",Hitachi_Init_st[Source_Index].Exe_Idty_CRC);
	BITSTUFFINC(16);

	Hitachi_Init_st[Source_Index].App_Idty_CRC = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("App_Idty_CRC       :%04u\n",Hitachi_Init_st[Source_Index].App_Idty_CRC);
	BITSTUFFINC(16);

	Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = ExtractBits(ucBit, 24, (unsigned char *)&Buff[ucByte]);
	//printf("Vital_CRC_Init_Val :0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
	Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value;
	BITSTUFFINC(24);

	Hitachi_prms_st[Source_Index].Seq_Init_Value = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	Hitachi_prms_st[Source_Index].Rx_Seq = Hitachi_prms_st[Source_Index].Seq_Init_Value;
	//printf("Seq_Init_Val       :%u\n",Hitachi_prms_st[Source_Index].Seq_Init_Value);
	BITSTUFFINC(8);

	Hitachi_Init_st[Source_Index].V_NV_Flag = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("V_NV_Flag          :%u\n",Hitachi_Init_st[Source_Index].V_NV_Flag);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Sz_Bool_V_Data     :%u\n", Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA);
	BITSTUFFINC(8);




	if(Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA != EI_data.Hitachi_st.Bool_outputs[Source_Index])
	{
		printf("Vital Inputs Not Matching\n");
		printf("Expected Inputs:%u\n",EI_data.Hitachi_st.Bool_outputs[Source_Index]);
		printf("Received Inputs:%u\n",Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA);
	}

	Hitachi_Init_st[Source_Index].Sz_Num_Val_Data = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Sz_Num_Val_Data    :%u\n",Hitachi_Init_st[Source_Index].Sz_Num_Val_Data);
	BITSTUFFINC(8);


	Hitachi_Init_st[Source_Index].Val_Bool_Ops = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Val_Bool_Ops       :%u\n",Hitachi_Init_st[Source_Index].Val_Bool_Ops);
	BITSTUFFINC(8);
	if(Hitachi_Init_st[Source_Index].Val_Bool_Ops != 1)
	{
		printf("Transmitted size of Numerical Received:%u\n",Hitachi_Init_st[Source_Index].Val_Bool_Ops);
		printf("This Value has to be \"1\"\n");
	}

	Hitachi_Init_st[Source_Index].Val_Num_Ops = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Val_Num_Ops        :%u\n",Hitachi_Init_st[Source_Index].Val_Num_Ops);
	BITSTUFFINC(8);

	Hitachi_Init_st[Source_Index].Init_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[ucByte]);
	//printf("Received CRC       :0x%08lX\n",Hitachi_Init_st[Source_Index].Init_CRC);
	BITSTUFFINC(32);

	//Calc_CRC = CalculateDirectReverseCrc(Buff, Length - 5);

	//printf("\n___________________________________ INIT-MSG ___________________________________\n");
	Send_Init_Msg(Source_Index,Des_Index);

	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_CRC = Recv_CRC;
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_Msg_Type = INIT_MSG;
	Tx_Ei_self_sharing_st[Source_Index].Peer_Index = Source_Index;
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = RECEIVED;
	memset(&Temp_Tx_Ei_self_sharing_st,0,sizeof(EI_SELF_SHARING_t));
	memcpy(&Temp_Tx_Ei_self_sharing_st,&Tx_Ei_self_sharing_st[Source_Index],sizeof(EI_SELF_SHARING_t));


#ifdef EI_1
	TransmitCanPkt(EI_1_SELF_SHARING); //for Immediate sharing of VItal CRC Init Value.
#endif

#ifdef EI_2
	TransmitCanPkt(EI_2_SELF_SHARING); //for Immediate sharing of VItal CRC Init Value.
#endif

	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = NOT_RECEIVED;

	Self_Sharing_CAN_Clk[Source_Index] = HAL_GetTick();
}

void Send_Init_Msg(uint8_t Source_Index,uint8_t Des_Index)
{

	uint8_t Transmit_Buff[60] = {0,};
	uint32_t Calc_CRC = 0;

	BITSTUFFINIT(0, 0);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], 0xf4);
	BITSTUFFINC(8);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.DEST_ADDRESSES[Source_Index]);
	BITSTUFFINC(16);

	if( (CHECKFLAG(2,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		if(Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index)
		{
			uint8_t Rand_Number  = 0;
#ifdef STM_32
			Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 200 )+1;
#endif
			printf("Injected TX Sequence value failure\n");
			printf("Actual   TX Sequence value:%u\n",0);
			printf("Injected TX Sequence value:%u\n",Rand_Number);
			InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_Number); // Rx Seq Number
			//Last Received Packet Tx_seq number is our Rx_Seq Number for VITAL_Message we don't Increment Sequence Number.
		}
	}
	else
	{
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], 0x00);
	}
	BITSTUFFINC(8);



	if( (CHECKFLAG(3,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		if(Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index)
		{
			uint8_t Rand_Number  = 0;
#ifdef STM_32
			Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 200 );
#endif
			printf("Injected RX Sequence value failure\n");
			printf("Actual   RX Sequence value:%u\n",Hitachi_prms_st[Source_Index].Rx_Seq);
			printf("Injected RX Sequence value:%u\n",Rand_Number);
			InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_Number); // Rx Seq Number
			//Last Received Packet Tx_seq number is our Rx_Seq Number for VITAL_Message we don't Increment Sequence Number.
		}

	}
	else
	{
		// Hitachi_prms_st[Source_Index].Rx_Seq = Hitachi_prms_st[Source_Index].Seq_Init_Value + 1;
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], ++Hitachi_prms_st[Source_Index].Rx_Seq);
	}
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], INIT_MSG);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Tx_Msg_Status_Flag = 0x0005;
	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Tx_Msg_Status_Flag);
	BITSTUFFINC(16);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], INIT_MSG_SIZE);
	BITSTUFFINC(8);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EIPC_PCOL_VER);
	BITSTUFFINC(16);

	//As Execuctive version is 6Bytes we divided in two parts as 32-bit AND 16-bit
	InsertBits(ucBit, 32, &Transmit_Buff[ucByte], EIPC_EXE_VER_1);
	BITSTUFFINC(32);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EIPC_EXE_VER_2);
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EIPC_APP_COMP_STAMP);
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EIPC_EXE_IDEN_CRC);
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EIPC_APP_IDEN_CRC);
	BITSTUFFINC(16);

	//we are sending VITAL CRC INIT VALUE by adding 1 to the Vital_CRC_Init_Value in INIT_MESSAGE from Hitachi
	if(Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value >= 0xfffff0)
	{
		Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value = Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value - 1;
	}
	else
	{
		Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value = Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value + 1;
	}

	InsertBits(ucBit, 24, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
	Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value;
	BITSTUFFINC(24);

	++Self_Seq_Init_Value[Source_Index];
	if((Self_Seq_Init_Value[Source_Index]>= 180) || (Self_Seq_Init_Value[Source_Index] == 0) )
	{
		Self_Seq_Init_Value[Source_Index] = 10;
	}

	//printf("\nSelf Seq Init Value:%u\n",Self_Seq_Init_Value[Source_Index]);
	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Self_Seq_Init_Value[Source_Index]);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], V_NV_FLAG);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], SIZE_BOOL_VAL_DATA);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], SIZE_NUMER_VAL_DATA);
	BITSTUFFINC(8);

	//printf("Hitachi_fault_injection_st.FAULT_INJECTION_BITS:%lu\n",Hitachi_fault_injection_st.FAULT_INJECTION_BITS);
	//printf("Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX:%u\n",Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX);

	if( ( (CHECKFLAG(5,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_Number = 12;
#ifdef STM_32
		Rand_Number  = (HAL_RNG_GetRandomNumber(&hrng) % 125) + 1;
#endif
		printf("Injecting Invalid Boolean value Outputs failure\n");
		printf("Actual Boolean value:%u\n",Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA);
		if(Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA == Rand_Number)
		{
			Rand_Number = Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA-2;
		}
		printf("Injected Boolean value:%u\n",Rand_Number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_Number);
	}
	else
	{
		//InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Size_of_VITAL_DATA);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], EI_data.Hitachi_st.Bool_outputs[Source_Index]);
	}
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], VAL_NUM_OUTPUTS);
	BITSTUFFINC(8);

	Calc_CRC = CalculateDirectReverseCrc(Transmit_Buff, ucByte);
	InsertBits(ucBit, 32, &Transmit_Buff[ucByte], Calc_CRC);
	BITSTUFFINC(32);

	if( ( CHECKFLAG(9,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)== SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		printf("Injected ETH tx Fail\n");
		printf("Not Sending Reply");
	}
	else
	{
		Process_Sending_Reply(Transmit_Buff,ucByte,Source_Index);
	}
}

void Vital_Msg_extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index)
{
	uint16_t Traverser = 0;
	uint32_t Calc_CRC = 0;
	uint32_t Recv_CRC = 0;
	uint32_t Ticks = HAL_GetTick();
	uint8_t noofIP = 0, uc_index = 0;
	uint8_t LinkEstablishFlag = RESET;
	uint8_t Temp_Vital_Data_Buff[HITACHI_MAX_PAYLOAD] = {0,};
	BITSTUFFINIT(0, 0);


	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		//Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = 0x123456;
		goto ROLLOVER_VITAL;
	}


	if( ( (CHECKFLAG(0,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint32_t Rand_Number = 22;
#ifdef STM_32
		Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 0xffff00);
#endif
		printf("\nInjected RX Vital CRC failure\n");
		printf("Actual   RX Vital Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
		printf("Injected RX Vital Init Value:0x%06lX\n",Rand_Number);
		InsertBits(0, 24, &Temp_Vital_Data_Buff[0],Rand_Number);
	}
	else
	{
		InsertBits(0, 24, &Temp_Vital_Data_Buff[0], Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
	}
	for (uc_index = 0; uc_index < (Length-5); uc_index++)
	{
		Temp_Vital_Data_Buff[uc_index + 3] = Buff[uc_index];
	}
	Calc_CRC = CalculateDirectReverseCrc(Temp_Vital_Data_Buff, uc_index + 3);
	Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length-5]);

	//	printf("\n___________________________________ VITAL-MSG ___________________________________\n");
	if(Calc_CRC != Recv_CRC)
	{
		if( ( ExtractBits(0, 8, (unsigned char *)&Buff[5]) == 0x00) || (ExtractBits(0, 8, (unsigned char *)&Buff[5]) == 0x01) )
		{
			printf("Tx seq Roll over Detected\n");
			memset(Temp_Vital_Data_Buff,0,sizeof(Temp_Vital_Data_Buff));
			InsertBits(0, 24, &Temp_Vital_Data_Buff[0], (Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value+1));
			for (uc_index = 0; uc_index < (Length-5); uc_index++)
			{
				Temp_Vital_Data_Buff[uc_index + 3] = Buff[uc_index];
			}
			Calc_CRC = CalculateDirectReverseCrc(Temp_Vital_Data_Buff, uc_index + 3);
			Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length-5]);

			if(Calc_CRC == Recv_CRC)
			{
				printf("CRC OK AFTER ROLL OVER\n");
				printf("Received CRC       :0x%08lX\n",Recv_CRC);
				printf("Previous Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
				++Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value;
				printf("Incremented Vital_CRC Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
				Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value;
				goto ROLLOVER_VITAL;
			}
			else
			{
				printf("Vital CRC Fail_Rollover:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				printf("R-CRC:0x%08lX\n",Recv_CRC);
				printf("C-CRC:0x%08lX\n",Calc_CRC);
				return;
			}
		}
		else
		{
			printf("Vital CRC Fail:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
			printf("R-CRC:0x%08lX\n",Recv_CRC);
			printf("C-CRC:0x%08lX\n",Calc_CRC);
			return;
		}
	}

	ROLLOVER_VITAL:
	Last_Recevied_Clock[Source_Index] = Ticks;
	Last_Recevied_Clock_ValidMsgs[Source_Index] = Ticks;
	Received_Vital[Source_Index] = SET;
	//Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = HAL_Elaps_Ticks(Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff); // To detected EiPC shifting Faster

	//------| To have a copy and check if the data received on Ethernet and CAN are same.
	//------| If both same no need extract data again in CAN.
	//------| This buffer will be cleared in VITAL-MSG-CAN Function After checking.
	memset(&Last_Rcv_Vdata_st[Source_Index].Last_Rcv_vdata,0,sizeof(Last_Rcv_Vdata_st[Source_Index].Last_Rcv_vdata));
	memcpy(&Last_Rcv_Vdata_st[Source_Index].Last_Rcv_vdata,Buff,Length);
	Last_Rcv_Vdata_st[Source_Index].Length = Length;
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	BITSTUFFINC(8); // Incrementing to escape SOM

	Hitachi_Vital_st[Source_Index].Des_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	printf("DA:%u ", Hitachi_Vital_st[Source_Index].Des_Addr);
	BITSTUFFINC(16);

	Hitachi_Vital_st[Source_Index].Src_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	printf("SA:%u ", Hitachi_Vital_st[Source_Index].Src_Addr);
	BITSTUFFINC(16);

	Hitachi_prms_st[Source_Index].Rx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Tx_seq             :%u\n", Hitachi_prms_st[Source_Index].Rx_Seq);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Tx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Rx_seq             :%u\n", Hitachi_prms_st[Source_Index].Tx_Seq);
	if(Hitachi_prms_st[Source_Index].Tx_Seq == 0)
	{
		printf("\nRX_Seq Roll over Detected Incremented self Vital Init Value:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
		printf("Prev Val:%06lx:",Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
		++Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value;
		printf("Cur Val:%06lx\n",Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
		Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value;
	}
	BITSTUFFINC(8);

	Hitachi_Vital_st[Source_Index].Msg_Type = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_Type           :%u\n",Hitachi_Vital_st[Source_Index].Msg_Type);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Msg_Status_Flag= ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);

	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		SET_BIT_USER(LINK_ESTABLISHED,Hitachi_prms_st[Source_Index].Msg_Status_Flag);
	}
	//printf("Msg_sts_flag       :0x%04X\n", Hitachi_prms_st[Source_Index].Msg_Status_Flag);
	BITSTUFFINC(16);

	//Debug_Status_Flag(Hitachi_prms_st[Source_Index].Msg_Status_Flag, Hitachi_Vital_st[Source_Index].Msg_Type);

	if ( ( SET == (CHECKFLAG(TIME_STAMP,Hitachi_prms_st[Source_Index].Msg_Status_Flag))) ) // Time-stamp Extraction
	{
		HITACHI_time_Stamp_st[Source_Index].YY = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("YEAR               :%02X\n", HITACHI_time_Stamp_st[Source_Index].YY);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].MM = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Month              :%02X\n", HITACHI_time_Stamp_st[Source_Index].MM);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].DD = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Date               :%02X\n", HITACHI_time_Stamp_st[Source_Index].DD);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].hh = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("HOURS              :%02X\n", HITACHI_time_Stamp_st[Source_Index].hh);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].mm = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("MINS               :%02X\n", HITACHI_time_Stamp_st[Source_Index].mm);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].ss = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("SEC                :%02X\n", HITACHI_time_Stamp_st[Source_Index].ss);
		BITSTUFFINC(8);

		(IS_MLK_EIPC_TIME_MATCHING(Source_Index))?(Change_MLK_TIME[Source_Index]=0):(Change_MLK_TIME[Source_Index]=1);

	}
	if ( ( SET == (CHECKFLAG(LINK_ESTABLISHED,Hitachi_prms_st[Source_Index].Msg_Status_Flag))) )
	{
		LinkEstablishFlag = SET;
	}

	Hitachi_Vital_st[Source_Index].No_Outputs = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	BITSTUFFINC(8);
	printf("OP:%u ", Hitachi_Vital_st[Source_Index].No_Outputs);

	Hitachi_Vital_st[Source_Index].Size_Num_val = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	BITSTUFFINC(8);
	//printf("Sz_Num_Val_Data    :%u\n", Hitachi_Vital_st[Source_Index].Size_Num_val);

	noofIP = (Hitachi_Vital_st[Source_Index].No_Outputs % 8);
	if (noofIP != 0)
	{
		noofIP = ((Hitachi_Vital_st[Source_Index].No_Outputs / 8)) + 1; //	including 0th buffer
	}
	else
	{
		noofIP = (Hitachi_Vital_st[Source_Index].No_Outputs / 8);
	}
	for (Traverser = 0; Traverser < noofIP; ++Traverser)
	{
		Hitachi_Vital_st[Source_Index].Outputs[Traverser] = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		printf("[%02u]:%02X ", Traverser,Hitachi_Vital_st[Source_Index].Outputs[Traverser]);
		BITSTUFFINC(8);
	}
	printf("\n");
	if(EI_data.Hitachi_st.Interface_Type[Source_Index] ==  SERIAL)
	{
		printf("Online Bit Status:%u\n",CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]));
	}

	for (Traverser = 0; Traverser < Hitachi_Vital_st[Source_Index].Size_Num_val; ++Traverser)
	{
		Hitachi_Vital_st[Source_Index].Num_Outputs[Traverser] = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Num op Values      :0x%02X\n",  Hitachi_Vital_st[Source_Index].Num_Outputs[Traverser]);
		BITSTUFFINC(8);
	}

	/* Reply to Vital Message */
	Send_Service_Signal_Message(Source_Index,Des_Index);
	/* Reply to Vital Message */

	/* To Send VITAL Message after Some Time say 500ms Later */
	Send_Vital_MSG_st.flag[Source_Index] = SET;
	Send_Vital_MSG_st.Clk[Source_Index] = Ticks;
	/* To Send VITAL Message after Some Time say 500ms Later */

	/*To Send VITAL DATA MESSAGE TO OTHER CARD*/
	if(Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY == RECEIVED)
	{
		memset(&Tx_VITAL_MSG_CAN_st,0,sizeof(SEND_VITAL_MSG_2_OTH_t));
		Tx_VITAL_MSG_CAN_st.Source_Peer_Index = Source_Index;
		Tx_VITAL_MSG_CAN_st.Packet_Length = Length;
		memcpy(Tx_VITAL_MSG_CAN_st.VITAL_MESSAGE,Buff,Length);
#ifdef EI_1
		TransmitCanPkt(SEND_VITAL_MSG_1);
#endif
#ifdef EI_2
		TransmitCanPkt(SEND_VITAL_MSG_2);
#endif
	}
	/*To Send VITAL DATA MESSAGE TO OTHER CARD*/


	/* To check Whether the Data received is from Active system or Not By checking the 1st bit Vital Data
	 * This checking is Only required for Serial Interface
	 */

	if((EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL) && (CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]) == RESET))
	{
		printf("Online Bit Drop\n");
		LinkEstablishFlag = RESET;
		Act_InactiveFlag[Source_Index] = SET;
		LastActCount[Source_Index] = RESET;
		memset(&Hitachi_Vital_st[Source_Index].Outputs,0,sizeof(Hitachi_Vital_st[Source_Index].Outputs));
	}
	else if((EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL) && (CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]) == SET) && (LinkEstablishFlag == SET))
	{
		if(Act_InactiveFlag[Source_Index] == SET)
		{
			++LastActCount[Source_Index];
			printf("Updated Count:%u\n",LastActCount[Source_Index]);
			if(LastActCount[Source_Index] >= MAX_LAST_ACT_COUNT)
			{
				Act_InactiveFlag[Source_Index] = RESET;
				LastActCount[Source_Index] = MAX_LAST_ACT_COUNT; /* To avoid Overflow of Variable */
			}
			else
			{
				LinkEstablishFlag = RESET;
				memset(&Hitachi_Vital_st[Source_Index].Outputs,0,sizeof(Hitachi_Vital_st[Source_Index].Outputs));
			}
		}
	}


	/* COPYING RELAYS INFO 2 Temporary Buffer */
	memset(&Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Relays_st[Source_Index].RelayIPS_Buff));
	memcpy(Relays_st[Source_Index].RelayIPS_Buff,Hitachi_Vital_st[Source_Index].Outputs,noofIP);
	Relays_st[Source_Index].RelayIPS_CRC = GetCRC16(0,Relays_st[Source_Index].RelayIPS_Buff,noofIP);
	Relays_st[Source_Index].No_ofIps = noofIP;
	Relays_st[Source_Index].Peer_Index = Source_Index;
	/* COPYING RELAYS INFO 2 Temporary Buffer */


	/*	Checking whether the Link is Established or Not
	 *	If Link is Established and CRC's Differ then only Copy to Main Buffer & Send to other EIPC
	 */
	if(LinkEstablishFlag == SET)
	{
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		memcpy(&Main_Relays_st[Source_Index],&Relays_st[Source_Index],sizeof(Relays_st[Source_Index]));
		Last_Vital_Received_Clk[Source_Index] = Ticks;

		if(Prev_Relay_CRC[Source_Index] != Main_Relays_st[Source_Index].RelayIPS_CRC)
		{
			Prev_Relay_CRC[Source_Index] = Main_Relays_st[Source_Index].RelayIPS_CRC;
			memset(&Tx_Temp_Main_Relays_st,0,sizeof(Tx_Temp_Main_Relays_st));
			memcpy(&Tx_Temp_Main_Relays_st,&Main_Relays_st[Source_Index],sizeof(Tx_Temp_Main_Relays_st));
#ifdef EI_1
			TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
			TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif

			if(MisMatchFlag[Source_Index] == RESET)
			{
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Source_Index * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Source_Index].RelayIPS_Buff,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
#ifdef EI_1

#if ENABLE_SIGNAL_PRINT
				PrintSignalsHitachi();
#endif
				ReArrangedVitalData();
				TransmitCanPkt(EI_PROCESS_DATA);
				MIE_Signal_Send_CLK = Ticks;
#endif
			}
		}
	}

	/*	Sending Message Details to Other Card */
	Tx_Ei_self_sharing_st[Source_Index].VITAL_DATA_CRC = Main_Relays_st[Source_Index].RelayIPS_CRC;
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_CRC = Recv_CRC;
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_Msg_Type = VDATAMSG;
	Tx_Ei_self_sharing_st[Source_Index].Peer_Index = Source_Index;
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = RECEIVED;
	Tx_Ei_self_sharing_st[Source_Index].LinkEstablishFlag = LinkEstablishFlag;
	memset(&Temp_Tx_Ei_self_sharing_st,0,sizeof(EI_SELF_SHARING_t));
	memcpy(&Temp_Tx_Ei_self_sharing_st,&Tx_Ei_self_sharing_st[Source_Index],sizeof(EI_SELF_SHARING_t));

#ifdef EI_1
	TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
	TransmitCanPkt(EI_2_SELF_SHARING);
#endif
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = NOT_RECEIVED;
	Self_Sharing_CAN_Clk[Source_Index] = Ticks;
	/*	Sending Message Details to Other Card */


	/* Checking the Link Status */
	if(LinkEstablishFlag == SET)
	{
		Link_Establish_Clock[Source_Index] = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishClk = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishFlag = SET;
		SelfLinkFlag[Source_Index] = SET;
	}
	else
	{
		SelfLinkFlag[Source_Index] = RESET;
		Hitachi_clk_flags_st[Source_Index].Link_FailClk = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishFlag = RESET;
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		Main_Relays_st[Source_Index].RelayIPS_CRC = 0;
	}
	/* Checking the Link Status */

	HAL_GPIO_TogglePin(S1_LED_GPIO_Port, S1_LED_Pin);

}

void Send_Vital_Data_Message(uint8_t Source_Index,uint8_t Des_Index)
{
	uint8_t Transmit_Buff[60] = {0,};
	uint8_t Temp_Buff[60] = {0,};
	uint8_t uc_index = 0;

	BITSTUFFINIT(0, 0);

	memset(Transmit_Buff,0,sizeof(Transmit_Buff));

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_SOM);
	BITSTUFFINC(8);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.DEST_ADDRESSES[Source_Index]);
	BITSTUFFINC(16);


	if( ( (CHECKFLAG(2,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_Number = 0;
#ifdef STM_32
		Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 200);
#endif
		printf("Injected Tx Sequence value failure\n");
		printf("Actual   Tx Sequence vale:%u\n",Hitachi_prms_st[Source_Index].Tx_Seq);
		printf("Injected Tx Sequence value:%u\n",Rand_Number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_Number);
	}
	else
	{
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Tx_Seq); // TX Seq Number
	}
	//Last Received Packet Rx_seq number is our TX_Seq Number
	BITSTUFFINC(8);


	if( ( (CHECKFLAG(3,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_Number  = 0;
#ifdef STM_32
		Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 200 );
#endif
		printf("Injected RX Sequence value failure\n");
		printf("Actual   RX Sequence value:%u\n",Hitachi_prms_st[Source_Index].Rx_Seq);
		printf("Injected RX Sequence value:%u\n",Rand_Number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_Number); // Rx Seq Number
		//Last Received Packet Tx_seq number is our Rx_Seq Number for VITAL_Message we don't Increment Sequence Number.
	}
	else
	{
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Rx_Seq); // Rx Seq Number
		//Last Received Packet Tx_seq number is our Rx_Seq Number for VITAL_Message we don't Increment Sequence Number.
	}
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], VDATAMSG);
	BITSTUFFINC(8);

	//Inserting Message Status Flag
	//InsertBits(ucBit, 16, &Transmit_Buff[ucByte], 0x002C);


	if(Change_MLK_TIME[Source_Index]== 1)
	{
		Get_time();
		memcpy(&HITACHI_time_Stamp_st[Source_Index],&RTC_Date_time_BCD_st,sizeof(GPS_DATE_TIME_t));
		printf("Sending Updated Time\n");
		Change_MLK_TIME[Source_Index] = 0;
		InsertBits(ucBit, 16, &Transmit_Buff[ucByte], 0x0068);
		BITSTUFFINC(16);
	}
	else
	{
		InsertBits(ucBit, 16, &Transmit_Buff[ucByte], 0x0028);
		BITSTUFFINC(16);
	}



	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].YY);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].MM);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].DD);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].hh);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].mm);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].ss);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], TX_SIZE_BOOL_OPS);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], TX_NUM_VAL_OPS);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], TX_BOOL_VAL_STATE);
	BITSTUFFINC(8);



	//	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	//	{
	//		Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value = 0x654321;
	//	}

	if( ( (CHECKFLAG(1,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint32_t Rand_Number = 0;
#ifdef STM_32
		Rand_Number = HAL_RNG_GetRandomNumber(&hrng);
#endif
		printf("\nInjected Vital Init Value Failure\n");
		printf("Actual Vital Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
		printf("Injected Vital Init Value:0x%06lX\n",Rand_Number);
		InsertBits(0, 24, &Temp_Buff[0], Rand_Number);
	}
	else
	{
		InsertBits(0, 24, &Temp_Buff[0], Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
	}
	for (uc_index = 0; uc_index < ucByte; uc_index++)
	{
		Temp_Buff[uc_index + 3] = Transmit_Buff[uc_index];
	}

	InsertBits(ucBit, 32, &Transmit_Buff[ucByte], CalculateDirectReverseCrc(Temp_Buff, uc_index + 3));
	BITSTUFFINC(32);

#if ENABLE_FAULT_INJECTION
	if( ( CHECKFLAG(9,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)== SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		printf("Injected ETH tx Fail\n");
		printf("Not Sending Reply");
	}
	else
	{
		Process_Sending_Reply(Transmit_Buff,ucByte,Source_Index);
	}
#else
	Process_Sending_Reply(Transmit_Buff,ucByte,Source_Index);
#endif
}


void Vital_Msg_extr_from_CAN(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index)
{
	uint16_t Traverser = 0;
	uint32_t Calc_CRC = 0;
	uint32_t Recv_CRC = 0;
	uint32_t Ticks = HAL_GetTick();
	uint8_t noofIP = 0, uc_index = 0;
	uint8_t LinkEstablishFlag = 0;
	uint8_t Temp_Vital_Data_Buff[128] = {0,};
	HITACHI_VITAL_t CAN_Hitachi_Vital_st;
	HITACHI_PARAMETERS_t CAN_Hitachi_Parameters_t;
	HITACHI_TIME_t CAN_HITACHI_time_Stamp_st;
	RELAYS_t CAN_Relay_st;

	BITSTUFFINIT(0, 0);

	if(Length == Last_Rcv_Vdata_st[Source_Index].Length)
	{
		if(memcmp(Buff,Last_Rcv_Vdata_st[Source_Index].Last_Rcv_vdata,Length) == 0)
		{
			printf("\nBoth Ethernet Vdata Msg and CAN Vdata Msg Matched for Peer:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
			return;
		}
	}

	Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length-5]);


#if ENABLE_FAULT_INJECTION
	if( ( (CHECKFLAG(0,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint32_t Rand_Number = 22;
		Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 0xffff00);
		printf("\nInjected RX Vital CRC failure\n");
		printf("Actual   RX Vital Init Value:0x%06lX\n",Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);
		printf("Injected RX Vital Init Value:0x%06lX\n",Rand_Number);
		InsertBits(0, 24, &Temp_Vital_Data_Buff[0],Rand_Number);
	}
	else
	{
		InsertBits(0, 24, &Temp_Vital_Data_Buff[0], Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);
	}
#else
	InsertBits(0, 24, &Temp_Vital_Data_Buff[0], Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);
#endif
	for (uc_index = 0; uc_index < (Length-5); uc_index++)
	{
		Temp_Vital_Data_Buff[uc_index + 3] = Buff[uc_index];
	}
	Calc_CRC = CalculateDirectReverseCrc(Temp_Vital_Data_Buff, uc_index + 3);

	if(Calc_CRC != Recv_CRC)
	{
		if( ExtractBits(0, 8, (unsigned char *)&Buff[5]) == 0x00)
		{
			printf("Tx seq Roll over Detected\n");
			memset(Temp_Vital_Data_Buff,0,sizeof(Temp_Vital_Data_Buff));
			InsertBits(0, 24, &Temp_Vital_Data_Buff[0], (Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val+1));
			for (uc_index = 0; uc_index < (Length-5); uc_index++)
			{
				Temp_Vital_Data_Buff[uc_index + 3] = Buff[uc_index];
			}
			Calc_CRC = CalculateDirectReverseCrc(Temp_Vital_Data_Buff, uc_index + 3);
			Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length-5]);

			if(Calc_CRC == Recv_CRC)
			{
				printf("CRC OK AFTER ROLL OVER @VITAL_CAN\n");
				printf("Received CRC       :0x%08lX\n",Recv_CRC);
				printf("Previous Init Value:%lu\n",Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);
				Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = ++Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val;
				printf("Incremented Vital_CRC Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
				Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value;
				goto ROLLOVER_VITAL_CAN;
			}
			else
			{
				printf("CAN VITAL MSG CRC fail:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				printf("R-CRC:0x%08lX\n", Recv_CRC);
				printf("C-CRC:0x%08lX\n", Calc_CRC);
				return;
			}
		}
		else
		{
			printf("CAN VITAL MSG CRC fail:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
			printf("R-CRC:0x%08lX\n", Recv_CRC);
			printf("C-CRC:0x%08lX\n", Calc_CRC);
			return;
		}
	}
	ROLLOVER_VITAL_CAN:
	//printf("Actual   RX Vital Init Value:0x%06lX\n",Rx_Ei_self_sharing_st.Vital_CRC_Init_Value[Source_Index]);
	BITSTUFFINC(8); // Incrementing to escape SOM

	CAN_Hitachi_Vital_st.Des_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	printf("DA:%u ",CAN_Hitachi_Vital_st.Des_Addr);
	BITSTUFFINC(16);

	CAN_Hitachi_Vital_st.Src_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	printf("SA:%u ",CAN_Hitachi_Vital_st.Src_Addr);
	BITSTUFFINC(16);

	CAN_Hitachi_Parameters_t.Tx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Tx_seq             :%u\n", CAN_Hitachi_Parameters_t.Tx_Seq);
	BITSTUFFINC(8);

	CAN_Hitachi_Parameters_t.Rx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Rx_seq             :%u\n", CAN_Hitachi_Parameters_t.Rx_Seq);
	BITSTUFFINC(8);

	CAN_Hitachi_Vital_st.Msg_Type = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_Type           :%u\n",CAN_Hitachi_Vital_st[Source_Index].Msg_Type);
	BITSTUFFINC(8);

	CAN_Hitachi_Parameters_t.Msg_Status_Flag= ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_sts_flag       :0x%04X\n", CAN_Hitachi_Parameters_t.Msg_Status_Flag);
	BITSTUFFINC(16);

	//Debug_Status_Flag(CAN_Hitachi_Parameters_t.Msg_Status_Flag,CAN_Hitachi_Vital_st.Msg_Type);

	if ( ( SET == (CHECKFLAG(TIME_STAMP,CAN_Hitachi_Parameters_t.Msg_Status_Flag))) ) // Time-stamp Extraction
	{
		CAN_HITACHI_time_Stamp_st.YY = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("YEAR               :0x%02X\n", CAN_HITACHI_time_Stamp_st.YY);
		BITSTUFFINC(8);

		CAN_HITACHI_time_Stamp_st.MM = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Month              :0x%02X\n", CAN_HITACHI_time_Stamp_st.MM);
		BITSTUFFINC(8);

		CAN_HITACHI_time_Stamp_st.DD = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Date               :0x%02X\n", CAN_HITACHI_time_Stamp_st.DD);
		BITSTUFFINC(8);

		CAN_HITACHI_time_Stamp_st.hh = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("HOURS              :0x%02X\n", CAN_HITACHI_time_Stamp_st.hh);
		BITSTUFFINC(8);

		CAN_HITACHI_time_Stamp_st.mm = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("MINS               :0x%02X\n", CAN_HITACHI_time_Stamp_st.mm);
		BITSTUFFINC(8);

		CAN_HITACHI_time_Stamp_st.ss = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("SEC                :0x%02X\n", CAN_HITACHI_time_Stamp_st.ss);
		BITSTUFFINC(8);

		//		(IS_MLK_EIPC_TIME_MATCHING(Source_Index))?(Change_MLK_TIME[Source_Index]=0):(Change_MLK_TIME[Source_Index]=1);
	}
	if ( ( SET == (CHECKFLAG(LINK_ESTABLISHED,CAN_Hitachi_Parameters_t.Msg_Status_Flag))) )
	{
		LinkEstablishFlag = SET;
	}


	CAN_Hitachi_Vital_st.No_Outputs = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	BITSTUFFINC(8);
	printf("OPS:%u ",CAN_Hitachi_Vital_st.No_Outputs);

	CAN_Hitachi_Vital_st.Size_Num_val = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	BITSTUFFINC(8);
	//printf("Sz_Num_Val_Data    :%u\n",CAN_Hitachi_Vital_st.Size_Num_val);

	noofIP = (CAN_Hitachi_Vital_st.No_Outputs% 8);
	if (noofIP != 0)
	{
		noofIP = ((CAN_Hitachi_Vital_st.No_Outputs / 8)) + 1; //	including 0th buffer
	}
	else
	{
		noofIP = (CAN_Hitachi_Vital_st.No_Outputs / 8);
	}
	for (Traverser = 0; Traverser < noofIP; ++Traverser)
	{
		CAN_Hitachi_Vital_st.Outputs[Traverser] = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		printf("[%02u]:0x%02X ", Traverser,CAN_Hitachi_Vital_st.Outputs[Traverser]);
		BITSTUFFINC(8);
	}
	printf("\n");

	for (Traverser = 0; Traverser <CAN_Hitachi_Vital_st.Size_Num_val; ++Traverser)
	{
		CAN_Hitachi_Vital_st.Num_Outputs[Traverser] = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Num op Values      :0x%02X\n", CAN_Hitachi_Vital_st.Num_Outputs[Traverser]);
		BITSTUFFINC(8);
	}

	/* COPYING RELAYS INFO 2 Temporary Buffer */
	memset(&CAN_Relay_st.RelayIPS_Buff,0,sizeof(CAN_Relay_st.RelayIPS_Buff));
	memcpy(CAN_Relay_st.RelayIPS_Buff,CAN_Hitachi_Vital_st.Outputs,noofIP);
	CAN_Relay_st.RelayIPS_CRC = GetCRC16(0,CAN_Relay_st.RelayIPS_Buff,noofIP);
	CAN_Relay_st.No_ofIps = noofIP;
	CAN_Relay_st.Peer_Index = Source_Index;
	/* COPYING RELAYS INFO 2 Temporary Buffer */



	/*	Checking whether the Link is Established or Not
	 *	If Link is Established and CRC's Differ then only Copy to Main Buffer & Send to other EIPC
	 */
	if ( (EI_data.Hitachi_st.Interface_Type[Source_Index] == ETHERNET) && (LinkEstablishFlag == SET) && (CAN_Relay_st.RelayIPS_CRC != Main_Relays_st[Source_Index].RelayIPS_CRC) )
	{
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		memcpy(&Main_Relays_st[Source_Index],&CAN_Relay_st,sizeof(RELAYS_t));
		Last_Vital_Received_Clk[Source_Index] = Ticks;

		if(Prev_Relay_CRC[Source_Index] != Main_Relays_st[Source_Index].RelayIPS_CRC)
		{
			Prev_Relay_CRC[Source_Index] = Main_Relays_st[Source_Index].RelayIPS_CRC;
			memset(&Tx_Temp_Main_Relays_st,0,sizeof(Tx_Temp_Main_Relays_st));
			memcpy(&Tx_Temp_Main_Relays_st,&Main_Relays_st[Source_Index],sizeof(Tx_Temp_Main_Relays_st));
#ifdef EI_1
			TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
			TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif

			if(MisMatchFlag[Source_Index] == RESET)
			{
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Source_Index * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Source_Index].RelayIPS_Buff,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
#if ENABLE_SIGNAL_PRINT
				PrintSignalsHitachi();
#endif

#ifdef EI_1
				ReArrangedVitalData();
				TransmitCanPkt(EI_PROCESS_DATA);
				MIE_Signal_Send_CLK = Ticks;

#endif
			}


		}
	}






	Tx_Ei_self_sharing_st[Source_Index].Peer_Index = Source_Index;
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = RECEIVED;
	memset(&Temp_Tx_Ei_self_sharing_st,0,sizeof(EI_SELF_SHARING_t));
	memcpy(&Temp_Tx_Ei_self_sharing_st,&Tx_Ei_self_sharing_st[Source_Index],sizeof(EI_SELF_SHARING_t));

#ifdef EI_1
	TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
	TransmitCanPkt(EI_2_SELF_SHARING);
#endif
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = NOT_RECEIVED;
	Self_Sharing_CAN_Clk[Source_Index] = Ticks;
	UNUSED(CAN_HITACHI_time_Stamp_st);
}


void Status_Msg_Extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index)
{
	uint8_t Temp_Buff[40] = {0,};
	uint8_t uc_index = 0;
	uint32_t Calc_CRC = 0;
	uint32_t Recv_CRC = 0;
	uint8_t LinkEstablishFlag = RESET;
	uint32_t Ticks = HAL_GetTick();
	uint8_t Seq_Num_2_Detect_Roll_over = 0;

	BITSTUFFINIT(0, 0);

	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		//Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = 0x123456;
		goto ROLLOVER_STATUS;
	}


#if ENABLE_FAULT_INJECTION

	if( ( (CHECKFLAG(0,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint32_t Rand_Number = 0;
#ifdef STM_32
		Rand_Number = (HAL_RNG_GetRandomNumber(&hrng) % 0xffff);
#endif
		printf("Injected RX Vital CRC failure\n");
		printf("Actual RX Vital Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
		printf("Injected RX Vital Init Value:0x%06lX\n",Rand_Number);
		InsertBits(0, 24, &Temp_Buff[0],Rand_Number);
	}
	else
	{
		InsertBits(0, 24, &Temp_Buff[0], Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
	}
#else
	InsertBits(0, 24, &Temp_Buff[0], Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
#endif
	//memcpy(&Temp_Buff[3],Buff,(Length-5));
	for (uc_index = 0; uc_index < (Length-5); uc_index++)
	{
		Temp_Buff[uc_index + 3] = Buff[uc_index];
	}
	Calc_CRC = CalculateDirectReverseCrc(Temp_Buff, uc_index + 3);
	Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length-5]);
	if(Calc_CRC != Recv_CRC)
	{
		Seq_Num_2_Detect_Roll_over = ExtractBits(0, 8, (unsigned char *)&Buff[5]);
		if( (Seq_Num_2_Detect_Roll_over ==0) || (Seq_Num_2_Detect_Roll_over == 1) )
		{
			printf("Tx seq Rollover Found for Peer:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
			memset(Temp_Buff,0,sizeof(Temp_Buff));
			InsertBits(0, 24, &Temp_Buff[0], (Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value+1));
			//memcpy(&Temp_Buff[3],Buff,(Length-5));
			for (uc_index = 0; uc_index < (Length-5); uc_index++)
			{
				Temp_Buff[uc_index + 3] = Buff[uc_index];
			}
			Calc_CRC = CalculateDirectReverseCrc(Temp_Buff, uc_index + 3);
			if(Calc_CRC == Recv_CRC)
			{
				printf("CRC OK AFTER ROLL OVER\n");
				printf("Prev Init Val:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
				++Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value;
				printf("After Init Val:0x%06lX\n",Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value);
				Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value;
			}
			else
			{
				printf("Status CRC fail Rollover:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				printf("R-CRC:0x%08lX\n",Recv_CRC);
				printf("C-CRC:0x%08lX\n",Calc_CRC);
				return;
			}
		}
		else
		{
			printf("Status  CRC fail:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
			printf("R-CRC:0x%08lX\n",Recv_CRC);
			printf("C-CRC:0x%08lX\n",Calc_CRC);
			return;
		}
	}
	else
	{
		//printf("Received CRC       :0x%08lX\n",Recv_CRC);
	}

	ROLLOVER_STATUS:

	BITSTUFFINC(8);

	Hitachi_Status_st[Source_Index].Des_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("Des_Addr           :%u\n", Hitachi_Status_st[Source_Index].Des_Addr);
	BITSTUFFINC(16);

	Hitachi_Status_st[Source_Index].Src_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("Src_Addr           :%u\n", Hitachi_Status_st[Source_Index].Src_Addr);
	BITSTUFFINC(16);

	Hitachi_prms_st[Source_Index].Rx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Tx_seq             :%u\n", Hitachi_prms_st[Source_Index].Rx_Seq);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Tx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Rx_Seq             :%u\n", Hitachi_prms_st[Source_Index].Tx_Seq);
	if(Hitachi_prms_st[Source_Index].Tx_Seq == 0)
	{
		printf("\nRX_Seq Roll over Detected Incremented self Vital Init Value:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
		printf("Prev:%06lx",Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
		++Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value;
		printf("Curr:%06lx",Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
		Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val = Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value;
	}
	BITSTUFFINC(8);

	Hitachi_Status_st[Source_Index].Msg_Type = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_type           :%u\n", Hitachi_Status_st[Source_Index].Msg_Type);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Msg_Status_Flag = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_Flag           :0x%04X\n", Hitachi_prms_st[Source_Index].Msg_Status_Flag);
	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		SET_BIT_USER(LINK_ESTABLISHED,Hitachi_prms_st[Source_Index].Msg_Status_Flag);
	}

	BITSTUFFINC(16);

	//Debug_Status_Flag(Hitachi_prms_st[Source_Index].Msg_Status_Flag,Hitachi_Status_st[Source_Index].Msg_Type);

	if ( SET == (CHECKFLAG(TIME_STAMP,Hitachi_prms_st[Source_Index].Msg_Status_Flag)) ) // Time-stamp Extraction
	{
		HITACHI_time_Stamp_st[Source_Index].YY = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("YEAR               :%02X\n", HITACHI_time_Stamp_st[Source_Index].YY);

		HITACHI_time_Stamp_st[Source_Index].MM = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("Month              :%02X\n", HITACHI_time_Stamp_st[Source_Index].MM);

		HITACHI_time_Stamp_st[Source_Index].DD = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("Date               :%02X\n", HITACHI_time_Stamp_st[Source_Index].DD);

		HITACHI_time_Stamp_st[Source_Index].hh = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("Hour               :%02X\n", HITACHI_time_Stamp_st[Source_Index].hh);

		HITACHI_time_Stamp_st[Source_Index].mm = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("Minutes            :%02X\n", HITACHI_time_Stamp_st[Source_Index].mm);

		HITACHI_time_Stamp_st[Source_Index].ss = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		BITSTUFFINC(8);
		//printf("Seconds            :%02X\n", HITACHI_time_Stamp_st[Source_Index].ss);

		(IS_MLK_EIPC_TIME_MATCHING(Source_Index))?(Change_MLK_TIME[Source_Index]=0):(Change_MLK_TIME[Source_Index]=1);

	}
	if ( ( SET == (CHECKFLAG(LINK_ESTABLISHED,Hitachi_prms_st[Source_Index].Msg_Status_Flag))) )
	{
		LinkEstablishFlag = SET;
	}


	/* Preparing and Sending Reply */
	Send_Status_Message(Source_Index,Des_Index);
	/* Preparing and Sending Reply */

	/* Updating the Clocks */
	Last_Recevied_Clock[Source_Index] = Ticks;
	Last_Recevied_Clock_ValidMsgs[Source_Index] = Ticks;
	//	Recv_Init_Msg_cnt[Source_Index] = 0;
	Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = HAL_Elaps_Ticks(Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff); // To detected EiPC shifting Faster
	/* Updating the Clocks */

	if((EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL) && (LastActCount[Source_Index] < MAX_LAST_ACT_COUNT) && (CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]) == RESET) && (Act_InactiveFlag[Source_Index] == SET))
	{
		LinkEstablishFlag = RESET;
	}
	else if (EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL)
	{
		printf("LAC[%lu]:%u\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index],LastActCount[Source_Index]);
		printf("Act_InactiveFlag:%u\n",Act_InactiveFlag[Source_Index]);
		printf("OnlineBit:%u\n",CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]));
	}


	/*	Checking whether the Link is Established or Not
	 *	If Link is Established and CRC's Differ then only Copy to Main Buffer & Send to other EIPC
	 */
	if( LinkEstablishFlag == SET )
	{
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		memcpy(&Main_Relays_st[Source_Index],&Relays_st[Source_Index],sizeof(Relays_st[Source_Index]));

		if(Prev_Relay_CRC[Source_Index] != Main_Relays_st[Source_Index].RelayIPS_CRC)
		{
			Prev_Relay_CRC[Source_Index] = Main_Relays_st[Source_Index].RelayIPS_CRC;
			memset(&Tx_Temp_Main_Relays_st,0,sizeof(Tx_Temp_Main_Relays_st));
			memcpy(&Tx_Temp_Main_Relays_st,&Main_Relays_st[Source_Index],sizeof(Tx_Temp_Main_Relays_st));
#ifdef EI_1
			TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
			TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif

			if(MisMatchFlag[Source_Index] == 0)
			{
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Source_Index * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Source_Index].RelayIPS_Buff,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
#ifdef EI_1
				ReArrangedVitalData();
				TransmitCanPkt(EI_PROCESS_DATA);
				MIE_Signal_Send_CLK = Ticks;

#endif
			}
		}
	}

	/* Sending Information to Other Card */
	Tx_Ei_self_sharing_st[Source_Index].VITAL_DATA_CRC = Main_Relays_st[Source_Index].RelayIPS_CRC;
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_CRC = Recv_CRC;
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_Msg_Type = STATUSMSG;

	Tx_Ei_self_sharing_st[Source_Index].Peer_Index = Source_Index;
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = RECEIVED;
	Tx_Ei_self_sharing_st[Source_Index].LinkEstablishFlag = LinkEstablishFlag;
	memset(&Temp_Tx_Ei_self_sharing_st,0,sizeof(EI_SELF_SHARING_t));
	memcpy(&Temp_Tx_Ei_self_sharing_st,&Tx_Ei_self_sharing_st[Source_Index],sizeof(EI_SELF_SHARING_t));

#ifdef EI_1
	TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
	TransmitCanPkt(EI_2_SELF_SHARING);
#endif
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = NOT_RECEIVED;
	Self_Sharing_CAN_Clk[Source_Index] = Ticks;
	/* Sending Information to Other Card */



	/* Checking the Link Status */
	if(LinkEstablishFlag == SET)
	{
		Link_Establish_Clock[Source_Index] = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishClk = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishFlag = SET;
		SelfLinkFlag[Source_Index] = SET;
	}
	else
	{
		SelfLinkFlag[Source_Index] = RESET;
		Hitachi_clk_flags_st[Source_Index].Link_FailClk = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishFlag = RESET;
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		Main_Relays_st[Source_Index].RelayIPS_CRC = 0;
	}
	/* Checking the Link Status */
	HAL_GPIO_TogglePin(S1_LED_GPIO_Port, S1_LED_Pin);

}

void Send_Status_Message(uint8_t Source_Index,uint8_t Des_Index)
{
	uint8_t Transmit_Buff[60] = {0,};
	uint8_t Temp_Buff[60] = {0,};
	uint8_t uc_index = 0;

	BITSTUFFINIT(0,0);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_SOM);
	BITSTUFFINC(8);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.DEST_ADDRESSES[Source_Index]);
	BITSTUFFINC(16);

	if( ( (CHECKFLAG(2,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_number = 0;
#ifdef STM_32
		Rand_number = HAL_RNG_GetRandomNumber(&hrng) % 200;
#endif
		printf("Injected TX Sequence value failure\n");
		printf("Actual TX Sequence value:%u\n",Hitachi_prms_st[Source_Index].Tx_Seq);
		printf("Injected TX Sequence Value:%u\n",Rand_number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte],Rand_number);
	}
	else
	{
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Tx_Seq); // TX Seq Number
		//Last Received Packet Rx_seq number is our TX_Seq Number
	}
	BITSTUFFINC(8);

	if( ( (CHECKFLAG(3,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_number = 0;
#ifdef STM_32
		Rand_number = (HAL_RNG_GetRandomNumber(&hrng) % 200);
#endif
		printf("Injected RX Sequence value failure\n");
		printf("Actual RX Sequence value:%u\n",Hitachi_prms_st[Source_Index].Rx_Seq);
		printf("Injected RX Vital Init Value:%u\n",Rand_number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_number); // Rx Seq Number
		Rx_Seq_failure = 1;
	}
	else
	{
		Rx_Seq_failure = 0;
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], ++Hitachi_prms_st[Source_Index].Rx_Seq); // Rx Seq Number
		//Last Received Packet Tx_seq number is our Rx_Seq Number incremented by 1.
	}
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], STATUSMSG);
	BITSTUFFINC(8);


	if(SET == CHECKFLAG(3,Hitachi_prms_st[Source_Index].Msg_Status_Flag))
	{
		if(Change_MLK_TIME[Source_Index]== 1)
		{
			InsertBits(ucBit, 16, &Transmit_Buff[ucByte],0x0068);
		}
		else
		{
			InsertBits(ucBit, 16, &Transmit_Buff[ucByte],0x0028);
		}
		BITSTUFFINC(16);
	}
	else
	{
		if(Change_MLK_TIME[Source_Index]== 1)
		{
			InsertBits(ucBit, 16, &Transmit_Buff[ucByte],0x0064);
		}
		else
		{
			InsertBits(ucBit, 16, &Transmit_Buff[ucByte],0x0024);
		}
		BITSTUFFINC(16);
	}

	if (((Hitachi_prms_st[Source_Index].Msg_Status_Flag >> 5) & 1)) // Time-stamp Extraction
	{
		if(Change_MLK_TIME[Source_Index]== 1)
		{
			Get_time();
			memcpy(&HITACHI_time_Stamp_st[Source_Index],&RTC_Date_time_BCD_st,sizeof(GPS_DATE_TIME_t));
			printf("Sending Updated Time\n");
			Change_MLK_TIME[Source_Index] = 0;
		}
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].YY);
		BITSTUFFINC(8);

		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].MM);
		BITSTUFFINC(8);

		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].DD);
		BITSTUFFINC(8);

		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].hh);
		BITSTUFFINC(8);

		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].mm);
		BITSTUFFINC(8);

		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].ss);
		BITSTUFFINC(8);

	}

	if( ( (CHECKFLAG(1,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint32_t Rand_number = 0;
#ifdef STM_32
		Rand_number = HAL_RNG_GetRandomNumber(&hrng) % 0xffff00;
#endif
		printf("\nInjected Vital Init Value Failure\n");
		printf("Actual Vital Init Value:0x%06lX\n",Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
		printf("Injected Vital Init Value:0x%06lX\n",Rand_number);
		InsertBits(0, 24, &Temp_Buff[0], Rand_number);
	}
	else
	{
		InsertBits(0, 24, &Temp_Buff[0], Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value);
	}
	for (uc_index = 0; uc_index < ucByte; uc_index++)
	{
		Temp_Buff[uc_index + 3] = Transmit_Buff[uc_index];
	}

	InsertBits(ucBit, 32, &Transmit_Buff[ucByte], CalculateDirectReverseCrc(Temp_Buff, uc_index + 3));
	Rx_Seq_failure = 0;
	BITSTUFFINC(32);

	if( ( CHECKFLAG(9,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)== SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		printf("Injected ETH tx Fail\n");
		printf("Not Sending Reply");
	}
	else
	{
		Process_Sending_Reply(Transmit_Buff, ucByte,Source_Index);
	}
}

void Service_Msg_Extr(unsigned char *Buff, unsigned char Length,uint8_t Source_Index,uint8_t Des_Index)
{
	uint32_t Calc_CRC = 0;
	uint32_t Recv_CRC = 0;
	uint8_t LinkEstablishFlag = RESET;
	uint32_t Ticks = HAL_GetTick();
	BITSTUFFINIT(0, 0);

	Calc_CRC = CalculateDirectReverseCrc(Buff, Length - 5);
	Recv_CRC = ExtractBits(ucBit, 32, (unsigned char *)&Buff[Length - 5]);

	if(Recv_CRC != Calc_CRC)
	{
		printf("Service Message CRC Fail:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
		printf("R-CRC:0x%08lX\n",Recv_CRC);
		printf("C-CRC:0x%08lX\n",Calc_CRC);
		return;
	}

	Last_Recevied_Clock[Source_Index] = Ticks;
	Last_Recevied_Clock_ValidMsgs[Source_Index] = Ticks;
	//Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = HAL_Elaps_Ticks(Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff); // To detected EiPC shifting Faster
	BITSTUFFINC(8);

	Hitachi_Service_st[Source_Index].Des_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//	printf("Des_Addr:%u\n", Hitachi_Service_st[Source_Index].Des_Addr);
	BITSTUFFINC(16);

	Hitachi_Service_st[Source_Index].Src_Addr = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);
	//	printf("Src_Addr:%u\n", Hitachi_Service_st[Source_Index].Src_Addr);
	BITSTUFFINC(16);

	Hitachi_prms_st[Source_Index].Rx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Tx_seq             :%u\n",  Hitachi_prms_st[Source_Index].Rx_Seq);
	BITSTUFFINC(8);

	Hitachi_prms_st[Source_Index].Tx_Seq = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Rx_Seq             :%u\n",Hitachi_prms_st[Source_Index].Tx_Seq);
	BITSTUFFINC(8);


	Hitachi_Service_st[Source_Index].Msg_Type = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
	//printf("Msg_type           :%u\n", Hitachi_Service_st[Source_Index].Msg_Type);
	BITSTUFFINC(8);


	Hitachi_prms_st[Source_Index].Msg_Status_Flag = ExtractBits(ucBit, 16, (unsigned char *)&Buff[ucByte]);

	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		SET_BIT_USER(LINK_ESTABLISHED,Hitachi_prms_st[Source_Index].Msg_Status_Flag);
	}

	//printf("Msg_Flag           :0x%04X\n", Hitachi_prms_st[Source_Index].Msg_Status_Flag);
	BITSTUFFINC(16);

	//Debug_Status_Flag(Hitachi_prms_st[Source_Index].Msg_Status_Flag,Hitachi_Service_st[Source_Index].Msg_Type);

	if ( SET == CHECKFLAG(TIME_STAMP,Hitachi_prms_st[Source_Index].Msg_Status_Flag) ) // Time-stamp Extraction
	{
		HITACHI_time_Stamp_st[Source_Index].YY = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("YEAR               :%02X\n", HITACHI_time_Stamp_st[Source_Index].YY);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].MM = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Month              :%02X\n", HITACHI_time_Stamp_st[Source_Index].MM);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].DD = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Date               :%02X\n", HITACHI_time_Stamp_st[Source_Index].DD);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].hh = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Hour               :%02X\n", HITACHI_time_Stamp_st[Source_Index].hh);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].mm = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Minutes            :%02X\n", HITACHI_time_Stamp_st[Source_Index].mm);
		BITSTUFFINC(8);

		HITACHI_time_Stamp_st[Source_Index].ss = ExtractBits(ucBit, 8, (unsigned char *)&Buff[ucByte]);
		//printf("Seconds            :%02X\n", HITACHI_time_Stamp_st[Source_Index].ss);
		BITSTUFFINC(8);


		(IS_MLK_EIPC_TIME_MATCHING(Source_Index))?(Change_MLK_TIME[Source_Index]=0):(Change_MLK_TIME[Source_Index]=1);

	}
	if ( ( SET == (CHECKFLAG(LINK_ESTABLISHED,Hitachi_prms_st[Source_Index].Msg_Status_Flag))) )
	{
		LinkEstablishFlag = SET;
	}

	/* Preparing and Sending Reply */
	Send_Status_Message(Source_Index, Des_Index);
	/* Preparing and Sending Reply */

	if((EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL) && (LastActCount[Source_Index] < MAX_LAST_ACT_COUNT) && (CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]) == RESET) && (Act_InactiveFlag[Source_Index] == SET))
	{
		LinkEstablishFlag = RESET;
	}
	else if (EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL)
	{
		printf("LAC[%lu]:%u\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index],LastActCount[Source_Index]);
		printf("Act_InactiveFlag:%u\n",Act_InactiveFlag[Source_Index]);
		printf("OnlineBit:%u\n",CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]));
	}

	/*	if((EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL) && (CHECKFLAG(0,Hitachi_Vital_st[Source_Index].Outputs[0]) == SET) && (LinkEstablishFlag == SET))
	{
		if(Act_InactiveFlag[Source_Index] == RESET)
		{

			++LastActCount[Source_Index];
			printf("Updating Count:%u\n",LastActCount[Source_Index]);
			if(LastActCount[Source_Index] >= 10)
			{
				Act_InactiveFlag[Source_Index] = SET;
			}
			else
			{
				LinkEstablishFlag = RESET;
				memset(&Hitachi_Vital_st[Source_Index].Outputs,0,sizeof(Hitachi_Vital_st[Source_Index].Outputs));
			}
		}
	}*/


	/*	Checking whether the Link is Established or Not
	 *	If Link is Established and CRC's Differ then only Copy to Main Buffer & Send to other EIPC
	 */
	if (LinkEstablishFlag == SET )
	{
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		memcpy(&Main_Relays_st[Source_Index],&Relays_st[Source_Index],sizeof(Relays_st[Source_Index]));

		if(Prev_Relay_CRC[Source_Index] != Main_Relays_st[Source_Index].RelayIPS_CRC)
		{
			Prev_Relay_CRC[Source_Index] = Main_Relays_st[Source_Index].RelayIPS_CRC;
			memset(&Tx_Temp_Main_Relays_st,0,sizeof(Tx_Temp_Main_Relays_st));
			memcpy(&Tx_Temp_Main_Relays_st,&Main_Relays_st[Source_Index],sizeof(Tx_Temp_Main_Relays_st));

#ifdef EI_1
			TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
			TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif

			Tx_Ei_self_sharing_st[Source_Index].VITAL_DATA_CRC = Main_Relays_st[Source_Index].RelayIPS_CRC;

			if(MisMatchFlag[Source_Index] == 0)
			{
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Source_Index * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Source_Index].RelayIPS_Buff,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
#ifdef EI_1
				ReArrangedVitalData();
				TransmitCanPkt(EI_PROCESS_DATA);
				MIE_Signal_Send_CLK = Ticks;
#endif
			}
		}
	}
	/*	Checking whether the Link is Established or Not
	 *	If Link is Established and CRC's Differ then only Copy to Main Buffer & Send to other EIPC
	 */


	/* Sending Information to Other card */
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_CRC = Recv_CRC;
	Tx_Ei_self_sharing_st[Source_Index].Last_Recv_Msg_Type = SERVICEMSG;

	Tx_Ei_self_sharing_st[Source_Index].Peer_Index = Source_Index;
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = RECEIVED;
	Tx_Ei_self_sharing_st[Source_Index].LinkEstablishFlag = LinkEstablishFlag;
	memset(&Temp_Tx_Ei_self_sharing_st,0,sizeof(EI_SELF_SHARING_t));
	memcpy(&Temp_Tx_Ei_self_sharing_st,&Tx_Ei_self_sharing_st[Source_Index],sizeof(EI_SELF_SHARING_t));
#ifdef EI_1
	TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
	TransmitCanPkt(EI_2_SELF_SHARING);
#endif
	Tx_Ei_self_sharing_st[Source_Index].Msg_Rcv_Flag = NOT_RECEIVED;
	Self_Sharing_CAN_Clk[Source_Index] = Ticks;
	/* Sending Information to Other card */


	/*	Checking Link Status */
	if( LinkEstablishFlag == SET)
	{
		Link_Establish_Clock[Source_Index] = Ticks;
		SelfLinkFlag[Source_Index] = SET;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishFlag = SET;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishClk = Ticks;
	}
	else
	{
		SelfLinkFlag[Source_Index] = RESET;
		Hitachi_clk_flags_st[Source_Index].Link_FailClk = Ticks;
		Hitachi_clk_flags_st[Source_Index].Link_EstablishFlag = RESET;
		memset(&Main_Relays_st[Source_Index].RelayIPS_Buff,0,sizeof(Main_Relays_st[Source_Index].RelayIPS_Buff));
		Main_Relays_st[Source_Index].RelayIPS_CRC = 0;
	}
	/*	Checking Link Status */

	HAL_GPIO_TogglePin(S1_LED_GPIO_Port, S1_LED_Pin);

}

void Send_Service_Signal_Message(uint8_t Source_Index,uint8_t Des_Index)
{
	uint8_t Transmit_Buff[60] = {0,};

	BITSTUFFINIT(0, 0);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_SOM); //Destination Address
	BITSTUFFINC(8);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]); //Destination Address
	BITSTUFFINC(16);

	InsertBits(ucBit, 16, &Transmit_Buff[ucByte], EI_data.Hitachi_st.DEST_ADDRESSES[Source_Index]); //Source Address
	BITSTUFFINC(16);

	if( ( (CHECKFLAG(2,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_number = 0;
#ifdef STM_32
		Rand_number = (HAL_RNG_GetRandomNumber(&hrng) % 200);
#endif
		printf("Injected TX Sequence value failure\n");
		printf("Actual TX Sequence value:%u\n",Hitachi_prms_st[Source_Index].Tx_Seq);
		printf("Injected TX Sequence Value:%u\n",Rand_number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_number); // TX Seq Number
	}
	else
	{
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Hitachi_prms_st[Source_Index].Tx_Seq); // TX Seq Number
		//Last Received Packet Rx_seq number is our TX_Seq Number
	}
	//Last Received Packet Rx_seq number is our TX_Seq Number
	BITSTUFFINC(8);

	if( ( (CHECKFLAG(3,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)) == SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		uint8_t Rand_number = 0;
#ifdef STM_32
		Rand_number = (HAL_RNG_GetRandomNumber(&hrng) % 200);
#endif
		printf("Injected RX Sequence value failure\n");
		printf("Actual RX Sequence value:%u\n",Hitachi_prms_st[Source_Index].Rx_Seq);
		printf("Injected RX Vital Init Value:%u\n",Rand_number);
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], Rand_number); // Rx Seq Number
	}
	else
	{
		InsertBits(ucBit, 8, &Transmit_Buff[ucByte], ++Hitachi_prms_st[Source_Index].Rx_Seq); // Rx Seq Number
		//Last Received Packet Tx_seq number is our Rx_Seq Number incremented by 1.
	}
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], SERVICEMSG);
	BITSTUFFINC(8);



	if(Change_MLK_TIME[Source_Index]== 1)
	{
		Get_time();
		memcpy(&HITACHI_time_Stamp_st[Source_Index],&RTC_Date_time_BCD_st,sizeof(GPS_DATE_TIME_t));
		printf("Sending Updated Time\n");
		Change_MLK_TIME[Source_Index] = 0;
		InsertBits(ucBit, 16, &Transmit_Buff[ucByte], 0x0069);
		BITSTUFFINC(16);
	}
	else
	{
		InsertBits(ucBit, 16, &Transmit_Buff[ucByte], 0x0029);
		BITSTUFFINC(16);
	}

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].YY);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].MM);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].DD);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].hh);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].mm);
	BITSTUFFINC(8);

	InsertBits(ucBit, 8, &Transmit_Buff[ucByte], HITACHI_time_Stamp_st[Source_Index].ss);
	BITSTUFFINC(8);

	InsertBits(ucBit, 32, &Transmit_Buff[ucByte], CalculateDirectReverseCrc(Transmit_Buff, ucByte));
	BITSTUFFINC(32);

	if( ( CHECKFLAG(9,Hitachi_fault_injection_st.FAULT_INJECTION_BITS)== SET) && (Hitachi_fault_injection_st.FOR_WHICH_PEER_INDEX == Source_Index) )
	{
		printf("Injected ETH tx Fail\n");
		printf("Not Sending Reply");
	}
	else
	{
		Process_Sending_Reply(Transmit_Buff,ucByte,Source_Index);
	}
}

void Process_Sending_Reply(uint8_t *Transmit_Buff, uint8_t Length,uint8_t Source_Index)
{
	uint8_t Temp_Buff[100] = {0,};
	uint8_t uc_index = 0;
	uint8_t Temp_Index = 0;

	//
	//	for(uint8_t Traverser = 0;Traverser <EI_data.Hitachi_st.PeersCnt;++Traverser)
	//	{
	//		printf("Lists[%u]:%s\n",Traverser,ipaddr_ntoa(&Recv_IP_List[Traverser]));
	//	}



	//memset(Temp_Buff,0,sizeof(Temp_Buff));
	//Print_EI_DATA(Transmit_Buff,XMT,Length);

	//	printf("Reply Packet:\n");
	Temp_Buff[Temp_Index++] = Transmit_Buff[0];
	for (uc_index = 1; uc_index < Length; ++uc_index)
	{
		if ((Transmit_Buff[uc_index] >= 0xF0) && (Transmit_Buff[uc_index] <= 0xFF))
		{
			Temp_Buff[Temp_Index++] = Transmit_Buff[uc_index] & 0xF0;
			Temp_Buff[Temp_Index++] = Transmit_Buff[uc_index] & 0x0F;
		}
		else
		{
			Temp_Buff[Temp_Index++] = Transmit_Buff[uc_index];
		}
	}
	Temp_Buff[Temp_Index++] = 0xf6;

	//	for (uc_index = 0; uc_index < Temp_Index; ++uc_index)
	//	{
	//		printf("0x%02X|", Temp_Buff[uc_index]);
	//	}
	//	printf("\n");

	if( (SEND_FORCE_REPLY[Source_Index] == SET) && (Rx_Print_st.Size == 0) && (Debug_flag == 1) )
	{
		Display("\n________________________________________MSG DETAILS________________________________________\n");
	}

	//	if( (EI_data.Hitachi_st.Interface_Type[Source_Index] == ETHERNET) && ((Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY == RECEIVED) || (SEND_FORCE_REPLY[Source_Index] == SET)))
	//if( (EI_data.Hitachi_st.Interface_Type[Source_Index] == ETHERNET) || ((Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY == RECEIVED) || (SEND_FORCE_REPLY[Source_Index] == SET)))
	if(EI_data.Hitachi_st.Interface_Type[Source_Index] == ETHERNET)
	{
		//printf("Sending Reply:%s\n",ipaddr_ntoa(&Recv_IP_List[0]));
		//printf("\n");
		//		char *SendingIP = ipaddr_ntoa(&Recv_IP_List[Source_Index]);
		//		printf("\nSent IP[%lu]:%s\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index], SendingIP);

		udpClient_send(Temp_Buff, Temp_Index,Recv_IP_List[Source_Index]);
		//udpClient_send(Temp_Buff, Temp_Index,Recv_IP_List[0]); //Only for IRISET
		SEND_FORCE_REPLY[Source_Index] = RESET;
		//udpClient_send(Temp_Buff, Temp_Index,Source_IP);
	}
	//	else
	//	{
	//		printf("\nNot sending:%u\n",Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY);
	//	}

	if(	EI_data.Hitachi_st.Interface_Type[Source_Index] == SERIAL)
	{
		for(uint16_t i=0;i<Temp_Index;++i)
		{
			Com_SendByte(THREE_PORT, Temp_Buff[i]);
			//printf("%02X ",Temp_Buff[i]);
		}
	}


	if(Debug_flag == 1)
	{
		Display("\n________________________________________MSG DETAILS________________________________________\n");
	}
	//	else
	//	{
	//		Display("\n");
	//	}

	if(Rx_Print_st.Size)//To Print Received Data
	{
		Print_EI_DATA(Rx_Print_st.Print_Buff, RCV, Rx_Print_st.Size);
		Rx_Print_st.Size = 0;
	}

	Print_EI_DATA(Transmit_Buff,XMT,Length); //To Print Transmitted data

	if(Debug_flag == 1)
	{
		Display("___________________________________________________________________________________________\n");
	}
	//	else
	//	{
	//		Display("\n");
	//	}
}
void Debug_Status_Flag(uint16_t Msg_sts_flag,uint8_t Message_Type)
{
	uint8_t i=0;

	switch(Message_Type)
	{
	case INIT_MSG:
		printf("FOR INIT MESSAGE   :");
		break;
	case VDATAMSG:
		printf("FOR VITAL MSG MSG  :");
		break;
	case STATUSMSG:
		printf("FOR STATUS MSG     :");
		break;
	case SERVICEMSG:
		printf("FOR SERVICE MSG    :");
		break;
	default:
		printf("Unknown Message Type:%u\n",Message_Type);
		return;
	}

	for(i=0; i<15; ++i)
	{
		if( (CHECKFLAG(i,Msg_sts_flag)) == SET )
		{
			switch(i)
			{
			case ACK_MESSAGE:
				printf("ACK MESSAGE FLAG SET|");
				break;

			case NACK_MESSAGE:
				printf("NACK MESSAGE FLAG SET|");
				break;

			case ACK_REQUIRED:
				printf("ACK REQUIRED FLAG SET|");
				break;

			case LINK_ESTABLISHED:
				printf("LINK_ESTABLISHED FLAG SET|");
				break;

			case BROADCAST:
				printf("BROADCAST FLAG SET|");
				break;

			case TIME_STAMP:
				printf("TIME_STAMP FLAG SET|");
				break;

			case TIME_STAMP_RESET:
				printf("TIME_STAMP_RESET FLAG SET|");
				break;

			case INIT_REQUEST:
				printf("INIT_REQUEST FLAG SET|");
				break;

			default:
				printf("UNKNOWN FLAG SET:\"%u\"|\n",i);
			}
		}
	}
	printf("\n");
}


void Check_Primary_r_Secondary(uint8_t Source_Index)
{
	uint32_t CurTicks = HAL_GetTick();
	if(EI_data.Hitachi_st.Interface_Type[Source_Index] ==  SERIAL)
	{
		return;
	}
	else
	{
		//_______________________ DECIDING PRIMARY OR SECONDARY _________________________________________
		if( (Tx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == NOT_RECEIVED) && (Rx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == NOT_RECEIVED) )
		{
			if(Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != SECONDARY)
			{
				printf("Both Cards not Receiving data, Shifting to Secondary:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY = SECONDARY;
			}
		}
		else if( (Tx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == NOT_RECEIVED) && (Rx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == RECEIVED) )
		{
			if(Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != SECONDARY)
			{
				printf("This Card not Receiving data, Shifting to Secondary:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY = SECONDARY;
			}
		}
		else if( (Tx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == RECEIVED) && (Rx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == NOT_RECEIVED) )
		{
			if(Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != PRIMARY)
			{
				printf("This Card Only Receiving data, Shifting to Primary:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				Last_Recevied_Clock[Source_Index] = CurTicks;
				Last_Recevied_Clock_ValidMsgs[Source_Index] = CurTicks;
				//				Link_Establish_Clock[Source_Index] = CurTicks;

				Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = 0;
				Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY = PRIMARY;
			}
		}
		else if( (Tx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == RECEIVED) && (Rx_Ei_self_sharing_st[Source_Index].EI_Receiving_Status == RECEIVED) )
		{
#ifdef EI_1
			if(Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != PRIMARY)
			{
				printf("Both Cards Receiving data,EiPC_1 Shifting to Primary:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				Last_Recevied_Clock[Source_Index] = CurTicks;
				Last_Recevied_Clock_ValidMsgs[Source_Index] = CurTicks;
				Tx_Ei_self_sharing_st[Source_Index].Last_RCV_Clk_diff = 0;
				Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY = PRIMARY;
			}
#endif

#ifdef EI_2
			if(Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != SECONDARY)
			{
				printf("Both Cards Receiving data,EiPC_2 Shifting to Secondary:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
				Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY = SECONDARY;
			}
#endif

		}
	}
}

void Check_Vital_CRC(uint8_t Source_Index)
{
	static uint8_t Diff_Cnt_Rx[MAX_PEER_CONNECTIONS] = {0,};
	static uint8_t Diff_Cnt_Tx[MAX_PEER_CONNECTIONS] = {0,};


	if(EI_data.Hitachi_st.Interface_Type[Source_Index] ==  SERIAL)
	{
		return;
	}
	else
	{
		if(Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val != Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val )
		{
			if(Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val != 0)
			{
				if( (Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != PRIMARY) || (INIT_Received[Source_Index] == NOT_RECEIVED) )
				{
					uint32_t Diff = abs(Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val - Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);

					if( (Diff != 1 ) || (Diff_Cnt_Rx[Source_Index] == 3) )
					{
						Diff_Cnt_Rx[Source_Index] = 0;
						printf("RX_Vital Init Value is not same in Self and Other EiPC Cards:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
						printf("RX_Vital Init Value S-EiPC:0x%06lX\n",Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);
						printf("RX_Vital Init Value O-EiPC:0x%06lX\n",Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val);
						printf("\nCopied RX_Vital Init Value from O-EiPC\n");
						Hitachi_prms_st[Source_Index].Rx_Vital_CRC_Init_Value = Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val;
						Tx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val = Rx_Ei_self_sharing_st[Source_Index].Rx_Vital_Init_Val;
					}
					else if(Diff == 1)
					{
						Diff_Cnt_Rx[Source_Index]++;
					}
				}
			}
		}
	}

	if(Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val != Rx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val )
	{
		if(Rx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val != 0)
		{
			if( (Tx_Ei_self_sharing_st[Source_Index].IS_PRIMARY != PRIMARY) || (INIT_Received[Source_Index] == NOT_RECEIVED) )
			{
				uint32_t Diff = abs(Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val - Rx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val);
				if( (Diff != 1 ) || (Diff_Cnt_Tx[Source_Index] == 3) )
				{
					Diff_Cnt_Tx[Source_Index] = 0;
					printf("TX_Vital Init Value is not same in Self and Other EiPC Cards:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
					printf("TX_Vital Init Value S-EiPC:0x%06lX\n",Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val);
					printf("TX_Vital Init Value O-EiPC:0x%06lX\n",Rx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val);
					printf("\nCopied TX Vital Init Value from O-EiPC\n");
					Hitachi_prms_st[Source_Index].Tx_Vital_CRC_Init_Value = Rx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val;
					Tx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val = Rx_Ei_self_sharing_st[Source_Index].Tx_Vital_Init_Val;
				}
				else if(Diff == 1)
				{
					Diff_Cnt_Tx[Source_Index]++;
				}
			}
		}
	}
}

void Check_Other_EiPC_Packet(uint8_t Source_Index)
{
	if(EI_data.Hitachi_st.Interface_Type[Source_Index] !=  SERIAL)
	{
		Check_Primary_r_Secondary(Source_Index);
		Check_Vital_CRC(Source_Index);
	}
	CheckOtherLinkStatus(Source_Index);
}

void CheckLinkEstablishTimeout(void)
{
	uint8_t Traverser = 0;
	static uint32_t Prev_Last_LinkEstablish_CLK[MAX_PEER_CONNECTIONS] = {0,};

	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		return;
	}

	for(Traverser=0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		if( (HAL_Elaps_Ticks(Link_Establish_Clock[Traverser]) > EI_data.Hitachi_st.LinkEstablishTimeout[Traverser]) && (Prev_Last_LinkEstablish_CLK[Traverser] != Link_Establish_Clock[Traverser]) )
		{
			Prev_Last_LinkEstablish_CLK[Traverser] = Link_Establish_Clock[Traverser];
			printf("Link Establish Timeout for:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
			SelfLinkFlag[Traverser] = RESET;
			memset(&Main_Relays_st[Traverser].RelayIPS_Buff,0,sizeof(Main_Relays_st[Traverser].RelayIPS_Buff));
			Main_Relays_st[Traverser].RelayIPS_CRC = 0;
			Tx_Ei_self_sharing_st[Traverser].Last_RCV_Clk_diff = HAL_Elaps_Ticks(Tx_Ei_self_sharing_st[Traverser].Last_RCV_Clk_diff); // To detected EiPC shifting Faster
			CheckOtherLinkStatus(Traverser);
		}
	}
}


void Send_Self_Sharing(void)
{
	for(uint8_t Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		if(HAL_Elaps_Ticks(Self_Sharing_CAN_Clk[Traverser]) > (EI_data.Hitachi_st.CAN_Xchange_Time) )
		{
			Tx_Ei_self_sharing_st[Traverser].Msg_Rcv_Flag = NOT_RECEIVED;
			Tx_Ei_self_sharing_st[Traverser].LinkEstablishFlag = Hitachi_clk_flags_st[Traverser].Link_EstablishFlag;

			memset(&Temp_Tx_Ei_self_sharing_st,0,sizeof(EI_SELF_SHARING_t));
			Tx_Ei_self_sharing_st[Traverser].Peer_Index = Traverser;
			memcpy(&Temp_Tx_Ei_self_sharing_st,&Tx_Ei_self_sharing_st[Traverser],sizeof(EI_SELF_SHARING_t));

#ifdef EI_1
			TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
			TransmitCanPkt(EI_2_SELF_SHARING);
#endif
			Self_Sharing_CAN_Clk[Traverser] = HAL_GetTick();
			return; /* Returning from the function because to avoid the Data Mismatch at Receiption end*/
		}
	}
}

void Detect_CAN_Xchange_Failure(void)
{
	static uint32_t Previous_Clock_RCV_SELF_XCHG_CLK = 0;

	if(Is_CAN_Exchange_happening == YES)
	{
		Blink_S2();
	}

	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		Is_CAN_Exchange_happening = YES;
		return;
	}
	if(HAL_Elaps_Ticks(RCV_SELF_XCHG_CLK)>= EI_data.Hitachi_st.CAN_EXCHANGE_TIME_OUT)
	{
		if(Previous_Clock_RCV_SELF_XCHG_CLK != RCV_SELF_XCHG_CLK)
		{
			Is_CAN_Exchange_happening = NO;
			Previous_Clock_RCV_SELF_XCHG_CLK = RCV_SELF_XCHG_CLK;
			printf("CAN Data Not Received from last %lu Seconds\n",(HAL_Elaps_Ticks(RCV_SELF_XCHG_CLK)/1000));
			printf("Mem setting Rx_Ei_self_sharing_st\n");
			memset(&Rx_Ei_self_sharing_st,0,(sizeof(EI_SELF_SHARING_t)* EI_data.Hitachi_st.PeersCnt));
			memset(&vEI_Transver_EI_To_MIE,0,sizeof(EI_VITAL_DATA_TO_VITAL_COMPUTER_t));
			ReArrangedVitalData();
			TransmitCanPkt(EI_PROCESS_DATA);
		}
	}
}

void Process_Send_Signals_2_MIE(void)
{

#ifdef EI_1
	static uint32_t send_signals_serial_Clk = 0;
	static uint8_t Prev_Buff[MAX_OUTPUTS_PER_PEER * MAX_PEER_CONNECTIONS] = {0,};
	uint8_t Serial_Send_Buffer[(MAX_OUTPUTS_PER_PEER * MAX_PEER_CONNECTIONS) + 10] = {0,};
	uint16_t Serial_Byte = RESET;
#endif


	if(HAL_Elaps_Ticks(MIE_Signal_Send_CLK) > (EI_data.Hitachi_st.MIE_SIGNAL_SEND_CLK))
	{
		uint8_t Traverser = RESET;
		uint8_t Choice = RESET;
#ifdef EI_1
		for(Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
		{
			if( (MisMatchFlag[Traverser] == SET) && ( strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0))
			{
				printf("MisMatch Found:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
				memset(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],0,MAX_OUTPUTS_PER_PEER);
				continue;
			}
			Choice = RESET;
			(OtherCardLink_Flag[Traverser] == SET)?SET_BIT_USER(0,Choice):CLEAR_BIT_USER(0,Choice);
			(SelfLinkFlag[Traverser] == SET)?SET_BIT_USER(1,Choice):CLEAR_BIT_USER(1,Choice);

			if(Debug_flag == 1)
			{
				printf("Choice:%02X|%lu\n",Choice,EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
			}

			switch(Choice)
			{
			case BOTHCARD_LINK_NOK:
				memset(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],0,MAX_OUTPUTS_PER_PEER);
				break;

			case OTHERCARD_LINK_OK:
				/*printf("\nOtherCard SET\n");*/
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],OtherCard_Relays_st[Traverser].RelayIPS_Buff,MAX_OUTPUTS_PER_PEER);
				break;
			case THISCARD_LINK_OK:
				/*printf("\nOnly This Card SET\n");*/
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Traverser].RelayIPS_Buff,MAX_OUTPUTS_PER_PEER);
				break;
			case BOTHCARD_LINK_OK:
				/*printf("\nBothCard SET3\n");*/
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Traverser].RelayIPS_Buff,MAX_OUTPUTS_PER_PEER);
				break;

			default:
				memset(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],0,MAX_OUTPUTS_PER_PEER);
			}
		}


		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
		{
			Is_CAN_Exchange_happening = YES;
		}

		if(Is_CAN_Exchange_happening == NO)
		{
			printf("CAN Packet Not Receiving from EIPC1\n");
			printf("Sending Blank Relay statuses\n");
			memset(&vEI_Transver_EI_To_MIE,0,sizeof(vEI_Transver_EI_To_MIE));
		}


		if(Debug_flag == 1)
		{
			Get_time();
			printf("Data Sending 2 MIE T:%02u:%02u:%02u\n",RTC_Date_time_Dec_st.Hour,RTC_Date_time_Dec_st.Mins,RTC_Date_time_Dec_st.Secs);
			ReArrangedVitalData();
			for(uint16_t Byte = 0; Byte<(EI_data.Hitachi_st.PeersCnt * MAX_OUTPUTS_PER_PEER);++Byte )
			{
				if(ReorderFlag == RESET)
				{
					printf("%02X ",vEI_Transver_EI_To_MIE.Inputs[Byte]);
				}
				else
				{
					printf("%02X ",HitachiEI_DataReArranged_st.Inputs[Byte]);
				}
				if(((Byte+1)%16)==0)
				{
					printf("\n");
				}
			}
			printf("\n");
		}

#if ENABLE_SIGNAL_PRINT
		PrintSignalsHitachi();
#endif
		ReArrangedVitalData();
		TransmitCanPkt(EI_PROCESS_DATA);


		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
		{
			if(HAL_Elaps_Ticks(send_signals_serial_Clk) >= TWO_SEC )
			{
				memcpy(Prev_Buff,&vEI_Transver_EI_To_MIE.Inputs[0],sizeof(Prev_Buff));
				Serial_Byte = 0;
				Serial_Send_Buffer[Serial_Byte++] = 0x7E;
				Serial_Send_Buffer[Serial_Byte++] = 0x7E;
				Serial_Send_Buffer[Serial_Byte++] = 0x7E;
				if(Debug_flag == 1)
				{
					printf("Sending Signals over Ethernet:\n");
				}
				for(uint16_t Byte = 0;Byte<(vEI_Transver_EI_To_MIE.ActualDataBytes);++Byte)
				{
					if(ReorderFlag == RESET)
					{
						Serial_Send_Buffer[Serial_Byte++] = vEI_Transver_EI_To_MIE.Inputs[Byte];
					}
					else
					{
						Serial_Send_Buffer[Serial_Byte++] = HitachiEI_DataReArranged_st.Inputs[Byte];
					}

				}
				Serial_Send_Buffer[Serial_Byte++] = 0x7F;
				Serial_Send_Buffer[Serial_Byte++] = 0x7F;
				Serial_Send_Buffer[Serial_Byte++] = 0x7F;

				Udp_Send_SigInfo(Serial_Send_Buffer, Serial_Byte, Recv_IP_List[0]);
				send_signals_serial_Clk = HAL_GetTick();
			}
			else if(memcmp(Prev_Buff,&vEI_Transver_EI_To_MIE.Inputs[0],sizeof(Prev_Buff)) != 0)
			{
				memcpy(Prev_Buff,&vEI_Transver_EI_To_MIE.Inputs[0],sizeof(Prev_Buff));
				Serial_Byte = 0;
				Serial_Send_Buffer[Serial_Byte++] = 0x7E;
				Serial_Send_Buffer[Serial_Byte++] = 0x7E;
				Serial_Send_Buffer[Serial_Byte++] = 0x7E;
				if(Debug_flag == 1)
				{
					printf("Sending Signals over Ethernet:\n");
				}
				for(uint16_t Byte = 0;Byte<(vEI_Transver_EI_To_MIE.ActualDataBytes);++Byte)
				{
					if(ReorderFlag == RESET)
					{
						Serial_Send_Buffer[Serial_Byte++] = vEI_Transver_EI_To_MIE.Inputs[Byte];
					}
					else
					{
						Serial_Send_Buffer[Serial_Byte++] = HitachiEI_DataReArranged_st.Inputs[Byte];
					}
				}
				Serial_Send_Buffer[Serial_Byte++] = 0x7F;
				Serial_Send_Buffer[Serial_Byte++] = 0x7F;
				Serial_Send_Buffer[Serial_Byte++] = 0x7F;
				Udp_Send_SigInfo(Serial_Send_Buffer, Serial_Byte, Recv_IP_List[0]);
			}

		}


#endif
#ifdef EI_2

		for(Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
		{
			if( (MisMatchFlag[Traverser] == SET) && ( strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0))
			{
				printf("MisMatch Found:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
				memset(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],0,MAX_OUTPUTS_PER_PEER);
				continue;
			}
			Choice = RESET;
			(OtherCardLink_Flag[Traverser] == SET)?SET_BIT_USER(0,Choice):CLEAR_BIT_USER(0,Choice);
			(SelfLinkFlag[Traverser] == SET)?SET_BIT_USER(1,Choice):CLEAR_BIT_USER(1,Choice);

			printf("Choice:%02X|%lu\n",Choice,EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);

			switch(Choice)
			{
			case BOTHCARD_LINK_NOK:
				memset(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],0,MAX_OUTPUTS_PER_PEER);
				break;

			case OTHERCARD_LINK_OK:
				/*printf("\nOtherCard SET\n");*/
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],OtherCard_Relays_st[Traverser].RelayIPS_Buff,MAX_OUTPUTS_PER_PEER);
				break;
			case THISCARD_LINK_OK:
				/*printf("\nOnly This Card SET\n");*/
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Traverser].RelayIPS_Buff,MAX_OUTPUTS_PER_PEER);
				break;
			case BOTHCARD_LINK_OK:
				/*printf("\nBothCard SET3\n");*/
				memcpy(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],Main_Relays_st[Traverser].RelayIPS_Buff,MAX_OUTPUTS_PER_PEER);
				break;

			default:
				memset(&vEI_Transver_EI_To_MIE.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],0,MAX_OUTPUTS_PER_PEER);
			}
		}
		if(Is_CAN_Exchange_happening == NO)
		{
			printf("CAN Packet Not Receiving from EIPC1\n");
			printf("Sending Blank Relay statuses\n");
			memset(&vEI_Transver_EI_To_MIE,0,sizeof(vEI_Transver_EI_To_MIE));
			ReArrangedVitalData();
			TransmitCanPkt(EI_PROCESS_DATA);
		}

		if(Debug_flag == 1)
		{
			printf("Data Sending 2 MIE T:%02u:%02u:%02u\n",RTC_Date_time_Dec_st.Hour,RTC_Date_time_Dec_st.Mins,RTC_Date_time_Dec_st.Secs);
			//		for(uint16_t Byte = 0; Byte<Num_of_Bytes;++Byte )
			ReArrangedVitalData();
			for(uint16_t Byte = 0; Byte<(EI_data.Hitachi_st.PeersCnt * MAX_OUTPUTS_PER_PEER);++Byte )

			{
				if(ReorderFlag == RESET)
				{
					printf("%02X ",vEI_Transver_EI_To_MIE.Inputs[Byte]);
				}
				else
				{
					printf("%02X ",HitachiEI_DataReArranged_st.Inputs[Byte]);
				}
				if(((Byte+1)%16)==0)
				{
					printf("\n");
				}
			}
			printf("\n");
		}
#if ENABLE_SIGNAL_PRINT
		PrintSignalsHitachi();
#endif

#endif
		MIE_Signal_Send_CLK = HAL_GetTick();
		UNUSED(Choice);
	}
}

#if 0
void Print_EI_DATA(uint8_t *Msg,uint8_t Msg_Status,uint8_t Length)
{
	char Print_buff[100] = {0,};
	uint8_t Msg_type;
	uint16_t Msg_status_flag;
	uint8_t Traverser = 0;

	Display("\n___________________________________________________\n");
	Display(" MSG STATUS: ");

	Msg_Status?Display("RCV      |"):Display("XMT      |");

	//sprintf(Print_buff,"D_ADDR: %04lX  |",ExtractBits(0,16,&Msg[1]));
	sprintf(Print_buff,"D_ADDR: %04lu  |",ExtractBits(0,16,&Msg[1]));
	Display(Print_buff);

	//sprintf(Print_buff,"S_ADDR: %04lX|\n",ExtractBits(0,16,&Msg[3]));
	sprintf(Print_buff,"S_ADDR: %04lu|\n",ExtractBits(0,16,&Msg[3]));
	Display(Print_buff);

	Msg_type = ExtractBits(0,8,&Msg[7]);
	Display(" MSG TYPE  : ");
	switch(Msg_type)
	{
	case 1:
		Msg_Status?Display("INIT_R   "):Display("INIT_X   ");
		//Display("INIT     ");
		break;
	case 2:
		Display("VITALDATA");
		break;
	case 3:
		Display("NONVITAL ");
		break;
	case 4:
		Display("STATUS   ");
		break;
	case 6:
		Display("SERVICE  ");
		break;
	default:
		Display("UNKNOWN MESSAGE\n");
	}
	Display("|");

	sprintf(Print_buff,"TX_SEQ: %02lX    |",ExtractBits(0,8,&Msg[5]));
	Display(Print_buff);

	sprintf(Print_buff,"RX_SEQ: %02lX  |",ExtractBits(0,8,&Msg[6]));
	Display(Print_buff);

	Msg_status_flag = ExtractBits(0,16,&Msg[8]);
	if( ((Msg_status_flag >> 5)&1) == 1)
	{
		sprintf(Print_buff,"\n DATE      : %02X/%02X/%02X |TIME: %02X:%02X:%02X|\n",Msg[10],Msg[11],Msg[12],Msg[13],Msg[14],Msg[15]);
		Display(Print_buff);
	}
	else
	{
		Display("\n NO TIME SPECIFIED\n");
	}
	for(Traverser = 0;Traverser<Length;++Traverser)
	{
		sprintf(Print_buff," %02X",Msg[Traverser]);
		Display(Print_buff);
		if( (Traverser>0) && ((Traverser%16) == 0) )
		{
			Display("\n");
		}
	}
	if(Msg_Status == XMT)
	{
		sprintf(Print_buff," %02X",0xF6);
		Display(Print_buff);
	}
	Display("\n___________________________________________________\n");
}
#endif

void Print_EI_DATA(uint8_t *Msg,uint8_t Msg_Status,uint8_t Length)
{
	char Print_buff[512] = {0,};
	uint8_t Msg_type;
	uint16_t Msg_status_flag;
	uint8_t Traverser = 0;

	//	Display("\n________________________________________MSG DETAILS________________________________________\n");
	//Display("\nMSG STATUS:");

	Msg_Status?Display("\nR|"):Display("\nX|");

	//sprintf(Print_buff,"D_ADDR: %04lX  |",ExtractBits(0,16,&Msg[1]));
	sprintf(Print_buff,"DA:%lu|",ExtractBits(0,16,&Msg[1]));
	Display(Print_buff);

	//sprintf(Print_buff,"S_ADDR: %04lX|\n",ExtractBits(0,16,&Msg[3]));
	sprintf(Print_buff,"SA:%lu|",ExtractBits(0,16,&Msg[3]));
	Display(Print_buff);

	Msg_type = ExtractBits(0,8,&Msg[7]);
	Display("MT:");
	switch(Msg_type)
	{
	case 1:
		Msg_Status?Display("INIT_R"):Display("INIT_X");
		//Display("INIT     ");
		break;
	case 2:
		Display("VIT");
		break;
	case 4:
		Display("STS");
		break;
	case 6:
		Display("SER");
		break;
	default:
		Display("UNKNOWN MESSAGE\n");
	}
	Display("|");

	sprintf(Print_buff,"TX:%02lX|",ExtractBits(0,8,&Msg[5]));
	Display(Print_buff);

	sprintf(Print_buff,"RX:%02lX|",ExtractBits(0,8,&Msg[6]));
	Display(Print_buff);

	Msg_status_flag = ExtractBits(0,16,&Msg[8]);
	sprintf(Print_buff,"MSF:%04X|",Msg_status_flag);
	Display(Print_buff);
	if( ((Msg_status_flag >> 5)&1) == 1)
	{
		sprintf(Print_buff,"DT:%02X%02X%02X|T:%02X:%02X:%02X\n",Msg[10],Msg[11],Msg[12],Msg[13],Msg[14],Msg[15]);
		Display(Print_buff);
	}
	else
	{
		Display("NO TIME SPECIFIED\n");
	}
	if(Debug_flag == 1)
	{
		for(Traverser = 0;Traverser<Length;++Traverser)
		{
			sprintf(Print_buff,"%02X ",Msg[Traverser]);
			Display(Print_buff);
			//		if( (Traverser>0) && ((Traverser%16) == 0) )
			//		{
			//			Display("\n");
			//		}
		}
		if(Msg_Status == XMT)
		{
			sprintf(Print_buff,"%02X ",0xF6);
			Display(Print_buff);
		}
	}
	//	Display("\n");
	//Display("\n__________ MSG DETAILS __________\n");
}

void Display(char *Data)
{
	uint8_t Traverser = 0;
	for(;Data[Traverser];++Traverser)
	{
		Com_SendByte(0,Data[Traverser]);
	}
}

void Send_Health_Status(void)
{
	static uint32_t Health_clock = 0;
	uint8_t Traverser = 0;

	memset(&Hitachi_fault_logging_st,0,sizeof(HITACHI_FAULT_LOGGING_t));
	Hitachi_fault_logging_st.No_of_Peers   = EI_data.Hitachi_st.PeersCnt;
	Hitachi_fault_logging_st.CAN_Receiving = Is_CAN_Exchange_happening;

	for(Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		if(Tx_Ei_self_sharing_st[Traverser].EI_Receiving_Status == RECEIVED)
		{
			SET_BIT_USER(Link_Status,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		else
		{
			CLEAR_BIT_USER(Link_Status,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}

		switch(IP_Source_Peer[Traverser])
		{
		case ZERO:
			CLEAR_BIT_USER(Receiving_From_A,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			CLEAR_BIT_USER(Receiving_From_B,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			break;
		case ONE:
			SET_BIT_USER(Receiving_From_A,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			CLEAR_BIT_USER(Receiving_From_B,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			break;
		case TWO:
			CLEAR_BIT_USER(Receiving_From_A,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			SET_BIT_USER(Receiving_From_B,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			break;

		default:
			printf("IP_Source is %u\n",IP_Source_Peer[Traverser]);
			CLEAR_BIT_USER(Receiving_From_A,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			CLEAR_BIT_USER(Receiving_From_B,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		if(App_Identity_CRC_Fail[Traverser] == 1)
		{
			SET_BIT_USER(App_CRC_MisMatch,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		else
		{
			CLEAR_BIT_USER(App_CRC_MisMatch,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}

		if(Tx_Ei_self_sharing_st[Traverser].IS_PRIMARY == RECEIVED)
		{
			SET_BIT_USER(EiPC_Primary,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		else
		{
			CLEAR_BIT_USER(EiPC_Primary,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		if(HAL_Elaps_Ticks(Last_ReceptionClk[Traverser]) > EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser])
		{
			SET_BIT_USER(LAST_RECV_LOGGING_TIMEOUT_ELAPSED,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		else
		{
			CLEAR_BIT_USER(LAST_RECV_LOGGING_TIMEOUT_ELAPSED,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		if(Received_Init[Traverser] == SET)
		{
			SET_BIT_USER(INIT_RECEIVED,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			Received_Init[Traverser] = RESET;
		}
		else
		{
			CLEAR_BIT_USER(INIT_RECEIVED,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		if(Received_Vital[Traverser] == SET)
		{
			SET_BIT_USER(VITAL_MSG_RECEIVED,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			Received_Vital[Traverser] = RESET;
		}
		else
		{
			CLEAR_BIT_USER(VITAL_MSG_RECEIVED,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
		if(INIT_REQ_SENT_Flag[Traverser] ==  SET)
		{
			SET_BIT_USER(INIT_REQ_SENT,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
			INIT_REQ_SENT_Flag[Traverser] = RESET;
		}
		else
		{
			CLEAR_BIT_USER(INIT_REQ_SENT,Hitachi_fault_logging_st.Fault_Bits[Traverser]);
		}
	}


	if(memcmp(&Prev_Hitachi_fault_logging_st,&Hitachi_fault_logging_st,sizeof(Hitachi_fault_logging_st)) != 0)
	{
		memcpy(&Prev_Hitachi_fault_logging_st,&Hitachi_fault_logging_st,sizeof(Hitachi_fault_logging_st));

		printf("PEER_CNT:%u\n",Hitachi_fault_logging_st.No_of_Peers);
		printf("CAN RCV_STS:%u\n",Hitachi_fault_logging_st.CAN_Receiving);

		for (uint8_t var = 0; var < Hitachi_fault_logging_st.No_of_Peers; ++var)
		{
			printf("Health[%u]:0x%04X\n",var,Hitachi_fault_logging_st.Fault_Bits[var]);
		}
		printf("\n");

#ifdef EI_1
		TransmitCanPkt(EiPC_1_HEALTH);
#endif
#ifdef EI_2
		TransmitCanPkt(EiPC_2_HEALTH);
#endif
		Health_clock = HAL_GetTick();
	}
	else if(HAL_Elaps_Ticks(Health_clock)>= EI_data.Hitachi_st.HEALTH_PACKET_TIME)
	{
		printf("PEER_CNT:%u\n",Hitachi_fault_logging_st.No_of_Peers);
		printf("CAN RCV_STS:%u\n",Hitachi_fault_logging_st.CAN_Receiving);

		for (uint8_t var = 0; var < Hitachi_fault_logging_st.No_of_Peers; ++var)
		{
			printf("Fault Log[%u]:0x%04X\n",var,Hitachi_fault_logging_st.Fault_Bits[var]);
		}
		printf("\n");

#ifdef EI_1
		TransmitCanPkt(EiPC_1_HEALTH);
#endif
#ifdef EI_2
		TransmitCanPkt(EiPC_2_HEALTH);
#endif
		Health_clock = HAL_GetTick();
	}

}

void print_self_Details_Hitachi(void)
{
	uint8_t Traverser = 0;

#ifdef EI_1
	printf("\n______________________________ EI_1_SELF_DETAILS ______________________________\n");
#endif
#ifdef EI_2
	printf("\n______________________________ EI_2_SELF_DETAILS ______________________________\n");
#endif

	printf("EI_data.Hitachi_st.No_of_IP_s:%u\n",EI_data.Hitachi_st.No_of_IP_s);
	printf("EI_data.Hitachi_st.CanXchange:%lu\n",EI_data.Hitachi_st.CAN_Xchange_Time);
	printf("EI_data.Hitachi_st.CanXchangeTimeout:%lu\n",EI_data.Hitachi_st.CAN_EXCHANGE_TIME_OUT);
	printf("EI_data.Hitachi_st.MIE_SIGNAL_SEND_CLK:%lu\n",EI_data.Hitachi_st.MIE_SIGNAL_SEND_CLK);
	printf("EI_data.Hitachi_st.HEALTH_PACKET_TIME:%lu\n",EI_data.Hitachi_st.HEALTH_PACKET_TIME);
	printf("EI_data.Hitachi_st.Self_Listening_Port:%u\n",EI_data.Hitachi_st.Self_Listening_Port[CARD_INDEX]);
	printf("EI_data.Hitachi_st.No_of_IP_s:%u\n",EI_data.Hitachi_st.No_of_IP_s);


	for(Traverser = 0;Traverser<EI_data.Hitachi_st.No_of_IP_s;++Traverser)
	{
		printf("Source IP[%u]  :%u.%u.%u.%u\n",Traverser,EI_data.Hitachi_st.Source_IP[CARD_INDEX][Traverser][0],EI_data.Hitachi_st.Source_IP[CARD_INDEX][Traverser][1],EI_data.Hitachi_st.Source_IP[CARD_INDEX][Traverser][2],EI_data.Hitachi_st.Source_IP[CARD_INDEX][Traverser][3]);
		printf("TX_Port[%u]:%u\n",Traverser,EI_data.Hitachi_st.PORTS[CARD_INDEX][Traverser][TX_PORT]);
		printf("RX_Port[%u]:%u\n",Traverser,EI_data.Hitachi_st.PORTS[CARD_INDEX][Traverser][RX_PORT]);
	}
	printf("Self IP       :%u.%u.%u.%u\n",EI_data.Hitachi_st.Self_IP[CARD_INDEX][0],EI_data.Hitachi_st.Self_IP[CARD_INDEX][1],EI_data.Hitachi_st.Self_IP[CARD_INDEX][2],EI_data.Hitachi_st.Self_IP[CARD_INDEX][3]);
	printf("Self MAC      :%02X:%02X:%02X:%02X:%02X:%02X\n",EI_data.Hitachi_st.Self_MAC[CARD_INDEX][0],EI_data.Hitachi_st.Self_MAC[CARD_INDEX][1],EI_data.Hitachi_st.Self_MAC[CARD_INDEX][2],EI_data.Hitachi_st.Self_MAC[CARD_INDEX][3],EI_data.Hitachi_st.Self_MAC[CARD_INDEX][4],EI_data.Hitachi_st.Self_MAC[CARD_INDEX][5]);

	printf("Listening Port:%u\n",EI_data.Hitachi_st.Self_Listening_Port[CARD_INDEX]);

	for(Traverser=0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		printf("\nPEER:%u Details:\n",Traverser+1);
		printf("Source Peer Address:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
		printf("Dest   Peer Address:%lu\n",EI_data.Hitachi_st.DEST_ADDRESSES[Traverser]);
		printf("MLK Application CRC:0x%04X\n",EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][Traverser]);
		printf("Number of Outputs  :%u\n",EI_data.Hitachi_st.Bool_outputs[Traverser]);
		printf("Clock Master:%u\n",EI_data.Hitachi_st.ClockMaster[Traverser]);
		printf("Interface Type:%u\n",EI_data.Hitachi_st.Interface_Type[Traverser]);
		printf("COMM_FAILURE_TIME_OUT:%lu\n",EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser]);
		printf("STALE_DATA_TIME_OUT:%lu\n",EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser]);
		printf("LinkEstablishTimeout:%lu\n",EI_data.Hitachi_st.LinkEstablishTimeout[Traverser]);

	}

#ifdef EI_1
	printf("\n______________________________ EI_1_SELF_DETAILS ______________________________\n");
#endif
#ifdef EI_2
	printf("\n______________________________ EI_2_SELF_DETAILS ______________________________\n");
#endif
}

void Send_Init_After_Timeout(void)
{
	uint8_t Traverser =0;

	for(Traverser=0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		if(Last_Recevied_Clock[Traverser] != 0)
		{
#ifdef EI_1
			if(
					(
							( (HAL_Elaps_Ticks(Last_Recevied_Clock[Traverser]) > EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser]) &&
									(HAL_Elaps_Ticks(OtherCardReceptionClk[Traverser]) > EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser])) &&

									(Init_Msg_Sent_After_Timeout[Traverser] != 1) &&
									(Last_Received_Init_Clk[Traverser] > ONE_SEC) )
			)
			{
				printf("Sending INIT-MSG After Timeout:%lu Seconds\n",HAL_Elaps_Ticks(Last_Recevied_Clock[Traverser])/1000);
				SEND_FORCE_REPLY[Traverser] = 1;
				Send_Init_Msg(Traverser, Traverser);
				Init_Msg_Sent_After_Timeout[Traverser] = 1;
				INIT_REQ_SENT_Flag[Traverser] = 1;

			}
#endif
#ifdef EI_2
			if(
					(
							( (HAL_Elaps_Ticks(Last_Recevied_Clock[Traverser]) > (EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser] + ONE_SEC)) &&
									(HAL_Elaps_Ticks(OtherCardReceptionClk[Traverser]) > (EI_data.Hitachi_st.STALE_DATA_TIME_OUT[Traverser] + ONE_SEC))) &&

									(Init_Msg_Sent_After_Timeout[Traverser] != 1) &&
									(Last_Received_Init_Clk[Traverser] > ONE_SEC) )
			)
			{

				printf("Sending INIT-MSG After Timeout:%lu Seconds\n",HAL_Elaps_Ticks(Last_Recevied_Clock[Traverser])/1000);
				SEND_FORCE_REPLY[Traverser] = 1;
				Send_Init_Msg(Traverser, Traverser);
				Init_Msg_Sent_After_Timeout[Traverser] = 1;
				INIT_REQ_SENT_Flag[Traverser] = 1;
			}
#endif
		}
	}
}

void Send_Vital_Msg_After_Rx_VITAL(void)
{
	uint8_t Traverser = 0;
	for(;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		/*		if(
				(Tx_Ei_self_sharing_st.IS_PRIMARY[Traverser] == PRIMARY) &&
				(Send_Vital_MSG_st.flag[Traverser] == SET) &&
				(HAL_Elaps_Ticks(Send_Vital_MSG_st.Clk[Traverser]) >= 150 )
		)
		{
			Send_Vital_Data_Message(Traverser, Traverser);
			Send_Vital_MSG_st.flag[Traverser] = RESET;
			Send_Vital_MSG_st.Clk[Traverser] = -1;
		}*/

		if(
				((Tx_Ei_self_sharing_st[Traverser].IS_PRIMARY == PRIMARY) || (EI_data.Hitachi_st.Interface_Type[Traverser] == SERIAL)) &&
				(Send_Vital_MSG_st.flag[Traverser] == SET) &&
				(HAL_Elaps_Ticks(Send_Vital_MSG_st.Clk[Traverser]) > 50 )
		)
		{
			Send_Vital_Data_Message(Traverser, Traverser);
			Send_Vital_MSG_st.flag[Traverser] = RESET;
			Send_Vital_MSG_st.Clk[Traverser] = -1;
		}

		//		if( (Send_Vital_MSG_st.flag[Traverser] == SET) &&(HAL_Elaps_Ticks(Send_Vital_MSG_st.Clk[Traverser]) > 20 ) )
		//		{
		//			Send_Vital_Data_Message(Traverser, Traverser);
		//			Send_Vital_MSG_st.flag[Traverser] = RESET;
		//			Send_Vital_MSG_st.Clk[Traverser] = -1;
		//		}
	}
}

void Check_Recv_Status_fail(void)
{
	for(uint8_t Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		if( (Last_Recevied_Clock_ValidMsgs[Traverser] != 0) && (Tx_Ei_self_sharing_st[Traverser].EI_Receiving_Status != NOT_RECEIVED) )
		{
			if(HAL_Elaps_Ticks(Last_Recevied_Clock_ValidMsgs[Traverser]) > EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser])
			{
				printf("!Received any Packet of:%lu from last %lu Seconds\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser],(EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser]/1000));
				printf("Last_Recevied_Clock_ValidMsgs[Traverser]:%lu\n",Last_Recevied_Clock_ValidMsgs[Traverser]);
				printf("Ticks:%lu\n",HAL_GetTick());
				Tx_Ei_self_sharing_st[Traverser].EI_Receiving_Status = NOT_RECEIVED;
			}
		}
	}
}

uint8_t IS_MLK_EIPC_TIME_MATCHING(uint8_t Source_Index)
{
	uint32_t Hitachi_Frame = 0;
	uint32_t Self_Frame = 0;
	uint8_t Return_Value = 1;

	if( (EI_data.Hitachi_st.ClockMaster[Source_Index] == SET) && (Is_GPS_Packet_Received == 1) )
	{
		Hitachi_Frame = ( ( (bcdToDec(HITACHI_time_Stamp_st[Source_Index].hh) * 3600) +
				(bcdToDec(HITACHI_time_Stamp_st[Source_Index].mm) * 60) +
				bcdToDec(HITACHI_time_Stamp_st[Source_Index].ss)) + 1);

		Get_time();

		Self_Frame = ( (RTC_Date_time_Dec_st.Hour * 3600) +
				(RTC_Date_time_Dec_st.Mins * 60) +
				(RTC_Date_time_Dec_st.Secs) + 1);


		if(abs(Hitachi_Frame - Self_Frame) > 1)
		{
			printf("Time Difference %lu Seconds found in MLK & EIPC for Peer:%lu\n",labs(Hitachi_Frame - Self_Frame),EI_data.Hitachi_st.SOURCE_ADDRESSES[Source_Index]);
			Return_Value  = 0;
		}
	}

	return Return_Value;
}

void Check_All_Peers(void)
{
	static uint32_t PrevLinkFailClk[MAX_PEER_CONNECTIONS] = {0,};

	for(uint8_t Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		Check_Vital_CRC(Traverser);

		if( (HAL_Elaps_Ticks(Hitachi_clk_flags_st[Traverser].Link_EstablishClk)) > EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[Traverser])
		{
			if((Hitachi_clk_flags_st[Traverser].Link_EstablishFlag == SET) && (PrevLinkFailClk[Traverser] != Hitachi_clk_flags_st[Traverser].Link_EstablishClk) )
			{
				Hitachi_clk_flags_st[Traverser].Link_EstablishFlag = RESET;
				PrevLinkFailClk[Traverser] = HAL_GetTick();
			}
		}
	}
}

void Check_for_RelayMisMatch(void)
{
	static uint32_t Check_Clk[MAX_PEER_CONNECTIONS] = {0,};

	if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
	{
		return;
	}

	for(uint8_t Traverser = 0;Traverser <EI_data.Hitachi_st.PeersCnt;++Traverser)
	{
		if(EI_data.Hitachi_st.Interface_Type[Traverser] == ETHERNET)
		{
			continue;
		}

		if((HAL_Elaps_Ticks(Check_Clk[Traverser]) < FIVE_SEC ) || (MisMatchFlag[Traverser] == SET))
		{
			if( (OtherCardLink_Flag[Traverser] == SET) && (SelfLinkFlag[Traverser] == RESET))
			{
				//				printf("This Card Link is Clear and Other Link is SET for Peer:%lu\n",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
				//				printf("Copying from Other Card\n");
				memcpy(&Main_Relays_st[Traverser],&OtherCard_Relays_st[Traverser],sizeof(RELAYS_t));
				//				printf("Self CRC:%04x\n",Main_Relays_st[Traverser].RelayIPS_CRC);
			}

			if(Main_Relays_st[Traverser].RelayIPS_CRC !=  OtherCard_Relays_st[Traverser].RelayIPS_CRC)
			{
				/* If Signals are Recently updated we are waiting to check CRC after ONE_SEC */
				if(HAL_Elaps_Ticks(Last_Vital_Received_Clk[Traverser]) < ONE_SEC)
				{
					continue;
				}
				else
				{
					MisMatchFlag[Traverser] = SET;
					if( ((Main_Relays_st[Traverser].RelayIPS_CRC == 0) && (OtherCard_Relays_st[Traverser].RelayIPS_CRC != 0)) || ((Main_Relays_st[Traverser].RelayIPS_CRC != 0) && (OtherCard_Relays_st[Traverser].RelayIPS_CRC == 0)) )
					{
						MisMatchFlag[Traverser] = RESET;
					}
					if(MisMatchFlag[Traverser] ==  SET)
					{
						printf("S:%u|O:%u\n",SelfLinkFlag[Traverser],OtherCardLink_Flag[Traverser]);
						printf("Peer:%lu ",EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser]);
						printf("SelfCRC:%04x ",Main_Relays_st[Traverser].RelayIPS_CRC);
						printf("OtherCRC:%04x\n",OtherCard_Relays_st[Traverser].RelayIPS_CRC);
					}
				}
			}
			else
			{
				MisMatchFlag[Traverser] = RESET; /* If both Relay CRC is Matching MisMatch will be RESET */
			}
		}
		Check_Clk[Traverser] = HAL_GetTick();
	}
}

void CheckOtherLinkStatus(uint8_t SourceIndex)
{
	uint32_t CurTicks = HAL_GetTick();

	if(Rx_Ei_self_sharing_st[SourceIndex].LinkEstablishFlag == SET)
	{
		OtherCardLink_Establish_Clock[SourceIndex] = CurTicks;
		Other_Hitachi_clk_flags_st[SourceIndex].Link_EstablishClk = CurTicks;
		Other_Hitachi_clk_flags_st[SourceIndex].Link_EstablishFlag = SET;
		OtherCardLink_Flag[SourceIndex] = SET;
		/* Checking the Link Status */
	}
	else
	{
		OtherCardLink_Flag[SourceIndex] = RESET;
		Other_Hitachi_clk_flags_st[SourceIndex].Link_FailClk = CurTicks;
		Other_Hitachi_clk_flags_st[SourceIndex].Link_EstablishFlag = RESET;
		OtherCard_Relays_st[SourceIndex].RelayIPS_CRC = 0;
		memset(&OtherCard_Relays_st[SourceIndex].RelayIPS_Buff,0,sizeof(OtherCard_Relays_st[SourceIndex].RelayIPS_Buff));
	}

	if(Rx_Ei_self_sharing_st[SourceIndex].Msg_Rcv_Flag == SET)
	{
		OtherCardReceptionClk[SourceIndex] = CurTicks;
	}
}

void SendRelay2_Other(void)
{
	static uint32_t RelaySendClk =0;
	static uint32_t Traverser = 0;

	if(HAL_Elaps_Ticks(RelaySendClk) > ONE_SEC)
	{

		memset(&Tx_Temp_Main_Relays_st,0,sizeof(RELAYS_t));
		memcpy(&Tx_Temp_Main_Relays_st,&Main_Relays_st[Traverser],sizeof(RELAYS_t));

		//		printf("P:%lu ",EI_data.Hitachi_st.SOURCE_ADDRESSES[Tx_Temp_Main_Relays_st.Peer_Index]);
		//		printf("I:%u ",Tx_Temp_Main_Relays_st.No_ofIps);
		//		printf("C:%04x\n",Tx_Temp_Main_Relays_st.RelayIPS_CRC);
		//		if(Traverser == 0)
		//		{
		//			printf("\nP:%lu ",EI_data.Hitachi_st.SOURCE_ADDRESSES[Tx_Temp_Main_Relays_st.Peer_Index]);
		//			printf("I:%u ",Tx_Temp_Main_Relays_st.No_ofIps);
		//			printf("C:%04X\n",Tx_Temp_Main_Relays_st.RelayIPS_CRC);
		//		}

#ifdef EI_1
		TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
		TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif

		++Traverser;
		if(Traverser == EI_data.Hitachi_st.PeersCnt)
		{
			Traverser = 0;
		}

		RelaySendClk = HAL_GetTick();
	}
}




void Process_HitachiEI(void)
{
	while(1)
	{
		process_to_comReceived();

		Check_OnlineDiag_OFC_Modem();

		MX_LWIP_Process(); //This funtion will get the Ethernet data.

		HAL_IWDG_Refresh(&hiwdg);

		Feed_wdt();

		readCanToStructs();

		Check_for_cmd();

		Process_Hitachi_Packets_Eth();

		Process_Hitachi_Packets_Com();

		Process_Send_Signals_2_MIE();

		Send_Self_Sharing();

		Detect_CAN_Xchange_Failure();

		//		Send_Health_Status();

		CheckLinkEstablishTimeout();

		Check_All_Peers();

		Send_Vital_Msg_After_Rx_VITAL();

		Send_Init_After_Timeout();

		Check_Recv_Status_fail();

		Update_Time_Every_Sec();

		Check_for_RelayMisMatch();

		SendRelay2_Other();

		Print_BuildDat_Details();

		CheckEIPC_DAT_CRC();

		if(Dats_Request_Flag > 0)
		{
			gucFlagUpdate = 3;
			break;
		}

		Enable_UART_Interrupts();

	}

	while(1)
	{
		Data_Processing();
		HAL_IWDG_Refresh(&hiwdg);
	}
}


#if ENABLE_SIGNAL_PRINT

void UpdateSIGNALSNamesHitachi(void)
{
	uint16_t Signals_Cnt = 0;
	Signals_Cnt = 128+128+128+128;
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"92_ONLINEBIT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A513DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A513HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A513HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A513RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507AT2RX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509ATRX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510AT1RX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510AT2RX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512ATRX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"2AT2RX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"2XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"2AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A509AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A511AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A512AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A513XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A513AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505AT1RX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505AT2RX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505AT2RX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507AT1RX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507AT2RX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508AT1RX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508AT2RX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508AT2RX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A510AT2RX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A507AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A508AXT");
	printf("Signals_Cnt:---------------:%u\n",Signals_Cnt);
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE3");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE4");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE5");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE6");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE7");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE8");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE9");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE10");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE11");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE12");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE13");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE14");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE15");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE16");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE17");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE18");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE19");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE20");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE21");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE22");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE23");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE24");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE25");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE26");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE27");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE28");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE29");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE30");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE31");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE32");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE33");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE34");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE35");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE36");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE37");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE38");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE39");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE40");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE41");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE42");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE43");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE44");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE45");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE46");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE47");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE48");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE49");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE50");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE51");


	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505DECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505HHECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505HECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505RECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504AMGECR");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"19ATRX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501ATRX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503ATRX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503ATRX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504AT1RX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504AT2RX1");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504AT2RX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A505TRX");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506ATRX2");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"19XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"19AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A501AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A502AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A503AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504XT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A504AXT");
	strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"A506AXT");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE52");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE53");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE54");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE55");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE56");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE57");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE58");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE59");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE60");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE61");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE62");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE63");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE64");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE65");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE66");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE67");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE68");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE69");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE70");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE71");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE72");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE73");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE74");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE75");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE76");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE77");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE78");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE79");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE80");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE81");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE82");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE83");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE84");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE85");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE86");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE87");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE88");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE89");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE90");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE91");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE92");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE93");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE94");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE95");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE96");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE97");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE98");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE99");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE100");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE101");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE102");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE103");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE104");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE105");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE106");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE107");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE108");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE109");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE110");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE111");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE112");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE113");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE114");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE115");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE116");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE117");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE118");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE119");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE120");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE121");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE122");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE123");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE124");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE125");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE126");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE127");
	//		strcpy((char *)SignalsHitachi[Signals_Cnt++], (const char *)"SPARE128");



	TotalSigCountHitachi = Signals_Cnt;


}


void PrintSignalsHitachi(void)
{
	//	memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);
	static uint8_t PrevRelays[1024] = {0,};
	uint8_t EnablePrint = 0;


	//if(memcmp(&PrevRelays,&vEI_Transver_EI_To_MIE.Inputs,(EI_data.Hitachi_st.PeersCnt*MAX_OUTPUTS_PER_PEER))!= 0)
	{
		EnablePrint = 1;
	}

	if(EnablePrint == 1)
	{
		memcpy(&PrevRelays,&vEI_Transver_EI_To_MIE.Inputs,(EI_data.Hitachi_st.PeersCnt*MAX_OUTPUTS_PER_PEER));
		Get_time();
		printf("Time:%02u:%02u:%02u\n",RTC_Date_time_Dec_st.Hour,RTC_Date_time_Dec_st.Mins,RTC_Date_time_Dec_st.Secs);
		for(uint16_t Traverser = 513;Traverser<=(vEI_Transver_EI_To_MIE.ActualDataBytes * 8);++Traverser)
		{

			if(Traverser > TotalSigCountHitachi)
			{
				continue;
			}

			if((Traverser >= 589 ) && (Traverser <641) )
			{
				continue;
			}

			if(GETPINHITACHI(Traverser) > 0)
			{
				printf("%15s:PICKUP   ",SignalsHitachi[Traverser-1]);
			}
			else
			{
				printf("%15s:DROP      ",SignalsHitachi[Traverser-1]);
			}

			if( (Traverser%4) == 0)
			{
				printf("\n");
			}
		}
	}
}

#endif


void ReAssignData(void)
{
	EI_t EI_dataTempEthernet,EI_dataTempSerial;
	uint8_t PeerTraverser = 0;
	bool SerialFlag = RESET;
	bool OrderOKFlag = SET;
	uint8_t EthernetIndex = 0;
	uint8_t SerialIndex = 0;



	printf("EI_data.Hitachi_st.EnableSerialPort:%u\n",EI_data.Hitachi_st.EnableSerialPort);
	if(EI_data.Hitachi_st.EnableSerialPort <= 0)
	{
		return;
	}

	memset(&EI_dataTempEthernet,0,sizeof(EI_t));
	memset(&EI_dataTempSerial,0,sizeof(EI_t));


	/* Check whether the order is valid */
	for(PeerTraverser= 0;PeerTraverser<EI_data.Hitachi_st.PeersCnt;++PeerTraverser)
	{
		if(EI_data.Hitachi_st.Interface_Type[PeerTraverser] == SERIAL)
		{
			SerialFlag = SET;
		}
		else if ((EI_data.Hitachi_st.Interface_Type[PeerTraverser] == ETHERNET) && (SerialFlag == SET))
		{
			printf("EI_data.Hitachi_st.EnableSerialPort11111111111:%u\n",EI_data.Hitachi_st.EnableSerialPort);
			OrderOKFlag = RESET;
		}
	}

	if(OrderOKFlag == RESET)
	{
		memcpy(&EI_BeforeReorderData,&EI_data,sizeof(EI_t));
		ReorderFlag = SET;
		printf("\n============================ CONFIG DATA IS NOT IN ORDER RE-ORDERING THE DATA ============================\n");

		for(PeerTraverser = 0;PeerTraverser<EI_data.Hitachi_st.PeersCnt;++PeerTraverser)
		{
			if(EI_data.Hitachi_st.Interface_Type[PeerTraverser] == ETHERNET)
			{
				EI_dataTempEthernet.Hitachi_st.ClockMaster[EthernetIndex] = EI_data.Hitachi_st.ClockMaster[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.Interface_Type[EthernetIndex] = EI_data.Hitachi_st.Interface_Type[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.SOURCE_ADDRESSES[EthernetIndex] = EI_data.Hitachi_st.SOURCE_ADDRESSES[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.DEST_ADDRESSES[EthernetIndex] = EI_data.Hitachi_st.DEST_ADDRESSES[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.App_Identity_CRC[CARD_INDEX][EthernetIndex] = EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.Bool_outputs[EthernetIndex] = EI_data.Hitachi_st.Bool_outputs[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.COMM_FAILURE_TIME_OUT[EthernetIndex] = EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.STALE_DATA_TIME_OUT[EthernetIndex] = EI_data.Hitachi_st.STALE_DATA_TIME_OUT[PeerTraverser];
				EI_dataTempEthernet.Hitachi_st.LinkEstablishTimeout[EthernetIndex] = EI_data.Hitachi_st.LinkEstablishTimeout[PeerTraverser];

				++EI_dataTempEthernet.Hitachi_st.PeersCnt;
				++EthernetIndex;
			}
			else if(EI_data.Hitachi_st.Interface_Type[PeerTraverser] == SERIAL)
			{
				EI_dataTempSerial.Hitachi_st.ClockMaster[SerialIndex] = EI_data.Hitachi_st.ClockMaster[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.Interface_Type[SerialIndex] = EI_data.Hitachi_st.Interface_Type[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.SOURCE_ADDRESSES[SerialIndex] = EI_data.Hitachi_st.SOURCE_ADDRESSES[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.DEST_ADDRESSES[SerialIndex] = EI_data.Hitachi_st.DEST_ADDRESSES[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.App_Identity_CRC[CARD_INDEX][SerialIndex] = EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][PeerTraverser];
				EI_dataTempSerial.Hitachi_st.Bool_outputs[SerialIndex] = EI_data.Hitachi_st.Bool_outputs[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.COMM_FAILURE_TIME_OUT[SerialIndex] = EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.STALE_DATA_TIME_OUT[SerialIndex] = EI_data.Hitachi_st.STALE_DATA_TIME_OUT[PeerTraverser];
				EI_dataTempSerial.Hitachi_st.LinkEstablishTimeout[SerialIndex] = EI_data.Hitachi_st.LinkEstablishTimeout[PeerTraverser];

				++EI_dataTempSerial.Hitachi_st.PeersCnt;
				++SerialIndex;
			}
		}

		EthernetIndex = 0;
		SerialIndex = 0;

		for(PeerTraverser = 0;PeerTraverser<EI_data.Hitachi_st.No_of_IP_s;++PeerTraverser)
		{
			EI_dataTempEthernet.Hitachi_st.Source_IP[CARD_INDEX][EthernetIndex][0] = EI_data.Hitachi_st.Source_IP[CARD_INDEX][PeerTraverser][0];
			EI_dataTempEthernet.Hitachi_st.Source_IP[CARD_INDEX][EthernetIndex][1] = EI_data.Hitachi_st.Source_IP[CARD_INDEX][PeerTraverser][1];
			EI_dataTempEthernet.Hitachi_st.Source_IP[CARD_INDEX][EthernetIndex][2] = EI_data.Hitachi_st.Source_IP[CARD_INDEX][PeerTraverser][2];
			EI_dataTempEthernet.Hitachi_st.Source_IP[CARD_INDEX][EthernetIndex][3] = EI_data.Hitachi_st.Source_IP[CARD_INDEX][PeerTraverser][3];

			EI_dataTempEthernet.Hitachi_st.PORTS[CARD_INDEX][EthernetIndex][TX_PORT] = EI_data.Hitachi_st.PORTS[CARD_INDEX][PeerTraverser][TX_PORT];
			EI_dataTempEthernet.Hitachi_st.PORTS[CARD_INDEX][EthernetIndex][RX_PORT] = EI_data.Hitachi_st.PORTS[CARD_INDEX][PeerTraverser][RX_PORT];
			++EthernetIndex;
		}


		EthernetIndex = 0;
		if(EI_dataTempEthernet.Hitachi_st.PeersCnt > 0)
		{
			for(PeerTraverser = 0;PeerTraverser<EI_dataTempEthernet.Hitachi_st.PeersCnt;++PeerTraverser)
			{
				EI_data.Hitachi_st.ClockMaster[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.ClockMaster[PeerTraverser];
				EI_data.Hitachi_st.Interface_Type[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.Interface_Type[PeerTraverser];
				EI_data.Hitachi_st.SOURCE_ADDRESSES[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.SOURCE_ADDRESSES[PeerTraverser];
				EI_data.Hitachi_st.DEST_ADDRESSES[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.DEST_ADDRESSES[PeerTraverser];
				EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][PeerTraverser] = EI_dataTempEthernet.Hitachi_st.App_Identity_CRC[CARD_INDEX][PeerTraverser];
				EI_data.Hitachi_st.Bool_outputs[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.Bool_outputs[PeerTraverser];
				EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.COMM_FAILURE_TIME_OUT[PeerTraverser];
				EI_data.Hitachi_st.STALE_DATA_TIME_OUT[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.STALE_DATA_TIME_OUT[PeerTraverser];
				EI_data.Hitachi_st.LinkEstablishTimeout[PeerTraverser] = EI_dataTempEthernet.Hitachi_st.LinkEstablishTimeout[PeerTraverser];
				++EthernetIndex;
			}
		}
		if(EI_dataTempSerial.Hitachi_st.PeersCnt > 0)
		{
			for(PeerTraverser = 0 ;PeerTraverser<EI_dataTempSerial.Hitachi_st.PeersCnt;++PeerTraverser)
			{
				EI_data.Hitachi_st.ClockMaster[EthernetIndex] = EI_dataTempSerial.Hitachi_st.ClockMaster[PeerTraverser];
				EI_data.Hitachi_st.Interface_Type[EthernetIndex] = EI_dataTempSerial.Hitachi_st.Interface_Type[PeerTraverser];
				EI_data.Hitachi_st.SOURCE_ADDRESSES[EthernetIndex] = EI_dataTempSerial.Hitachi_st.SOURCE_ADDRESSES[PeerTraverser];
				EI_data.Hitachi_st.DEST_ADDRESSES[EthernetIndex] = EI_dataTempSerial.Hitachi_st.DEST_ADDRESSES[PeerTraverser];
				EI_data.Hitachi_st.App_Identity_CRC[CARD_INDEX][EthernetIndex] = EI_dataTempSerial.Hitachi_st.App_Identity_CRC[CARD_INDEX][PeerTraverser];
				EI_data.Hitachi_st.Bool_outputs[EthernetIndex] = EI_dataTempSerial.Hitachi_st.Bool_outputs[PeerTraverser];
				EI_data.Hitachi_st.COMM_FAILURE_TIME_OUT[EthernetIndex] = EI_dataTempSerial.Hitachi_st.COMM_FAILURE_TIME_OUT[PeerTraverser];
				EI_data.Hitachi_st.STALE_DATA_TIME_OUT[EthernetIndex] = EI_dataTempSerial.Hitachi_st.STALE_DATA_TIME_OUT[PeerTraverser];
				EI_data.Hitachi_st.LinkEstablishTimeout[EthernetIndex] = EI_dataTempSerial.Hitachi_st.LinkEstablishTimeout[PeerTraverser];
				++EthernetIndex;

			}
		}

		print_self_Details_Hitachi();

	}
	else
	{
		printf("\nNo PROBLEM Found\n");
	}

}

void ReArrangedVitalData(void)
{
	if(ReorderFlag == SET)
	{
		memset(&HitachiEI_DataReArranged_st,0,sizeof(EI_VITAL_DATA_TO_VITAL_COMPUTER_t));
		HitachiEI_DataReArranged_st.ActualDataBytes = vEI_Transver_EI_To_MIE.ActualDataBytes;

		for(uint8_t Traverser = 0;Traverser<EI_data.Hitachi_st.PeersCnt;++Traverser)
		{
			for(uint8_t PeerFinder= 0;PeerFinder<EI_data.Hitachi_st.PeersCnt;++PeerFinder)
			{
				if(EI_data.Hitachi_st.SOURCE_ADDRESSES[Traverser] == EI_BeforeReorderData.Hitachi_st.SOURCE_ADDRESSES[PeerFinder])
				{
					memcpy(&HitachiEI_DataReArranged_st.Inputs[Traverser * MAX_OUTPUTS_PER_PEER],&vEI_Transver_EI_To_MIE.Inputs[PeerFinder * MAX_OUTPUTS_PER_PEER],MAX_OUTPUTS_PER_PEER);
					break;
				}
			}
		}
	}
}
