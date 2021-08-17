/*
 * comm_def.h
 *
 *  Created on: 2021年8月8日
 *      Author: Lichangchun
 */

/* in case no define NULL */
#include <stddef.h>

/* arm-eabi-gcc or IAR */
#if defined(__GNUC__) || defined(__ICCARM__)
#define __nop() asm("nop")
#endif
