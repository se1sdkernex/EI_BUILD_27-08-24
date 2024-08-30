/*
 * Headers.h
 *
 *  Created on: May 10, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_HEADERS_H_
#define INC_HEADERS_H_




#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif

#include "i2c.h"
#include "Process_Cmd.h"
#include "crc.h"
#include "usart.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "udp.h"
#include "spi.h"
#include "Macros.h"
#include "User_UDP.h"
#include "Diagnosis.h"
#include "Hitachi.h"
#include "Siemens.h"
#include "Medha.h"
#include "kyosan.h"
#include "Config.h"
#include "iwdg.h"
#include "Ticks.h"
#include "can.h"
#include "gpio.h"
#include "rng.h"
#include "Last_Reset_Reason.h"
#include "RTC.h"


//#include "Hitachi.h"
#include "BitManip.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>



#endif /* INC_HEADERS_H_ */
