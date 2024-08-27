/*
 * BitManip.h
 *
 *  Created on: May 15, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_BITMANIP_H_
#define INC_BITMANIP_H_


#define BITSTUFFINIT(x,y)	uint16_t ucByte = y, ucBit = x;
//BITSTUFFSIZE(x) ucSize = x;
#define BITSTUFFINC(x)	{ucByte = (unsigned short int)((short int)ucByte+(((short int)ucBit+x)/8)); ucBit = (unsigned short int)((short int)(ucBit+x)%8);}

#define BYTEINIT(x)		uint16_t ucByte = x;
#define BYTEINC(x) 		{ucByte+=x;}

#define GET_BIT_MASK(bit)	((0x01)<<(bit))
#define SET_BIT_USER(bit,val)	((val) |= (GET_BIT_MASK(bit)))
#define CLEAR_BIT_USER(bit,val)	((val) &= ~(GET_BIT_MASK(bit)))
#define CHECKFLAG(bit,val)	(((val) & (GET_BIT_MASK(bit)))?SET:CLEAR)
//
//#define ONE					1
//#define TWO					2
//#define THREE				3
//#define FOUR				4
//#define FIVE				5
//#define SEVEN				7
//#define EIGHT				8
//#define FIFTEEN				15
//#define SIXTEEN				16
//#define TWENTY_FOUR			24
//#define TWENTY_THREE			23
//#define THIRTY_ONE			31

/* Function for extraction of Data*/

unsigned long ExtractBits(unsigned short int, unsigned short int,
								unsigned char *);
/* Function for insertion of Data*/
unsigned long InsertBits(unsigned short int, unsigned short int,
								unsigned char *, unsigned long);

#endif /* INC_BITMANIP_H_ */
