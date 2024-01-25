// hl2.h

#ifndef HL2_H
#define HL2_H

#include <stdint.h>
#include <stdlib.h>
#include "common_hll.h"

typedef struct {
    CommonHLL commonHLL;
} HL2;

HL2* createHyperLogLog2(unsigned char p, unsigned char q);
void destroyHyperLogLog2(HL2* hll);

#endif // HL2_H


