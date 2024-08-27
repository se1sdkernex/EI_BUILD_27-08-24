/*
 * RTC.c
 *
 *  Created on: Sep 7, 2023
 *      Author: Ravi Teja P
 */

#include "Headers.h"


GPS_DATE_TIME_t Gps_date_time_st,Temp_Gps_date_time_st,RTC_Date_time_Dec_st,RTC_Date_time_BCD_st;
#define RTC_I2C_ADDR 			0xD0 //0x68<<1
#define TEMPERATURE_I2C_ADDR 	0x90 //0x48<<1


uint8_t bcdToDec(const uint8_t Kuc_bcd)
{
	return (((Kuc_bcd/16)*10) + (Kuc_bcd%16));
}

uint8_t Dec_to_BCD(uint8_t c)
{
	c=((c/10)*16)+(c%10);
	return c;
}

uint8_t Is_GPS_Valid(void)
{
	//printf("\n%u:%u:%u|%u/%u/%u\n",Gps_date_time_st.ucHour,Gps_date_time_st.ucMin,Gps_date_time_st.ucSec,Gps_date_time_st.ucDate,Gps_date_time_st.ucMonth,Gps_date_time_st.ucYear);

	if(Gps_date_time_st.Year < 10)
	{
		return 0;
	}
	if(Gps_date_time_st.Mont == 0 )
	{
		return 0;
	}
	if(Gps_date_time_st.Date == 0 )
	{
		return 0;
	}
	return 1;
}


void Write_RTC(void)
{
	uint8_t set_time[7] = {0,};

	if(Is_GPS_Valid() ==0)
	{
		printf("Invalid DATE received from MIE\n");
		return;
	}


	Is_GPS_Packet_Received = 1;

	set_time[0] = Dec_to_BCD(Gps_date_time_st.Secs);
	set_time[1] = Dec_to_BCD(Gps_date_time_st.Mins);
	set_time[2] = Dec_to_BCD(Gps_date_time_st.Hour);

	set_time[3] = Dec_to_BCD(Gps_date_time_st.Date);
	set_time[4] = Dec_to_BCD(Gps_date_time_st.Mont);
	set_time[5] = Dec_to_BCD(Gps_date_time_st.Year);

	HAL_I2C_Mem_Write(&hi2c1, RTC_I2C_ADDR,0X00,1,&set_time[0],1,1000); //Seconds

	HAL_I2C_Mem_Write(&hi2c1, RTC_I2C_ADDR,0X01,1,&set_time[1],1,1000); //Minutes

	HAL_I2C_Mem_Write(&hi2c1, RTC_I2C_ADDR,0X02,1,&set_time[2],1,1000); //Hour

	HAL_I2C_Mem_Write(&hi2c1, RTC_I2C_ADDR,0X04,1,&set_time[3],1,1000); //Date

	HAL_I2C_Mem_Write(&hi2c1, RTC_I2C_ADDR,0X05,1,&set_time[4],1,1000); //Month

	HAL_I2C_Mem_Write(&hi2c1, RTC_I2C_ADDR,0X06,1,&set_time[5],1,1000); //Year

	Get_time();

}


void Get_time(void)
{
	uint8_t get_time[7];

	memset(get_time,0,sizeof(get_time));
	HAL_I2C_Mem_Read(&hi2c1, RTC_I2C_ADDR,0X00,1,get_time,7,1000);

	RTC_Date_time_Dec_st.Secs = bcdToDec(get_time[0]);
	RTC_Date_time_Dec_st.Mins = bcdToDec(get_time[1]);
	RTC_Date_time_Dec_st.Hour = bcdToDec(get_time[2]);
	RTC_Date_time_Dec_st.Date = bcdToDec(get_time[4]);
	RTC_Date_time_Dec_st.Mont = bcdToDec(get_time[5]);
	RTC_Date_time_Dec_st.Year = bcdToDec(get_time[6]);


	RTC_Date_time_BCD_st.Secs = (get_time[0]);
	RTC_Date_time_BCD_st.Mins = (get_time[1]);
	RTC_Date_time_BCD_st.Hour = (get_time[2]);
	RTC_Date_time_BCD_st.Date = (get_time[4]);
	RTC_Date_time_BCD_st.Mont = (get_time[5]);
	RTC_Date_time_BCD_st.Year = (get_time[6]);

	//printf(" CURRENT DATE: 	");
	//printf("%02u/%02u/%02u\n",RTC_Date_time_Dec_st.Date,RTC_Date_time_Dec_st.Mont,RTC_Date_time_Dec_st.Year);

	//printf(" CURRENT TIME: 	");
	//printf("%02u:%02u:%02u\n\n",RTC_Date_time_Dec_st.Hour,RTC_Date_time_Dec_st.Mins,RTC_Date_time_Dec_st.Secs);

	return;
}


void Update_Time_Every_Sec(void)
{
	static uint32_t RTC_CLK=0;

	if( HAL_Elaps_Ticks(RTC_CLK) >=1000)
	{
		Get_time();
		//		printf("\n CURRENT DATE: 	");
		//		printf("%02u/%02u/%02u\n",RTC_Date_time_Dec_st.Date,RTC_Date_time_Dec_st.Mont,RTC_Date_time_Dec_st.Year);
		//
		//		printf(" CURRENT TIME: 	");
		//		printf("%02u:%02u:%02u\n\n",RTC_Date_time_Dec_st.Hour,RTC_Date_time_Dec_st.Mins,RTC_Date_time_Dec_st.Secs);
		RTC_CLK=HAL_GetTick();
	}
	return;
}



