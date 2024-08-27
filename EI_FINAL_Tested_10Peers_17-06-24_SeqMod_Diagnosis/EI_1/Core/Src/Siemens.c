/*
 * Siemens.c
 *
 *  Created on: Sep 25, 2023
 *      Author: Ravi Teja P
 */


#include "Headers.h"
#include "lwip.h"

SIEMENS_PKT_t Siemens_RX_st,Siemens_TX_st;
SIEMENS_PKT_t Temp_SiemensTx_st;
SIEMENS_TX_PKT_t Siemens_tx_Buffer_st;
SIEMENS_FAULT_INJECTION_t Siemens_fault_injection_st;
SIEMENS_SELF_SHARING_t Tx_Siemens_self_sharing_st,Rx_Siemens_self_sharing_st,Temp_Siemens_self_sharing_st;


SIEMENS_CLOCKS_t SiemensClocks_st;

VITAL_DATA_t VitalData2MIE_st;
VITAL_DATA_SHARING_t SiemensMain_Relays_st,Siemens_OtherRelays_st,TempSiemensRx_OtherRelays_st;

uint32_t Prev_RxTimeStampSiemens = 0;
uint16_t RxTimeStampNotUpdatedCount = 0;
uint32_t SelfTimeStamp = 0;

uint8_t Signals[70][40] = {{0,},};
uint16_t TotalSigCount = 0;

static uint8_t Commands_List[20][50] = {{0,},};
static uint8_t Commands_Count = 0;

uint8_t SendReply = 0;


void Process_SiemensEI(void)
{
	while(1)
	{
		MX_LWIP_Process();

		HAL_IWDG_Refresh(&hiwdg);

		Feed_wdt();

		readCanToStructs();

		Chech_forSiemensCMD();

		ProcessSendingSignals2MIE();

		ProcessSelfSharing();

		Siemens_Data_Analyser();

		ProcessDetectNoValidDataReceive();

		SendRelays2Other();

		CheckRelayMisMatch();

		Detect_SiemensCAN_Xchange_Failure();

		Print_BuildDat_Details();

		GetTimeStamp();

		CheckEIPC_DAT_CRC();

		if(Dats_Request_Flag > 0)
		{
			gucFlagUpdate = 3;
			break;
		}

	}

	while(1)
	{
		Data_Processing();
		HAL_IWDG_Refresh(&hiwdg);
	}
}

void Siemens_Data_Analyser(void)
{

	uint8_t Data_Traverser =0;
	uint16_t Cal_CRC=0,Cal_CRC_Compliment=0;
	uint16_t Recv_CRC=0,Recv_CRC_Compliment=0;
	uint16_t Recv_CRC_Loc=0,Recv_CRC_Compliment_Loc = 0,Recv_CRC_Compliment_Size=0;
	uint16_t Compliment_Data_Start_Loc = 0;
	uint16_t App_data_Traverser = 0;
	uint16_t VitalCRC = 0;
	uint16_t App_data_Size = 0;
	uint16_t TempApp_data_Size = 0;
	uint8_t App_Data[SIEMENS_MAX_RELAY_SIZE];
	SIEMENS_PKT_t Temp_Siemens_RX_st;
	uint8_t Buff[MAX_SIEMENS_PKT_LENGTH] = {0,};
	uint16_t Size = 0;
	uint8_t RejectPacketFlag = 0;


	if(Ethernet_Circle_buff_st.Write_end > 10)
	{
		memcpy(&Buff,Ethernet_Circle_buff_st.Ethernet_Buffers,Ethernet_Circle_buff_st.Write_end);
		Size = Ethernet_Circle_buff_st.Write_end;
		memset(&Ethernet_Circle_buff_st,0,sizeof(ETHERNET_Circle_t));
	}
	else
	{
		return;
	}

	memset(&Temp_Siemens_RX_st,0,sizeof(SIEMENS_PKT_t));


	BITSTUFFINIT(0,0);

	Recv_CRC_Loc = ((Size/2)-2);
	Recv_CRC_Compliment_Size = ((Size/2)-2);
	Recv_CRC_Compliment_Loc = ((Size)-2);
	Compliment_Data_Start_Loc = (Size/2);


	if(  Data_Compliment(Buff,Size,VERIFY)  != 1)
	{
		printf("Compliment Data Not Verified\n");
		return ;
	}

	Recv_CRC = ExtractBits(0,16,&Buff[Recv_CRC_Loc]);
	Cal_CRC = Siemens_CRC(&Buff[0],Recv_CRC_Loc);

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_PACKET_CRC_SIEMENS,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_PACKET_CRC_SIEMENS\n");
		Recv_CRC = HAL_RNG_GetRandomNumber(&hrng) % 0xffff;
		if(Recv_CRC == Cal_CRC)
		{
			Recv_CRC += 4;
		}
	}
#endif

	if(Cal_CRC != Recv_CRC)
	{
		printf("1st CRC Fail\n");
		printf("Calculated:0x%04x\n",Cal_CRC);
		printf("Received:0x%04x\n",Recv_CRC);
		return;
	}

	Recv_CRC_Compliment = ExtractBits(0,16,&Buff[Recv_CRC_Compliment_Loc]);
	Cal_CRC_Compliment = Siemens_CRC(&Buff[Compliment_Data_Start_Loc],Recv_CRC_Compliment_Size);

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_COMP_PACKET_CRC_SIEMENS,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_COMP_PACKET_CRC_SIEMENS\n");
		Recv_CRC_Compliment = HAL_RNG_GetRandomNumber(&hrng) % 0xffff;
		if(Recv_CRC_Compliment == Cal_CRC_Compliment)
		{
			Recv_CRC_Compliment += 4;
		}
	}
#endif

	if(Cal_CRC_Compliment != Recv_CRC_Compliment)
	{
		printf("2nd CRC Fail\n");
		printf("Cal_CRC_Compliment:0x%04x\n",Cal_CRC_Compliment);
		printf("Recv_CRC_Compliment:0x%04x\n",Recv_CRC_Compliment);
		return;
	}

	Temp_Siemens_RX_st.DesSysType = Extract_Parity_Data(ucBit,8,&Buff[ucByte],ODD_PARITY);

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_DES_SYS_TYPE,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_DES_SYS_TYPE\n");
		Temp_Siemens_RX_st.DesSysType = HAL_RNG_GetRandomNumber(&hrng) % 0xff;
		if(Temp_Siemens_RX_st.DesSysType == EI_data.Siemens_st.DestSysType)
		{
			Temp_Siemens_RX_st.DesSysType += 4;
		}
	}
#endif

	if(Temp_Siemens_RX_st.DesSysType != EI_data.Siemens_st.DestSysType)
	{
		printf("Destination System Type Not Matched\n");
		printf("Received:%u\n",Temp_Siemens_RX_st.DesSysType);
		printf("Expected:%u\n",EI_data.Siemens_st.DestSysType);
		RejectPacketFlag = 1;
	}
	//	printf("Des_type:%u\n",Temp_Siemens_RX_st.dst_sys_typ);
	BITSTUFFINC(8);//Destination System Type

	Temp_Siemens_RX_st.DestAddr = Extract_Parity_Data(ucBit,24,&Buff[ucByte],EVEN_PARITY);

	if(Temp_Siemens_RX_st.DestAddr != -1)
	{
		uint8_t ExtractBuffer[3] = {0,};
		ExtractAppData(&Buff[ucByte], 3, ExtractBuffer);
		memcpy(&Temp_Siemens_RX_st.ExtDestAddr,&ExtractBuffer, 3);
	}
	else
	{
		RejectPacketFlag = 1;
	}


	//#if EN_SIEMENS_FAULTINJECTION
	//	if(  (CHECKFLAG(INVALID_DES,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	//	{
	//		printf("Injecting INVALID_DES_SYS_TYPE\n");
	//		Temp_Siemens_RX_st.DesSysType = HAL_RNG_GetRandomNumber(&hrng) % 0xff;
	//		if(Temp_Siemens_RX_st.DesSysType == EI_data.Siemens_st.DestSysType)
	//		{
	//			Temp_Siemens_RX_st.DesSysType += 4;
	//		}
	//	}
	//#endif

	if(Temp_Siemens_RX_st.ExtDestAddr != EI_data.Siemens_st.DestAddress)
	{
		printf("Destination Address Not Matched\n");
		printf("Received:%lu\n",Temp_Siemens_RX_st.DestAddr);
		printf("Expected:%lu\n",EI_data.Siemens_st.DestAddress);
		RejectPacketFlag = 1;
	}
	//	printf("dst_add:%lu\n",Temp_Siemens_RX_st.dst_add);
	BITSTUFFINC(24);//Destination Address

	Temp_Siemens_RX_st.SrcSysType = Extract_Parity_Data(ucBit,8,&Buff[ucByte],ODD_PARITY);

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_SRC_SYS_TYPE,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_SRC_SYS_TYPE\n");
		Temp_Siemens_RX_st.SrcSysType = HAL_RNG_GetRandomNumber(&hrng) % 0xff;
		if(Temp_Siemens_RX_st.SrcSysType == EI_data.Siemens_st.SourceSysType)
		{
			Temp_Siemens_RX_st.SrcSysType += 4;
		}
	}
#endif

	if(Temp_Siemens_RX_st.SrcSysType != EI_data.Siemens_st.SourceSysType)
	{
		printf("Source System Type Not Matched\n");
		printf("Received:%u\n",Temp_Siemens_RX_st.SrcSysType);
		printf("Expected:%u\n",EI_data.Siemens_st.SourceSysType);
		RejectPacketFlag = 1;
	}
	//	printf("Src_sys_type:%u\n",Temp_Siemens_RX_st.Src_sys_type);
	BITSTUFFINC(8);//Source System Type

	Temp_Siemens_RX_st.SourceAddr = Extract_Parity_Data(ucBit,24,&Buff[ucByte],EVEN_PARITY);

	if(Temp_Siemens_RX_st.SourceAddr != -1)
	{
		uint8_t ExtractBuffer[3] = {0,};
		ExtractAppData(&Buff[ucByte], 3, ExtractBuffer);
		memcpy(&Temp_Siemens_RX_st.ExtSourceAddr,&ExtractBuffer, 3);
	}

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_SRC_ADDR,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_SRC_ADDR\n");
		Temp_Siemens_RX_st.SourceAddr = HAL_RNG_GetRandomNumber(&hrng) % 0xffffff;
		if(Temp_Siemens_RX_st.ExtSourceAddr == EI_data.Siemens_st.SourceAddress)
		{
			Temp_Siemens_RX_st.ExtSourceAddr += 4;
		}
	}
#endif


	if(Temp_Siemens_RX_st.ExtSourceAddr != EI_data.Siemens_st.SourceAddress)
	{
		printf("Source Address Not Matched\n");
		printf("Received:%lu\n",Temp_Siemens_RX_st.ExtSourceAddr);
		printf("Expected:%lu\n",EI_data.Siemens_st.SourceAddress);
		RejectPacketFlag = 1;
	}
	//	printf("src_add:%lu\n",Temp_Siemens_RX_st.src_add);
	BITSTUFFINC(24);//Source Address

	Temp_Siemens_RX_st.ProdDataVer = Extract_Parity_Data(ucBit,8,&Buff[ucByte],EVEN_PARITY);
	//	printf("prod_data_ver:%u\n",Temp_Siemens_RX_st.prod_data_ver);
	BITSTUFFINC(8);//Product Data Version

	Temp_Siemens_RX_st.CompIndex = Extract_Parity_Data(ucBit,8,&Buff[ucByte],ODD_PARITY);
	//	printf("comp_index:%u\n",Temp_Siemens_RX_st.comp_index);
	BITSTUFFINC(8);//Compatability Index

	Temp_Siemens_RX_st.DestPort = Extract_Parity_Data(ucBit,8,&Buff[ucByte],NO_PARITY);

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_DES_PORT,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_DES_PORT\n");
		Temp_Siemens_RX_st.DestPort = HAL_RNG_GetRandomNumber(&hrng) % 0xff;
		if(Temp_Siemens_RX_st.DestPort == EI_data.Siemens_st.DestPort)
		{
			Temp_Siemens_RX_st.DestPort += 4;
		}
	}
#endif

	if(Temp_Siemens_RX_st.DestPort != EI_data.Siemens_st.DestPort)
	{
		printf("Destination Port Not Matched\n");
		printf("Received:%u\n",Temp_Siemens_RX_st.DestPort);
		printf("Expected:%u\n",EI_data.Siemens_st.DestPort);
		RejectPacketFlag = 1;
	}
	//	printf("dst_port:%u\n",Temp_Siemens_RX_st.dst_port);
	BITSTUFFINC(8); //Dest Port.

	Temp_Siemens_RX_st.SourcePort = Extract_Parity_Data(ucBit,8,&Buff[ucByte],NO_PARITY);

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_SRC_PORT,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting INVALID_SRC_PORT\n");
		Temp_Siemens_RX_st.SourcePort = HAL_RNG_GetRandomNumber(&hrng) % 0xff;
		if(Temp_Siemens_RX_st.SourcePort == EI_data.Siemens_st.SourcePort)
		{
			Temp_Siemens_RX_st.SourcePort += 4;
		}
	}
#endif

	if(Temp_Siemens_RX_st.SourcePort != EI_data.Siemens_st.SourcePort)
	{
		printf("Source Port Not Matched\n");
		printf("Received:%u\n",Temp_Siemens_RX_st.SourcePort);
		printf("Expected:%u\n",EI_data.Siemens_st.SourcePort);
		RejectPacketFlag = 1;
	}
	//	printf("src_port:%u\n",Temp_Siemens_RX_st.src_port);
	BITSTUFFINC(8); // Source Port.


	Temp_Siemens_RX_st.RxTimeStamp = Extract_Parity_Data(ucBit,32,&Buff[ucByte],NO_PARITY);
	//	printf("Rx_time_stamp:%lu\n",Temp_Siemens_RX_st.Rx_time_stamp);
	BITSTUFFINC(32);//RX_TIME STAMP


	Temp_Siemens_RX_st.TxTimeStamp = Extract_Parity_Data(ucBit,32,&Buff[ucByte],NO_PARITY);
	//	printf("Tx_time_stamp:%lu\n",Temp_Siemens_RX_st.Tx_time_stamp);
	BITSTUFFINC(32);//TX_TIME STAMP

	//TO DO WRITE DATA EXTRACTION
	App_data_Size = 0;
	printf("\n");
	for(Data_Traverser = ucByte; Data_Traverser <Recv_CRC_Loc; Data_Traverser++)
	{
		App_Data[App_data_Traverser++] = Extract_Parity_Data(ucBit,8,&Buff[Data_Traverser],EVEN_PARITY);
		//printf("D[%02u]:%02x|",App_data_Size,App_Data[App_data_Size]);
		//Temp_Siemens_RX_st.app_data[App_data_Traverser++] = Extract_Parity_Data(ucBit,8,&Buff[Data_Traverser],EVEN_PARITY);
		//Extract the Received application data into buffer and checking parity and Removing Parity.
		++App_data_Size; //This variable is to know the Latest Size of Received Application Data
		BITSTUFFINC(8);
	}
	printf("\n");


	//	printf("App_data_Size:%u\n",App_data_Size);
	Temp_Siemens_RX_st.No_ofDataBytes = ExtractAppData(App_Data, App_data_Size, Temp_Siemens_RX_st.AppData);

	//	Temp_Siemens_RX_st.No_ofDataBytes = App_data_Size;

	TempApp_data_Size  = EI_data.Siemens_st.Booloutputs % 8;
	if(TempApp_data_Size!= 0)
	{
		TempApp_data_Size  = (EI_data.Siemens_st.Booloutputs / 8) + 1;
	}
	else
	{
		TempApp_data_Size  = (EI_data.Siemens_st.Booloutputs/8);
	}


	if(Temp_Siemens_RX_st.No_ofDataBytes != TempApp_data_Size)
	{
		printf("Application Data Not Matched\n");
		printf("Received:%u\n",Temp_Siemens_RX_st.No_ofDataBytes);
		printf("Expected:%u\n",TempApp_data_Size);

		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0)
		{
			RejectPacketFlag = 1;
		}
		if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
		{
			EI_data.Siemens_st.Booloutputs = (Temp_Siemens_RX_st.No_ofDataBytes * 8);
		}
	}

	if(RejectPacketFlag == 1)
	{
		return;
	}

	VitalCRC = GetCRC16(0, Temp_Siemens_RX_st.AppData, App_data_Size); /* Calculating Application Data CRC */

	Temp_Siemens_RX_st.crc = ExtractBits(0,16,&Buff[ucByte]); /* Extraction of Packet CRC */






	/* if Everything is Okay copy to Main Rx Structure */
	memset(&Siemens_RX_st,0,sizeof(SIEMENS_PKT_t));
	memcpy(&Siemens_RX_st,&Temp_Siemens_RX_st,sizeof(SIEMENS_PKT_t));

	/* Send Reply to Received Packet */
	Form_Siemens_Reply();







	//	if( (RxTimeStampNotUpdatedCount <= EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount)  ) /* Consider the Packet If our Time stamp is Updating only (or) up to Certain count */
	if( abs(RxTimeStampNotUpdatedCount - EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount) <= EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount)
	{
		if(Prev_RxTimeStampSiemens != Temp_Siemens_RX_st.RxTimeStamp)
		{
			/* Update Clocks if Any */
			//			printf("Updating...\n");
			SiemensClocks_st.LastReceiveClk = HAL_GetTick();



			Prev_RxTimeStampSiemens = Temp_Siemens_RX_st.RxTimeStamp;
			RxTimeStampNotUpdatedCount = 0;
			//			printf("Siemens RxTS Updating:%u\n",RxTimeStampNotUpdatedCount);
			if(SiemensMain_Relays_st.VitalCRC != VitalCRC)
			{
				memset(&SiemensMain_Relays_st,0,sizeof(VITAL_DATA_SHARING_t));
				memset(&VitalData2MIE_st,0,sizeof(VITAL_DATA_t));

				SiemensMain_Relays_st.VitalCRC = VitalCRC;
				SiemensMain_Relays_st.NoofDataBytes = Siemens_RX_st.No_ofDataBytes;
				memcpy(&SiemensMain_Relays_st.Relays,&Siemens_RX_st.AppData,Siemens_RX_st.No_ofDataBytes);

				VitalData2MIE_st.NoofDataBytes = Siemens_RX_st.No_ofDataBytes;
				memcpy(&VitalData2MIE_st.Relays,&Siemens_RX_st.AppData,Siemens_RX_st.No_ofDataBytes);

				if(SiemensClocks_st.MisMatchFlag == RESET)
				{
#ifdef EI_1
					TransmitCanPkt(EI_PROCESS_DATA);
#endif
				}

#ifdef EI_1
				TransmitCanPkt(EIPC1_RELAYS_2_OTHER);

#endif
#ifdef EI_2
				TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif

				SiemensClocks_st.Relays2OtheClk = HAL_GetTick();

			}
		}
		else
		{
			RxTimeStampNotUpdatedCount++;
			//			printf("Siemens RxTS not Updating:%u\n",RxTimeStampNotUpdatedCount);

			if(RxTimeStampNotUpdatedCount >  EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount)
			{
				printf("Siemens RxTS not Updating:%u\n",RxTimeStampNotUpdatedCount);
				RxTimeStampNotUpdatedCount = EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount + 1; /* To Avoid Overflow */
			}
		}
	}


	Print_Siemens_MSG(&Temp_Siemens_RX_st,Buff,Size,RX);
	if(Siemens_tx_Buffer_st.Tx_Length > 0)
	{
		Print_Siemens_MSG(&Temp_SiemensTx_st,Siemens_tx_Buffer_st.Tx_Buff,Siemens_tx_Buffer_st.Tx_Length,TX);
		memset(&Siemens_tx_Buffer_st,0,sizeof(SIEMENS_TX_PKT_t));
	}

	Tx_Siemens_self_sharing_st.RxTimeStampNotUpdatedCount = RxTimeStampNotUpdatedCount;
	Tx_Siemens_self_sharing_st.DataReceving = RECEIVING;
	Tx_Siemens_self_sharing_st.MesgReceived = RECEIVED;

#ifdef EI_1
	TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
	TransmitCanPkt(EI_2_SELF_SHARING);
#endif
	SiemensClocks_st.SelfSharingClk = HAL_GetTick();

	return;
}

uint16_t ExtractAppData(uint8_t *MainBuffer, uint16_t MainBufferLength, uint8_t *ExtractBuffer)
{
	uint16_t ExtractBufferByteIndex = 0;
	uint8_t ExtractBufferBitIndex = 0;
	uint16_t TotalMainBits = (MainBufferLength * 8);

	for (uint16_t Traverser = 0; Traverser < TotalMainBits; ++Traverser)
	{
		int Temp = (Traverser / 8);
		int BitJump = (Traverser % 8);
		if (BitJump == 7)
		{
			continue;
		}
		if ((MainBuffer[Temp] >> (Traverser % 8)) & 1)
		{
			SET_BIT_USER(ExtractBufferBitIndex, ExtractBuffer[ExtractBufferByteIndex]);
		}
		++ExtractBufferBitIndex;
		if (ExtractBufferBitIndex == 8)
		{
			ExtractBufferBitIndex = 0;
			++ExtractBufferByteIndex;
		}
	}
	return (ExtractBufferByteIndex);
}

void Form_Siemens_Reply(void)
{

	uint8_t Buffer[REPLY_MAX_SIZE] = {0,};
	uint16_t App_data_Traverser = 0;
	uint16_t Calc_CRC =0;
	uint16_t Calc_Compliment_CRC = 0;
	uint16_t CRC_Compliment_Loc  = 0;
	BITSTUFFINIT(0,0);


	//	printf("\nSending Packet Data:\n");
	memset(&Siemens_TX_st,0,sizeof(SIEMENS_PKT_t));


	Siemens_TX_st.DesSysType = Siemens_RX_st.SrcSysType; //Destination System_Type
	Insert_cal_parity(ucBit,8,&Buffer[ucByte],Siemens_TX_st.DesSysType,ODD_PARITY);
	//	printf("Dst_type:%u\n",Buffer[ucByte]);
	BITSTUFFINC(8);

	Siemens_TX_st.DestAddr = Siemens_RX_st.SourceAddr; //Destination Address
	Siemens_TX_st.ExtDestAddr = Siemens_RX_st.ExtSourceAddr;
	Insert_cal_parity(ucBit,24,&Buffer[ucByte],Siemens_TX_st.DestAddr,EVEN_PARITY);
	//	printf("\nSiemens_TX_st.DestAddr = %lu\n",Siemens_TX_st.DestAddr);
	BITSTUFFINC(24);

	Siemens_TX_st.SrcSysType = Siemens_RX_st.DesSysType; //Destination Address
	Insert_cal_parity(ucBit,8,&Buffer[ucByte],Siemens_TX_st.SrcSysType,ODD_PARITY);
	//	printf("Src_sys_type:%u\n",Buffer[ucByte]);
	BITSTUFFINC(8);

	Siemens_TX_st.SourceAddr = Siemens_RX_st.DestAddr;
	Siemens_TX_st.ExtSourceAddr = Siemens_RX_st.ExtDestAddr;
	Insert_cal_parity(ucBit,24,&Buffer[ucByte],Siemens_TX_st.SourceAddr,EVEN_PARITY);
	//	printf("\nSiemens_TX_st.SourceAddr = %lu\n",Siemens_TX_st.SourceAddr);
	BITSTUFFINC(24);

	//	Siemens_TX_st.ProdDataVer = PROD_VERSION; //Self Product Version
	Siemens_TX_st.ProdDataVer = Siemens_RX_st.ProdDataVer; //Self Product Version
	Insert_cal_parity(ucBit,8,&Buffer[ucByte],Siemens_TX_st.ProdDataVer,EVEN_PARITY);
	//	printf("prod_data_ver:%u\n",Buffer[ucByte]);
	BITSTUFFINC(8);

	Siemens_TX_st.CompIndex = COMP_INDEX; //Self Compatability Index
	Insert_cal_parity(ucBit,8,&Buffer[ucByte],Siemens_TX_st.CompIndex,ODD_PARITY);
	//	printf("comp_index:%u\n",Buffer[ucByte]);
	BITSTUFFINC(8);


	Siemens_TX_st.DestPort = Siemens_RX_st.SourcePort; //Destination Port
	Insert_cal_parity(ucBit,8,&Buffer[ucByte],Siemens_TX_st.DestPort,NO_PARITY);
	//	printf("dst_port:%u\n",Buffer[ucByte]);
	BITSTUFFINC(8);

	Siemens_TX_st.SourcePort = Siemens_RX_st.DestPort; //Source Port
	Insert_cal_parity(ucBit,8,&Buffer[ucByte],Siemens_TX_st.SourcePort,NO_PARITY);
	//	printf("src_port:%u\n",Buffer[ucByte]);
	BITSTUFFINC(8);

	Siemens_TX_st.RxTimeStamp = Siemens_RX_st.TxTimeStamp; //Source Port
	Insert_cal_parity(ucBit,32,&Buffer[ucByte],Siemens_TX_st.RxTimeStamp,NO_PARITY);
	//	printf("Rx_time_stamp:%lu\n",Siemens_TX_st.Rx_time_stamp);
	BITSTUFFINC(32);

	Siemens_TX_st.TxTimeStamp = SelfTimeStamp;//(uint32_t)(HAL_GetTick()/100);//Temp_Siemens_RX_st.Tx_time_stamp; //Source Port
	Insert_cal_parity(ucBit,32,&Buffer[ucByte],Siemens_TX_st.TxTimeStamp,NO_PARITY);
	//	printf("Tx_time_stamp:%lu\n",Siemens_TX_st.Tx_time_stamp);
	BITSTUFFINC(32);

	CRC_Compliment_Loc = Data_Compliment(Buffer,ucByte,CONVERT); //ucByte = 30;

	Calc_CRC = Siemens_CRC(Buffer,ucByte); //30


#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_PACKET_CRC_EIPC,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		uint16_t Temp_Crc = Calc_CRC;
		printf("Injecting INVALID_PACKET_CRC_SIEMENS\n");
		printf("Actual CRC:%04x\n",Calc_CRC);

		Calc_CRC = HAL_RNG_GetRandomNumber(&hrng) % 0xffff;
		if(Calc_CRC == Temp_Crc)
		{
			Calc_CRC += 4;
		}
		printf("Injected CRC:%04x\n",Calc_CRC);
	}
#endif




	//	printf("Cal_CRC:0x%04x\n",Calc_CRC);
	Insert_cal_parity(ucBit,16,&Buffer[ucByte],Calc_CRC,NO_PARITY);
	BITSTUFFINC(16);

	Calc_Compliment_CRC = Siemens_CRC(&Buffer[ucByte],ucByte-2);//32,30
#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(INVALID_COMP_PACKET_CRC_EIPC,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		uint16_t Temp_Crc = Calc_Compliment_CRC;
		printf("Injecting INVALID_PACKET_CRC_SIEMENS\n");
		printf("Actual Compliment CRC:%04x\n",Calc_Compliment_CRC);
		Calc_Compliment_CRC = HAL_RNG_GetRandomNumber(&hrng) % 0xffff;
		if(Calc_Compliment_CRC == Temp_Crc)
		{
			Calc_Compliment_CRC += 4;
		}
		printf("Injected Compliment CRC:%04x\n",Calc_Compliment_CRC);
	}
#endif
	//	printf("Calc_Compliment_CRC:0x%04x\n",Calc_Compliment_CRC);
	ucByte+=ucByte;
	Insert_cal_parity(ucBit,16,&Buffer[CRC_Compliment_Loc],Calc_Compliment_CRC,NO_PARITY);
	BITSTUFFINC(16);


	memcpy(Siemens_tx_Buffer_st.Tx_Buff,Buffer,CRC_Compliment_Loc+2);
	Siemens_tx_Buffer_st.Tx_Length = (CRC_Compliment_Loc+2);

	CheckToSendReplyorNot();

#if EN_SIEMENS_FAULTINJECTION
	if(  (CHECKFLAG(NO_REPLY_FROM_EIPC,Siemens_fault_injection_st.FAULT_INJECTION_BITS)) == SET)
	{
		printf("Injecting NO REPLY FROM EIPC Failure\n");
	}
	else
	{
		if(SendReply == 1)
		{
			udpClient_send(Buffer,CRC_Compliment_Loc+2,Source_IP);
		}
	}
#else
	if(SendReply == 1)
	{
		udpClient_send(Buffer,CRC_Compliment_Loc+2,Source_IP);
	}
#endif

	memset(&Temp_SiemensTx_st,0,sizeof(SIEMENS_PKT_t));
	memcpy(&Temp_SiemensTx_st,&Siemens_TX_st,sizeof(SIEMENS_PKT_t));
	UNUSED(App_data_Traverser);
}

void ProcessSendingSignals2MIE(void)
{
	if(HAL_Elaps_Ticks(SiemensClocks_st.Signal2MIE_Clk) > EI_data.Siemens_st.MIE_SIGNAL_SEND_CLK)
	{
		if(
				(SiemensClocks_st.MisMatchFlag == RESET) &&
				(SiemensClocks_st.SiemensCanXchangeHappening == YES) &&
				( (Rx_Siemens_self_sharing_st.DataReceving == RECEIVING) && (Tx_Siemens_self_sharing_st.DataReceving == RECEIVING) )
		)
		{
			/*
			 * CAN Exchange Happening.
			 * No MisMatch Found
			 */
			//			printf("\n1\n");
			memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);
		}
		else if( ((Rx_Siemens_self_sharing_st.DataReceving == RECEIVING) && (Tx_Siemens_self_sharing_st.DataReceving == NOT_RECEIVING)) && (SiemensClocks_st.SiemensCanXchangeHappening == YES) )
		{
			/*
			 * CAN Exchange Happening
			 * MisMatch Found because of Other Card is Receiving,
			 * This Card is Not Receiving
			 */
			//			printf("\n2\n");
			memcpy(&VitalData2MIE_st.Relays,&Siemens_OtherRelays_st.Relays,Siemens_OtherRelays_st.NoofDataBytes);
		}
		else if( ((Tx_Siemens_self_sharing_st.DataReceving == RECEIVING) && (Rx_Siemens_self_sharing_st.DataReceving == NOT_RECEIVING)) && (SiemensClocks_st.SiemensCanXchangeHappening == YES) )
		{
			/*
			 * CAN Exchange Happening
			 * MisMatch Found because of This Card is Receiving,
			 * Other Card is Not Receiving
			 */
			//			printf("\n4\n");
			memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);
		}
		else
		{
			//			printf("\n5\n");
			memset(&VitalData2MIE_st.Relays,0,sizeof(VitalData2MIE_st.Relays));
		}


		if((Tx_Siemens_self_sharing_st.RxTimeStampNotUpdatedCount >= EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount) && (Rx_Siemens_self_sharing_st.RxTimeStampNotUpdatedCount >= EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount) )
		{
			//			printf("\n6\n");
			memset(&VitalData2MIE_st.Relays,0,sizeof(VitalData2MIE_st.Relays));
		}

		VitalData2MIE_st.NoofDataBytes = EI_data.Siemens_st.Booloutputs  % 8;

		if(VitalData2MIE_st.NoofDataBytes != 0)
		{
			VitalData2MIE_st.NoofDataBytes  = (EI_data.Siemens_st.Booloutputs / 8) + 1;
		}
		else
		{
			VitalData2MIE_st.NoofDataBytes = (EI_data.Siemens_st.Booloutputs / 8);
		}

		if(((strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") != 0)) && (SiemensClocks_st.SiemensCanXchangeHappening == NO))
		{
			TransmitCanPkt(EI_PROCESS_DATA);
		}
#ifdef EI_1
		else
		{
			if(strcmp((char *)EI_data.STN_NAME,(char *)"LAB_TESTING") == 0)
			{
				memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);
			}
			TransmitCanPkt(EI_PROCESS_DATA);
		}
#endif




#ifdef EI_1
		printf("\nSendingRelaysToMIE:");
		for(uint16_t Traverser = 0;Traverser<VitalData2MIE_st.NoofDataBytes;++Traverser)
		{
			printf("%02X ",VitalData2MIE_st.Relays[Traverser]);
		}
		printf("\n");

		//		for(uint16_t Traverser = 0;Traverser<(VitalData2MIE_st.NoofDataBytes * 8);++Traverser)
		//		{
		//			if(GETPIN(Traverser) > 0)
		//			{
		//				printf("%u:SET\n",Traverser);
		//			}
		//			else
		//			{
		//				printf("%u:CLEAR\n",Traverser);
		//			}
		//		}
		//		printf("\n____________________________________ \n");




#endif
		SiemensClocks_st.Signal2MIE_Clk = HAL_GetTick();
	}
}


void ProcessSelfSharing(void)
{
	if( HAL_Elaps_Ticks(SiemensClocks_st.SelfSharingClk) > EI_data.Siemens_st.CAN_Xchange_Time)
	{
		Tx_Siemens_self_sharing_st.MesgReceived = NOT_RECEIVED;

#ifdef EI_1
		TransmitCanPkt(EI_1_SELF_SHARING);
#endif
#ifdef EI_2
		TransmitCanPkt(EI_2_SELF_SHARING);
#endif

		SiemensClocks_st.SelfSharingClk = HAL_GetTick();
	}
}

void ProcessDetectNoValidDataReceive(void)
{

	if((SiemensClocks_st.LastReceiveClk != 0) && (HAL_Elaps_Ticks(SiemensClocks_st.LastReceiveClk) > EI_data.Siemens_st.COMM_FAILURE_TIME_OUT) )
	{
		if(Tx_Siemens_self_sharing_st.DataReceving != NOT_RECEIVED)
		{
			printf("This Card Not Receiving Data\n");
			Tx_Siemens_self_sharing_st.DataReceving = NOT_RECEIVED;
		}
	}

	if((Tx_Siemens_self_sharing_st.RxTimeStampNotUpdatedCount > EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount) && (Rx_Siemens_self_sharing_st.RxTimeStampNotUpdatedCount > EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount) )
	{
		if(Tx_Siemens_self_sharing_st.DataReceving != NOT_RECEIVED)
		{
			printf("This Card Not Valid Receiving Data\n");
			Tx_Siemens_self_sharing_st.DataReceving = NOT_RECEIVED;
		}
	}
}

void SendRelays2Other(void)
{
	if( HAL_Elaps_Ticks(SiemensClocks_st.Relays2OtheClk) > ONE_SEC)
	{

#ifdef EI_1
		TransmitCanPkt(EIPC1_RELAYS_2_OTHER);
#endif
#ifdef EI_2
		TransmitCanPkt(EIPC2_RELAYS_2_OTHER);
#endif
		SiemensClocks_st.Relays2OtheClk = HAL_GetTick();

	}
}

void CheckRelayMisMatch(void)
{
	if(HAL_Elaps_Ticks(SiemensClocks_st.RelayMisMatchCheckClk) > FIVE_HUNDRED_MS)
	{
		if( (Tx_Siemens_self_sharing_st.DataReceving == NOT_RECEIVING) && (Rx_Siemens_self_sharing_st.DataReceving == RECEIVING) )
		{
			memcpy(&SiemensMain_Relays_st,&Siemens_OtherRelays_st,sizeof(VITAL_DATA_SHARING_t));
		}
		if(Siemens_OtherRelays_st.VitalCRC != SiemensMain_Relays_st.VitalCRC)
		{
			SiemensClocks_st.MisMatchFlag = SET;
		}
		else
		{
			SiemensClocks_st.MisMatchFlag = RESET;
		}

		SiemensClocks_st.RelayMisMatchCheckClk = HAL_GetTick();
	}
}

void Detect_SiemensCAN_Xchange_Failure(void)
{
	static uint32_t Previous_Clock_RCV_SELF_XCHG_CLK = 0;

	if(SiemensClocks_st.SiemensCanXchangeHappening == YES)
	{
		Blink_S2();
	}

	if(HAL_Elaps_Ticks(SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK)>= EI_data.Siemens_st.CAN_EXCHANGE_TIME_OUT)
	{
		if(Previous_Clock_RCV_SELF_XCHG_CLK != SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK)
		{
			SiemensClocks_st.SiemensCanXchangeHappening = NO;
			Previous_Clock_RCV_SELF_XCHG_CLK = SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK;
			printf("CAN Data Not Received from last %lu Seconds\n",(HAL_Elaps_Ticks(SiemensClocks_st.SiemensRCV_SELF_XCHG_CLK)/1000));
		}
	}
}

void Print_Siemens_MSG(SIEMENS_PKT_t *SiemensPrint_st,uint8_t *Buff,uint16_t Length,uint8_t TX_RX)
{

	//	printf("\n________________________________________________________________________________________________\n");
	printf("\nTX_RX:");
	if(TX_RX == TX)
	{
		printf("TX|");
	}
	else if(TX_RX == RX)
	{
		printf("RX|");
	}
	printf("DT:%u|",SiemensPrint_st->DesSysType);
	printf("DA:%lu|",SiemensPrint_st->ExtDestAddr);
	printf("ST:%u|",SiemensPrint_st->SrcSysType);
	printf("SA:%lu|",SiemensPrint_st->ExtSourceAddr);
	printf("PD_V:%u|",SiemensPrint_st->ProdDataVer);
	printf("CI:%u|",SiemensPrint_st->CompIndex);
	printf("DP:%u|",SiemensPrint_st->DestPort);
	printf("SP:%u|",SiemensPrint_st->SourcePort);
	printf("RX_TS:%lu|",SiemensPrint_st->RxTimeStamp);
	printf("TX_TS:%lu\n",SiemensPrint_st->TxTimeStamp);
	if(TX_RX == RX)
	{
		printf("RC:%u|",SiemensPrint_st->No_ofDataBytes);
		for(uint16_t Traverser = 0;Traverser <SiemensPrint_st->No_ofDataBytes;++Traverser)
		{
			printf("%02X ",SiemensPrint_st->AppData[Traverser]);
		}
		printf("\n");

		if(Debug_flag == 2)
		{
			PrintSignals();
		}

		//		memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);
		//
		//		for(uint16_t Traverser = 0;Traverser<(VitalData2MIE_st.NoofDataBytes * 8);++Traverser)
		//		{
		//			if(GETPIN(Traverser+1) > 0)
		//			{
		//				printf("%u:SET\n",(Traverser+1));
		//			}
		//			else
		//			{
		//				printf("%u:CLEAR\n",(Traverser+1));
		//			}
		//		}
		//		printf("\n____________________________________ \n");


	}
	if(Debug_flag == 1)
	{
		for(uint16_t Byte = 0;Byte<Length;++Byte)
		{
			if((Byte != 0) && ( (Byte % 32) == 0) )
			{
				printf("\n");
			}
			printf("%02X ",Buff[Byte]);
		}
		printf("\n");
	}
	//	printf("\n");
	//	printf("\n________________________________________________________________________________________________\n");
}



void Chech_forSiemensCMD(void)
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
			switch(Is_SiemensCommand_Valid(Buffer,Index))
			{
			case ZERO:
				Dats_Request_Flag = 1;
				printf("\n*************PLEASE LOAD EI DATS USING APPLICATION************\n");
				break;
			case ONE:
				printf("Received \"%s\":%u\n",Commands_List[ONE],(Buffer[Index-1])-'0');
				if( (((Buffer[Index-1])-'0') >= 0) && ((Buffer[Index-1])-'0') < 3)
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
				printf("Received %s\n",Commands_List[TWO]);
				printf("INVALID_DES_SYS_TYPE\n");
				SET_BIT_USER(INVALID_DES_SYS_TYPE,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case THREE:
				printf("Received %s\n",Commands_List[THREE]);
				printf("INVALID_SRC_SYS_TYPE\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_SRC_SYS_TYPE,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case FOUR:
				printf("Received %s\n",Commands_List[FOUR]);
				printf("INVALID_SRC_ADDR\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_SRC_ADDR,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case FIVE:
				printf("Received %s\n",Commands_List[FIVE]);
				printf("INVALID_DES_PORT\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_DES_PORT,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;

			case SIX:
				printf("Received %s\n",Commands_List[SIX]);
				printf("INVALID_SRC_PORT\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_SRC_PORT,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case SEVEN:
				printf("Received %s\n",Commands_List[SEVEN]);
				printf("INVALID_PACKET_CRC_SIEMENS\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_PACKET_CRC_SIEMENS,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case EIGHT:
				printf("Received %s Command\n",Commands_List[EIGHT]);
				printf("NO_REPLY_FROM_EIPC\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(NO_REPLY_FROM_EIPC,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case NINE:
				printf("Received %s\n",Commands_List[NINE]);
				printf("INVALID_PACKET_CRC_EIPC\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_PACKET_CRC_EIPC,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case TEN:
				printf("Received %s\n",Commands_List[TEN]);
				printf("INVALID_COMP_PACKET_CRC_SIEMENS\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_COMP_PACKET_CRC_SIEMENS,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case ELEVEN:
				printf("Received %s\n",Commands_List[ELEVEN]);
				printf("INVALID_COMP_PACKET_CRC_EIPC\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				SET_BIT_USER(INVALID_COMP_PACKET_CRC_EIPC,Siemens_fault_injection_st.FAULT_INJECTION_BITS);
				IS_COMMAND_RECEIVED = YES;
				break;
			case TWELVE:
				printf("Received %s\n",Commands_List[TWELVE]);
				printf("CLEAR ALL\n");
				Siemens_fault_injection_st.FAULT_INJECTION_BITS = 0;
				IS_COMMAND_RECEIVED = YES;
				break;
			default:
				printf("Please Send Valid Command\n");
			}
		}

		if(IS_COMMAND_RECEIVED == YES)
		{
			TransmitCanPkt(CAN_FAULT_INJECTION);
		}
	}
}


void Update_SiemensCommands(void)
{
	uint8_t CmdCnt = 0;

	Commands_List[CmdCnt][0] = 0X24;
	Commands_List[CmdCnt][1] = 0X01;
	Commands_List[CmdCnt][2] = 0X30;
	Commands_List[CmdCnt][3] = 0X40;
	Commands_List[CmdCnt][4] = 0XC0;
	Commands_List[CmdCnt][5] = 0X4E;
	Commands_List[CmdCnt][6] = 0X26;
	++CmdCnt;

	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"DEBUG_MODE");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_DES_SYS_TYPE");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_SRC_SYS_TYPE");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_SRC_ADDR");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_DES_PORT");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_SRC_PORT");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INAVLID_PACKET_CRC_SIEMENS");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"NO_REPLY_FROM_EIPC");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_PACKET_CRC_EIPC");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_COMP_PACKET_CRC_SIEMENS");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"INVALID_COMP_PACKET_CRC_EIPC");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"CLEAR_ALL");
	strcpy( (char *)Commands_List[CmdCnt++], (const char *)"HELP");
	Commands_Count =  CmdCnt;
}

uint8_t Is_SiemensCommand_Valid(uint8_t *Buffer,uint8_t Length)
{
	for(uint8_t Traverser = 0; Traverser < Commands_Count; ++Traverser)
	{
		//		if(strstr((char *)Buffer,(char *)Commands_List[Traverser]) != NULL)
		if(memcmp(Buffer,Commands_List[Traverser],strlen((char *)Commands_List[Traverser])) == 0)
		{
			return Traverser;
		}
	}
	return 255;
}


/*
 * SIEMENS CRC & Data Extract FUNTIONS
 */

uint16_t Siemens_CRC(uint8_t *data, uint16_t len)
{
	uint16_t crc = 0xFFFF;
	uint16_t i=0;
	int j =0;
	uint8_t crc_le[2];

#if (SIEMENS_DEBUG==1)
	printf("for CRC:\n");
	for ( i = 0; i < len; i++)
	{
		printf("0x%02x|",data[i]);
	}
	printf("\n");
#endif

	for ( i = 0; i < len; i++)
	{
		crc ^= data[i];
		for ( j = 0; j < 8; j++)
		{
			if (crc & 0x0001)
			{
				crc = (crc >> 1) ^ 0x8408;
			}
			else
			{
				crc >>= 1;
			}
		}
	}

	crc_le[0] = crc & 0xFF;
	crc_le[1] = (crc >> 8) & 0xFF;
	//printf("Before Reverse 1:0x%02x|0x%02x\n",crc_le[0],crc_le[1]);



	crc = crc_le[0] << 8 | crc_le[1]; // reverse CRC bytes
	crc ^= 0xFFFF; // perform final XOR with 0xFFFF

	//printf("Before Reverse 2:0x%02x\n",crc);


	// crc = crc_le[1] << 8 | crc_le[0]; // reverse CRC bytes
	// crc ^= 0xFFFF; // perform final XOR with 0xFFFF
	return crc;
}


int32_t Extract_Parity_Data(uint8_t Start,uint8_t No_of_Bits,uint8_t *DataIn,uint8_t Parity_Type)
{
	uint8_t Temp_val = 0,t1,t2,t3;
	uint32_t Return_value = 0;

	if(Parity_Type != NO_PARITY)
	{
		if(Verify_parity(DataIn,(No_of_Bits/8),Parity_Type)==0)
		{
			printf("Parity Verfication Failed\n");
			return -1;
		}
		else
		{
			//printf("Parity Verfication Success\n");
		}
	}
	else
	{
		Return_value = ExtractBits(Start,No_of_Bits,DataIn);
		return Return_value;
	}



	if(No_of_Bits==8)
	{
		//printf("Data:0x%02x\n",DataIn[0]);
		Temp_val = DataIn[0];
		Temp_val &= ~(1<<7);
		return (Temp_val);
	}
	else if(No_of_Bits==24)
	{
		//printf("24 bit extraction\n");
		Temp_val = DataIn[0];
		//printf("Data:0x%02x\n",DataIn[0]);
		t1 = ((Temp_val>>0)&1); //copy of 0th bit
		t2 = ((Temp_val>>1)&1); //copy of 1st bit
		Temp_val &= ~(1<<7); //clearing 7th bit
		Temp_val>>=2; //Shift two times.
		//printf("Temp:0x%02x\n",Temp_val);
		Return_value = Temp_val;

		Temp_val = DataIn[1];
		//printf("Data:0x%02x\n",DataIn[1]);
		t3 = ((Temp_val>>0)&1);//copy of 0th bit.
		Temp_val &= ~(1<<7); //clearing 7th bit.
		Temp_val>>=1; //Shift one time.

		(t2==0)?(Temp_val &= ~(1 << 7)):(Temp_val|= 1 << 7);//copying Previous bit
		(t1==0)?(Temp_val &= ~(1 << 6)):(Temp_val|= 1 << 6);//copying Previous bit
		//printf("Temp2:0x%02x\n",Temp_val);
		Return_value<<=8;
		Return_value+=Temp_val;


		Temp_val = DataIn[2];
		//printf("Data:0x%02x\n",DataIn[2]);
		(t3==0)?(Temp_val &= ~(1 << 7)):(Temp_val|= 1 << 7);//copying Previous bit
		//printf("Temp3:0x%02x\n",Temp_val);
		Return_value<<=8;
		Return_value+=Temp_val;
		//printf("Data:0x%02x\n",DataIn[2]);
		//printf("Data:0x%02x\n",DataIn[1]);
		//printf("Data:0x%02x\n",DataIn[0]);
		return Return_value;
	}
	return 0;
}

uint8_t Calculate_Parity(uint8_t Buff, uint8_t Parity_Type)
{
	int8_t bit = 0;
	int8_t count = 0;

	// printf("For Value:0x%02x\n", Buff);
	for (bit = 6; bit >= 0; bit--)
	{
		//printf("[%02u]:%u| ", bit, ((Buff >> bit) & 1));
		if ((Buff >> bit) & 1)
		{
			count++;
		}
	}
	//printf("\n");
	if (Parity_Type == 0)
	{
		if ((count % 2) == 0)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	if (Parity_Type == 1)
	{
		if ((count % 2) == 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

uint8_t Verify_parity(uint8_t *Data,uint8_t Length,uint8_t Parity_Type)
{
	uint8_t Byte_Traverser = 0;

	for(Byte_Traverser= 0; Byte_Traverser < Length; ++Byte_Traverser)
	{
		if( (Calculate_Parity(Data[Byte_Traverser],Parity_Type)) != ((Data[Byte_Traverser]>>7)&1))
		{
			printf("Failed for data:0x%02x\n",Data[Byte_Traverser]);
			printf("Expected Parity:%u\n",Calculate_Parity(Data[Byte_Traverser],Parity_Type));
			printf("Available Parity:%u\n",((Data[Byte_Traverser]>>7)&1));
			return 0;
		}
	}
	return 1;
}


void Insert_cal_parity(uint8_t start, uint8_t No_of_Bits, uint8_t *Dest_Buff, uint64_t DataIn, uint8_t Parity_Type)
{
	uint16_t t1 = 0, t2 = 0, t3 = 0, t4 = 0;
	uint8_t Temp_Data = 0;

	if(Parity_Type == NO_PARITY)
	{
		InsertBits(start,No_of_Bits,Dest_Buff,DataIn);
		return;
	}

	if (No_of_Bits == 8)
	{
		// printf("One Byte Calculation\n");
		if (DataIn > 0x7f)
		{
			printf("Can't Insert Data as it is more than 7 Bits Length\n");
			return;
		}
		else
		{
			if (Calculate_Parity(DataIn, Parity_Type) == 1)
			{
				//printf("\nSET\n");
				Dest_Buff[0] = DataIn;
				Dest_Buff[0] |= 1 << 7;
			}
			else
			{
				//printf("\nCLEAR\n");
				Dest_Buff[0] = DataIn;
				Dest_Buff[0] &= ~(1 << 7);
			}
		}
	}
	else if (No_of_Bits == 24)
	{
		// printf("Three Byte's Calculation\n");
		if (DataIn > 0x1fffff)
		{
			printf("Can't Insert Data as it is more than 21 Bits Length\n");
			return;
		}
		else
		{
			uint8_t Byte_Traverser = 0;

			for (Byte_Traverser = 0; Byte_Traverser < 3; Byte_Traverser++)
			{
				Temp_Data = DataIn >> (((Byte_Traverser * 8)));
				// printf("#Temp_Data:0x%02x\n",Temp_Data);
				switch (Byte_Traverser)
				{
				case ZERO:
					//printf("*0\n");
					t1 = (((DataIn >> (((Byte_Traverser * 8)))) >> 7) & 1);
					//printf("\nt1 = %u\n", t1);
					break;
				case ONE:
					// printf("*1\n");
					Temp_Data<<=1; //Shifting data by 1 bit
					(t1==0)?(Temp_Data &= ~(1 << 0)):(Temp_Data|= 1 << 0);//copying Previous bit
					t1 = (((DataIn >> (((Byte_Traverser * 8)))) >> 7) & 1);
					//printf("\nt1 = %u\n", t1);
					t2 = (((DataIn >> (((Byte_Traverser * 8)))) >> 6) & 1);
					//printf("\nt2 = %u\n", t2); // Collecting t1 and t2 to inserting in next byte
					break;
				case TWO:
					// printf("*2\n");
					//printf("Temp_Data:%u\n",Temp_Data);
					Temp_Data<<=2; //Shifting data by 2 bit
					//Temp_Data = (uint8_t) Temp_Data;
					//printf("Temp_Data:%u\n",Temp_Data);
					(t2==0)?(Temp_Data &= ~(1 << 0)):(Temp_Data|= 1 << 0);//copying Previous bit
					(t1==0)?(Temp_Data &= ~(1 << 1)):(Temp_Data|= 1 << 1);//copying Previous bit
					//printf("Temp_Data:%u\n",Temp_Data);
					break;
				default:
					break;
				}
				//if (Calculate_Parity(DataIn >> ((16 - (Byte_Traverser * 8))), Parity_Type) == 1)
				if (Calculate_Parity(Temp_Data, Parity_Type) == 1)
				{
					// printf("SET\n");
					//Dest_Buff[Byte_Traverser] = DataIn >> ((16 - (Byte_Traverser * 8)));
					Dest_Buff[3- Byte_Traverser-1] = Temp_Data;
					Dest_Buff[3- Byte_Traverser-1] |= 1 << 7;
					// N |= 1 << K
				}
				else
				{
					// printf("CLEAR\n");
					//Dest_Buff[Byte_Traverser] = DataIn >> ((16 - (Byte_Traverser * 8)));
					Dest_Buff[3- Byte_Traverser-1] = Temp_Data;
					Dest_Buff[3 -Byte_Traverser-1] &= ~(1 << 7);
				}
			}
		}
	}
	else
	{
		printf("Selection Wrong\n");
		return;
	}
	UNUSED(t1);UNUSED(t2);UNUSED(t3);UNUSED(t4);
}

uint16_t Data_Compliment(uint8_t *Buff,uint16_t Size,uint8_t Type)
{
	uint16_t Traverser = 0;
	uint16_t Actual_Size = 0;

#if (SIEMENS_DEBUG==1)
	printf("Size:%u\n",Size);
#endif

	if(Type == VERIFY)
	{
		Actual_Size = ((Size/2)-2);
		for(Traverser = 0; Traverser < Actual_Size; ++Traverser)
		{
			// printf("0x%02x|Compliment:0x%02x\n",Buff[Traverser],Buff[Traverser+Actual_Size+2]);
			if(Buff[Traverser] != (uint8_t)~Buff[Traverser+Actual_Size+2])
			{
				printf("Compliment failed for DATA\n0x%02x|Compliment:0x%02x\n",Buff[Traverser],(uint8_t)~Buff[Traverser+Actual_Size+2]);
				return 0;
			}
		}
		return 1;
	}
	else if(Type == CONVERT)
	{
#if (SIEMENS_DEBUG==1)
		printf("Original Data\n");
		for(Traverser = 0; Traverser <(Size); ++Traverser)
		{
			printf("0x%02x|",Buff[Traverser]);
		}
		printf("Compliment Data:\n");
#endif
		for(Traverser = 0; Traverser <(Size); ++Traverser)
		{
			Buff[Traverser+2+Size] = ~Buff[Traverser];
#if (SIEMENS_DEBUG==1)
			printf("0x%02x|",Buff[Traverser+2+Size]);
#endif
		}
		return (Traverser+Size+2);
	}
	return 0;
}

void CheckToSendReplyorNot(void)
{
	uint8_t Value = 0;

	if(Tx_Siemens_self_sharing_st.MesgReceived == RECEIVED)
	{
		SET_BIT_USER(0,Value);
	}
	else
	{
		CLEAR_BIT_USER(0,Value);
	}
	if(Rx_Siemens_self_sharing_st.MesgReceived == RECEIVED)
	{
		SET_BIT_USER(1,Value);
	}
	else
	{
		CLEAR_BIT_USER(1,Value);
	}

	//	printf("Value:%u\n",Value);

	switch(Value)
	{
	case ZERO:
	case ONE:
	case TWO:
		SendReply = 1;
		break;
	case THREE:
#ifdef EI_1
		SendReply = 1;
#endif
#ifdef EI_2
		SendReply = 0;
#endif
		break;
	default:
		SendReply = 1;
		break;
	}

}



void PrintSignals(void)
{
	memcpy(&VitalData2MIE_st.Relays,&SiemensMain_Relays_st.Relays,SiemensMain_Relays_st.NoofDataBytes);

	for(uint16_t Traverser = 1;Traverser<=(VitalData2MIE_st.NoofDataBytes * 8);++Traverser)
	{
		if(Traverser > TotalSigCount)
		{
			continue;
		}

		if(GETPIN(Traverser) > 0)
		{
			printf("%15s:PICKUP   ",Signals[Traverser-1]);
		}
		else
		{
			printf("%15s:DROP      ",Signals[Traverser-1]);
		}

		if( (Traverser%2) == 0)
		{
			printf("\n");
		}
	}
}

void GetTimeStamp(void)
{
	uint32_t TimeFrame = 0;
	static uint32_t PrevTimeFrame = 0;
	uint32_t TimeStamp = 0;
	static 	uint32_t PrevTimeStamp = 0;
	static uint32_t TickswhenTimeChanged = 0;
	uint32_t ElapsTicksAfterTimeChange = 0;

	Get_time();
	TimeFrame = (((RTC_Date_time_Dec_st.Hour * 3600) +(RTC_Date_time_Dec_st.Mins * 60) + (RTC_Date_time_Dec_st.Secs)));

	if(TimeFrame != PrevTimeFrame)
	{
		PrevTimeFrame = TimeFrame;
		TickswhenTimeChanged = HAL_GetTick();
	}

	ElapsTicksAfterTimeChange = HAL_Elaps_Ticks(TickswhenTimeChanged);

	TimeStamp  = (TimeFrame * 10) + (ElapsTicksAfterTimeChange/100);
	if(TimeStamp != PrevTimeStamp)
	{
		PrevTimeStamp = TimeStamp;
		SelfTimeStamp = TimeStamp;
		//		printf("Ticks:%lu|ms:%lu|%lu\n",HAL_GetTick(),	TimeStamp,(ElapsTicksAfterTimeChange));
	}
}


void UpdateSIGNALSNames(void)
{
	uint8_t Signals_Cnt = 0;
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"CLTPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"DMTPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"UMTPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"ULTPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"1ATPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"30ATPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"3_4TPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"6TPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"26_28TPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"25TPR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1D1_DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1D1_HHECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1D1_HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1D2_DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1D2_HHECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1D2_HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1UECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S1A_COHECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S3DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S3HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S3RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S4HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S4RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S6DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S6RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S25DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S25RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S26HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S26RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S27HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S27RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S28DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S28HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S28RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30RECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30UECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30A_COHECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30D2_DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30D2_HHECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30D2_HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30D1_DECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30D1_HHECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"S30D1_HECR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"11NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"11RWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"12NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"12RWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"13NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"13RWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"18NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"18RWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"19NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"19RWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"20NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"20RWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"15KL_NWKR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"22LXCR");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"BR_LCPR_STNA");
	strcpy((char *)Signals[Signals_Cnt++], (const char *)"BR_LCPR_STNC");
	TotalSigCount = Signals_Cnt;


}

void print_self_DetailsSiemens(void)
{

	printf("Station Name:%s\n",EI_data.STN_NAME);
	printf("No of IPS:%u\n",EI_data.Siemens_st.No_of_IP_s);
	printf("Self IP:%u.%u.%u.%u\n",EI_data.Siemens_st.Self_IP[CARD_INDEX][0],EI_data.Siemens_st.Self_IP[CARD_INDEX][1],EI_data.Siemens_st.Self_IP[CARD_INDEX][2],EI_data.Siemens_st.Self_IP[CARD_INDEX][3]);
	printf("Self MAC:%02X-%02X-%02X-%02X-%02X-%02X\n",EI_data.Siemens_st.Self_MAC[CARD_INDEX][0],EI_data.Siemens_st.Self_MAC[CARD_INDEX][1],EI_data.Siemens_st.Self_MAC[CARD_INDEX][2],EI_data.Siemens_st.Self_MAC[CARD_INDEX][3],EI_data.Siemens_st.Self_MAC[CARD_INDEX][4],EI_data.Siemens_st.Self_MAC[CARD_INDEX][5]);
	printf("Self Listening Port:%u\n",EI_data.Siemens_st.Self_Listening_Port[CARD_INDEX]);

	for(uint8_t i=0;i<EI_data.Siemens_st.No_of_IP_s;++i)
	{
		printf("IP[%u]:%u.%u.%u.%u\n",i+1,EI_data.Siemens_st.Source_IP[CARD_INDEX][i][0],EI_data.Siemens_st.Source_IP[CARD_INDEX][i][1],EI_data.Siemens_st.Source_IP[CARD_INDEX][i][2],EI_data.Siemens_st.Source_IP[CARD_INDEX][i][3]);
		printf("Dest Port-TX:%u-RX:%u\n",EI_data.Siemens_st.PORTS[CARD_INDEX][i][TX_PORT],EI_data.Siemens_st.PORTS[CARD_INDEX][i][RX_PORT]);
	}
	printf("DestSysType:%u\n",EI_data.Siemens_st.DestSysType);
	printf("SourceSysType:%u\n",EI_data.Siemens_st.SourceSysType);
	printf("SourceAddress:%lu\n",EI_data.Siemens_st.SourceAddress);
	printf("SourceAddress:%lu\n",EI_data.Siemens_st.DestAddress);
	printf("DestPort:%u\n",EI_data.Siemens_st.DestPort);
	printf("SourcePort:%u\n",EI_data.Siemens_st.SourcePort);
	printf("No of Inputs:%u\n",EI_data.Siemens_st.Booloutputs);

	printf("CAN_Xchange_Time:%lu\n",EI_data.Siemens_st.CAN_Xchange_Time);
	printf("HEALTH_PACKET_TIME:%lu\n",EI_data.Siemens_st.HEALTH_PACKET_TIME);
	printf("COMM_FAILURE_TIME_OUT:%lu\n",EI_data.Siemens_st.COMM_FAILURE_TIME_OUT);
	printf("MIE_SIGNAL_SEND_CLK:%lu\n",EI_data.Siemens_st.MIE_SIGNAL_SEND_CLK);
	if(EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount == 0)
	{
		EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount = 50;
	}
	printf("Max_NoRXTSUpdatedFailCount:%lu\n",EI_data.Siemens_st.Max_NoRXTSUpdatedFailCount);

}
