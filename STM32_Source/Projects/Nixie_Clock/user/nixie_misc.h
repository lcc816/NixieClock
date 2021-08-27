/*
 * nixie_misc.h
 *
 *  Created on: Aug 28, 2021
 *      Author: lcc
 */

#ifndef __NIXIE_MISC_H_
#define __NIXIE_MISC_H_

#include "comm_def.h"
#include <stdint.h>

int date2day(uint16_t year, uint8_t month, uint8_t date, uint8_t *day);

#endif /* __NIXIE_MISC_H_ */
