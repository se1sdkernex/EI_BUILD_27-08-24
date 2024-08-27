/*
 * Macros.h
 *
 *  Created on: May 10, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_MACROS_H_
#define INC_MACROS_H_

#include "Headers.h"



enum Numbers {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    TEN, ELEVEN, TWELVE, THIRTEEN, FOURTEEN, FIFTEEN, SIXTEEN, SEVENTEEN, EIGHTEEN, NINETEEN,
    TWENTY, TWENTY_ONE, TWENTY_TWO, TWENTY_THREE, TWENTY_FOUR, TWENTY_FIVE, TWENTY_SIX, TWENTY_SEVEN, TWENTY_EIGHT, TWENTY_NINE,
    THIRTY, THIRTY_ONE, THIRTY_TWO, THIRTY_THREE, THIRTY_FOUR, THIRTY_FIVE, THIRTY_SIX, THIRTY_SEVEN, THIRTY_EIGHT, THIRTY_NINE,
    FORTY, FORTY_ONE, FORTY_TWO, FORTY_THREE, FORTY_FOUR, FORTY_FIVE, FORTY_SIX, FORTY_SEVEN, FORTY_EIGHT, FORTY_NINE,
    FIFTY, FIFTY_ONE, FIFTY_TWO, FIFTY_THREE, FIFTY_FOUR, FIFTY_FIVE, FIFTY_SIX, FIFTY_SEVEN, FIFTY_EIGHT, FIFTY_NINE,
    SIXTY, SIXTY_ONE, SIXTY_TWO, SIXTY_THREE, SIXTY_FOUR, SIXTY_FIVE, SIXTY_SIX, SIXTY_SEVEN, SIXTY_EIGHT, SIXTY_NINE,
    SEVENTY, SEVENTY_ONE, SEVENTY_TWO, SEVENTY_THREE, SEVENTY_FOUR, SEVENTY_FIVE, SEVENTY_SIX, SEVENTY_SEVEN, SEVENTY_EIGHT, SEVENTY_NINE,
    EIGHTY, EIGHTY_ONE, EIGHTY_TWO, EIGHTY_THREE, EIGHTY_FOUR, EIGHTY_FIVE, EIGHTY_SIX, EIGHTY_SEVEN, EIGHTY_EIGHT, EIGHTY_NINE,
    NINETY, NINETY_ONE, NINETY_TWO, NINETY_THREE, NINETY_FOUR, NINETY_FIVE, NINETY_SIX, NINETY_SEVEN, NINETY_EIGHT, NINETY_NINE,
    HUNDRED
};

enum EI_Protocols
{
	HITACHI =1,
	SIEMENS,
	MEDHA,
	KYOSAN
};

#define CLEAR                   0
#define SET						1



//------------------------- SPI_FLASH -------------------------
#define CHIP1						1
#define CHIP2						2
#define CHIP3						3
#define CHIP4						4
//------------------------- SPI_FLASH -------------------------


#define DATREQUEST				0X01
#define ACKOK					0x02
#define CRCNOK					0X03
#define DATCONFRMTION 			0X04
#define NODATA					0X05

#define DATLOADING				0X0001
#define DATSENDING				0X0002

#define WAITFORDAT				30


#ifdef EI_1
#define CARD_INDEX	0
#endif
#ifdef EI_2
#define CARD_INDEX	1
#endif

#define EIPC_CARDS				2
#define MAX_NO_IP				(MAX_PEER_CONNECTIONS * 2)
#define IP_LENGTH				4
#define PORTS_CNT_PER_IP		2
#define MAC_LENGTH				6


#define SIEMENS_MAX_IP		4
#define SIEMENS_MAX_PORT	4



//#define ZERO					0
//#define ONE						1
//#define TWO						2
//#define THREE					3
//#define FOUR                    4
//#define FIVE                    5
//#define SIX                     6
//#define SEVEN                   7
//#define EIGHT                   8
//#define NINE                    9
//#define TEN                     10
//#define ELEVEN	                11
//#define TWELVE	                12
//#define THIRTEEN				13
//#define FOURTEEN	            14
//#define FIFTEEN					15
//#define TWENTY_FOUR             24
//#define FIFTY				    50
//#define FIFTYFIVE               55
//#define SIXTY                   60

#define HEX_F				    0x000F
#define HEX_FF				    0x00FF
#define HEX_FFF				    0x0FFF
#define HEX_FFFF			    0xFFFF

#define NO						0
#define YES						1
#define START_OF_MESSAGE		'$'
#define END_OF_MESSAGE			'&'
#define	DLE						0x05


//#define HITACHI					1
//#define	SIEMENS					2
//#define MEDHA                   3

#define TX_PORT					0
#define RX_PORT					1

#define MAX_UDP_CONNECTIONS		(MAX_PEER_CONNECTIONS * 2)

#define ONE_MS 						1
#define TWO_MS						2
#define	FIVE_MS						5
#define TEN_MS						10
#define TWENTY_MS					20
#define THIRTY_MS					30
#define FIFTY_MS					50
#define HUNDRED_MS					100
#define TWO_HUNDRED_MS				200
#define THREE_HUNDRED_MS			300
#define FIVE_HUNDRED_MS				500
#define ONE_SEC 					1000
#define ONE_HALF_SEC				1500
#define TWO_SEC 					2000
#define THREE_SEC					3000
#define FOUR_SEC					4000
#define FIVE_SEC 					5000
#define SIX_SEC						6000
#define SEVEN_SEC					7000
#define EIGHT_SEC					8000
#define TEN_SEC						10000
#define FIFTEEN_SEC					15000
#define TWENTY_SEC					20000
#define THIRTY_SEC					30000
#define ONE_MIN						60000


#endif /* INC_MACROS_H_ */
