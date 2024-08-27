/*
 * Ticks.c
 *
 *  Created on: May 11, 2023
 *      Author: Ravi Teja P
 */


#include "Headers.h"


uint32_t HAL_Elaps_Ticks(uint32_t Ticks)
{
	return abs( (HAL_GetTick()-Ticks) );
}
