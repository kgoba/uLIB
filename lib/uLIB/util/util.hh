#pragma once

#include "../PTLib/types.hh"

#define C_ARRAY_SIZE(array)     (sizeof(array) / sizeof(array[0]))

#define bit_set(var, bit)       (var |= (1 << (bit)))
#define bit_clear(var, bit)     (var &= ~(1 << (bit)))
#define bit_check(var, bit)     (var & (1 << (bit)))

#define WORD_LO(x)            ((x) & 0xFF)
#define WORD_HI(x)            (((x) >> 8) & 0xFF)

#define ENTER_CRITICAL   byte tmpSREG = SREG; cli();
#define EXIT_CRITICAL    SREG = tmpSREG;
