#pragma once

#include "types.hh"

#define ARRAY_SIZE(array)     (sizeof(array) / sizeof(array[0]))

#define bit_set(var, bit)       (var |= (1 << (bit)))
#define bit_clear(var, bit)     (var &= ~(1 << (bit)))
#define bit_check(var, bit)     (var & (1 << (bit)))
#define bit_mask1(bit)          (1 << (bit))
#define bit_mask2(bit1, bit2)   ((1 << bit1) | (1 << bit2))
#define bit_mask3(b1, b2, b3)   ((1 << b1) | (1 << b2) | (1 << b3))
#define bit_mask4(b1, b2, b3, b4)   ((1 << b1) | (1 << b2) | (1 << b3) | (1 << b4))

#define WORD_LO(x)            ((x) & 0xFF)
#define WORD_HI(x)            (((x) >> 8) & 0xFF)
#define WORD_LH(l, h)         ((l) | ((word)(h) << 8))

#define ENTER_CRITICAL   byte tmpSREG = SREG; cli();
#define EXIT_CRITICAL    SREG = tmpSREG;

