/*
 * Last_Reset_Reason.h
 *
 *  Created on: Oct 17, 2023
 *      Author: Ravi Teja P
 */

#ifndef INC_LAST_RESET_REASON_H_
#define INC_LAST_RESET_REASON_H_


#include "Headers.h"


typedef enum reset_cause_e
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET,
    RESET_CAUSE_BROWNOUT_RESET,
} reset_cause_t;


reset_cause_t reset_cause_get(void);
char * reset_cause_get_name(reset_cause_t reset_cause);

#endif /* INC_LAST_RESET_REASON_H_ */
