/*
 * BitManip.c
 *
 *  Created on: May 15, 2023
 *      Author: Ravi Teja P
 */


#include "Headers.h"

/******************************************************************************
*	File Header             BitManip.c
*	Procedure Name :        CommExtractBits
*	Package Name   :        Communication Module
*	Date of creation :      21/10/2009
*	Created by     :        dskr
*
*	Modification history
*	+-------------------------------------------------------------------------+
*	|  Revision   |  SPR   |   ATR    |  Date      |     Remarks              |
*	|-------------|-------------------|------------|--------------------------|
*	|     01      |        |          | 21/10/2009 |      Initial Version     |
*	+-------------------------------------------------------------------------+
*
*	Abstract : This function extracts the value from the given message given
*			   the starting bit and no. of bits
*
*	Allocation    :	   NIL
*
*	DR            :    NIL
*
*	Interfaces	  :		  None
*	Calls
*
*	Called by     :       1.Cmgt_AssignConfig()
*						  2.Comm_UpdateCfgParams()
*						  3.Comm_UpdateDateAndTime()
*						  4.Comm_ProcessMasterData()
*
*	Input variables  :
*	Local variables  :  Type                Name		               comments
*
*					 unsigned short int	   us_Start	     Starting Bit Position
*					 unsigned short int	   us_NoOfBits    No of Bits
*					 unsigned char         puc_Msg		 Message Contains Data
*
*	Global variables :     Type                   Name		     comments
*
*							--                    None              --
*
*	Output variables :
*	Local variables  :     Type                   Name		     comments
*
*						   --                     None              --
*
*	Global variables :     Type                   Name		     comments
*
*						unsigned long          ul_DataBits    Extracted Data
*
*
*	Reference	     :     SRS		SDR		SWRS		DR
*
*	Derived Requirements  :
*
*	Program Design Language        :
*
*	step1 : compute the offset of the farthest bit
*	step2 : Read required no. of bytes
*	step3 : Compute number of bit position to shift
*	step4 : Shift the mask to required position and extract the bits
*	step5 : Right adjust the extracted bits
*
******************************************************************************/

unsigned long ExtractBits(unsigned short int us_Start,
                            unsigned short int us_NoOfBits,
                            unsigned char *puc_Msg)
{
    unsigned long ul_DataBits;          /* To store extracted data */
    unsigned long ul_BitMask;           /* Used for bit masking */
    unsigned short int us_ShiftCount;   /* Used for shifting */
    unsigned short int us_BitPos;       /* Contains the bit position */
    unsigned short int us_NBytes;       /* Number of bytes */

    static unsigned long sul_AdMask[] =
    {
        0x0,0x1,0x3,0x7,0xF,
		0x1F,0x3F,0x7F,0xFF,
		0x1FF,0x3FF,0x7FF,0xFFF,
		0x1FFF,0x3FFF,0x7FFF,0xFFFF,
		0x1FFFFL,0x3FFFFL,0x7FFFFL,0xFFFFFL,
		0x1FFFFFL,0x3FFFFFL,0x7FFFFFL,0xFFFFFFL,
		0x1FFFFFFL,0x3FFFFFFL,0x7FFFFFFL,0xFFFFFFFL,
		0x1FFFFFFFL,0x3FFFFFFFL,0x7FFFFFFFL,0xFFFFFFFFL
    };                                  /* Used for Masking */

    /* compute the offset of the farthest bit */
	us_BitPos = us_Start + us_NoOfBits;

    if (us_BitPos <= EIGHT)
    {
	    /* All bits are in the same byte */
	    us_Start = SEVEN - us_Start;
	    us_NBytes = ONE;
    }
    else if (us_BitPos <= SIXTEEN)
    {
	    /* Bits straddle 2 bytes */
	    us_Start = FIFTEEN - us_Start;
	    us_NBytes = TWO;
    }
		 else if (us_BitPos <= TWENTY_FOUR)
    {
	    /* Bits straddle 2 bytes */
	    us_Start = TWENTY_THREE - us_Start;
	    us_NBytes = THREE;
    }
    else
    {
	    /* Bits straddle 3 bytes */
	    us_Start = THIRTY_ONE - us_Start;
	    us_NBytes = FOUR;
    }
    ul_DataBits = ZERO;

    /* Read required no. of bytes */
    switch (us_NBytes)
    {
	    case FOUR:
	        ul_DataBits |= *puc_Msg++;	/* caution: Fall through */
	    case THREE:
	        ul_DataBits <<= EIGHT;
	        ul_DataBits |= *puc_Msg++;
	    case TWO:
	        ul_DataBits <<= EIGHT;
	        ul_DataBits |= *puc_Msg++;
	    case ONE:
	        ul_DataBits <<= EIGHT;
	        ul_DataBits |= *puc_Msg++;
    }

    /* Compute number of bit position to shift */
    us_ShiftCount = us_Start - us_NoOfBits + ONE;

    /* Get bit mask which contains the 'us_NoOfBits' set to 1 */
    ul_BitMask = sul_AdMask[us_NoOfBits];

    /* Shift the mask to required position and extract the bits */
    ul_DataBits = ((ul_BitMask << (us_ShiftCount)) & ul_DataBits);

    /* Right adjust the extracted bits */
    ul_DataBits = (ul_DataBits >> (us_ShiftCount));

    return(ul_DataBits);
}	/* End of CommExtractBits */

/******************************************************************************
*	File Header           BitManip.c
*	Procedure Name :      CommInsertBits
*	Package Name   :      Communication Module
*	Date of creation :    21/10/2009
*	Created by     :      dskr
*
*	Modification history
*	+-------------------------------------------------------------------------+
*	|  Revision   |  SPR   |   ATR    |  Date      |     Remarks              |
*	|-------------|-------------------|------------|--------------------------|
*	|     01      |        |          | 21/10/2009 |      Initial Version     |
*	+-------------------------------------------------------------------------+
*
*	Abstract      :    This function inserts a given value into a character
*					   string given the starting bit no. and the no. of bits
*
*	Allocation	  :	   NIL
*
*	DR            :    NIL
*
*	Interfaces	  :	   None
*	Calls
*
*	Called by     :      1.Comm_FormACKTrainDeparture()
*						 2.Comm_FormACKTrainApproach()
*						 3.Comm_FormACKMModeSelection()
*						 4.Comm_FormACKMModeDeselection()
*						 5.Comm_FormACKOperationMode()
*						 6.Comm_FormAckConfigModeStatus()
*						 7.Comm_FormAckFieldDevice()
*						 8.Comm_FormAckDateLog()
*						 9.Comm_FormAckNoData()
*						10.Comm_FormAckEcmPartialfail()
*						11.Comm_FormAckConfigDataReq()
*						12.Comm_FormAckConfig()
*						13.Comm_FormAckDateTime()
*
*	Input variables  :
*	Local variables  :     Type                       Name		       comments
*
*				unsigned short int		   us_Start        Stating Bit Position
*				unsigned short int		   us_NoOfBits      Number of Bits
*				unsigned char		       puc_Msg          Message Buffer
*				unsigned long		       ul_DataIn       Data to be Inserted
*
*
*	Global variables :     Type                       Name		       comments
*
*							--	                      None                --
*
*
*	Output variables :
*	Local variables  :     Type                       Name		       comments
*
*							--                        None                --
*
*
*	Global variables :     Type                       Name		       comments
*
*				     unsigned long              ul_DataBits      Extracted Data
*
*
*	Reference	     :     SRS		SDR		SWRS		DR
*
*	Derived Requirements  :
*
*	Program Design Language        :
*
*	step1 : compute the offset of the farthest bit
*	step2 : Read required no. of bytes
*	step3 : Compute number of bit position to shift
*	step4 : Shift the mask to required position and insert the bits
*	step5 : Copy the bits and then Update bytes
*
******************************************************************************/

unsigned long InsertBits(unsigned short int us_Start,
                        unsigned short int us_NoOfBits,
                        unsigned char *puc_Msg,
                        unsigned long ul_DataIn)
{
    unsigned long ul_DataBits;          /* To store extracted data */
    unsigned long ul_BitMask;           /* Used for bit masking */
    unsigned short int us_ShiftCount;   /* Used for shifting */
    unsigned short int us_BitPos;       /* Contains the bit position */
    unsigned short int us_NBytes;       /* Number of bytes */
    unsigned char *puc_Bit;             /* For bit information */

    static unsigned long sul_AdMask[] =
    {
	    0x0,0x1,0x3,0x7,0xF,
        0x1F,0x3F,0x7F,0xFF,
	    0x1FF,0x3FF,0x7FF,0xFFF,
	    0x1FFF,0x3FFF,0x7FFF,0xFFFF,
	    0x1FFFFL,0x3FFFFL,0x7FFFFL,0xFFFFFL,
	    0x1FFFFFL,0x3FFFFFL,0x7FFFFFL,0xFFFFFFL,
	    0x1FFFFFFL,0x3FFFFFFL,0x7FFFFFFL,0xFFFFFFFL,
	    0x1FFFFFFFL,0x3FFFFFFFL,0x7FFFFFFFL,0xFFFFFFFFL
    };                                   /* Used for Masking */

    /* Save current byte pointer */
	puc_Bit = puc_Msg;

    /* compute the offset of the farthest bit */
    us_BitPos = us_Start + us_NoOfBits;

    if (us_BitPos <= EIGHT)
    {
	    /* All bits are in the same byte */
	    us_Start = SEVEN - us_Start;
	    us_NBytes = ONE;
    }
    else if (us_BitPos <= SIXTEEN)
    {
	    /* Bits straddle 2 bytes */
	    us_Start = FIFTEEN - us_Start;
	    us_NBytes = TWO;
    }
		else if (us_BitPos <= TWENTY_FOUR)
		{
			us_Start = TWENTY_THREE - us_Start;
	    us_NBytes = THREE;
		}
    else
    {
	    /* Bits straddle 4 bytes */
	    us_Start = THIRTY_ONE - us_Start;
	    us_NBytes = FOUR;
    }

    ul_DataBits = ZERO;

    /* Read current data */
    switch (us_NBytes)
    {
	    case FOUR:
	        ul_DataBits |= *puc_Msg++;	 /* Caution: Fall through */
	    case THREE:
	        ul_DataBits <<= EIGHT;
	        ul_DataBits |= *puc_Msg++;
	    case TWO:
	        ul_DataBits <<= EIGHT;
	        ul_DataBits |= *puc_Msg++;
	    case ONE:
	        ul_DataBits <<= EIGHT;
	        ul_DataBits |= *puc_Msg;
			default:
				   ;
    }

    /* Compute number of bit position to shift */
    us_ShiftCount = us_Start - us_NoOfBits + ONE;

    /* get bit mask which contains the 'us_NoOfBits' set to 1 */
    ul_BitMask = sul_AdMask[us_NoOfBits];

    /* Mask the unwanted bits */
    ul_DataIn &= ul_BitMask;

    /* Shift the data bits to required position */
    ul_DataIn <<= us_ShiftCount;

    /* Reset the existing bits to 0 as per the mask */
    ul_DataBits &= ~(ul_BitMask << us_ShiftCount);

    /* Copy the bits */
    ul_DataBits |= ul_DataIn;

    /* Update bytes */
    switch (us_NBytes)
    {
	    case FOUR:
	        *puc_Bit++ = (char)(ul_DataBits >> TWENTY_FOUR);	/* caution: Fall through */
	    case THREE:
	        *puc_Bit++ = (char)(ul_DataBits >> SIXTEEN);
	    case TWO:
	        *puc_Bit++ = (char)(ul_DataBits >> EIGHT);
	    case ONE:
	        *puc_Bit = (char)(ul_DataBits);
			default:
				   ;
    }

    return ul_DataBits;
}	/* End of CommInsertBits */

/*End Of File*/




