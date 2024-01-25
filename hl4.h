// hl4.h

#ifndef HL4_H
#define HL4_H

#include <stdint.h>
#include <stdlib.h>
#include "common_hll.h"

typedef struct {
    CommonHLL commonHLL;
} HL4;

HL4* createHyperLogLog4(unsigned char p, unsigned char q);
void destroyHyperLogLog4(HL4* hll);

#endif // HL2_H


