#ifndef _SYSTICK_H_INCLUDED_
#define _SYSTICK_H_INCLUDED_

#include <stdint.h>


void systick_setup(void);


void delay(uint32_t delay);


uint32_t get_millis_elapsed(void);

#endif
