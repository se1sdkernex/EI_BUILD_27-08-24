/*
 * RTC.h
 *
 *  Created on: Sep 7, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

#include "Headers.h"


typedef struct
{
	uint8_t Hour;
	uint8_t Mins;
	uint8_t Secs;
	uint8_t Date;
	uint8_t Mont;
	uint8_t Year;

}GPS_DATE_TIME_t;



extern GPS_DATE_TIME_t Gps_date_time_st,Temp_Gps_date_time_st,RTC_Date_time_Dec_st,RTC_Date_time_BCD_st;

uint8_t bcdToDec(const uint8_t Kuc_bcd);
void Write_RTC(void);
uint8_t Dec_to_BCD(uint8_t c);
void Get_time(void);
void Update_Time_Every_Sec(void);
#endif /* INC_RTC_H_ */
