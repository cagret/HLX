// hl3.h

#ifndef HL3_H
#define HL3_H

#include <stdint.h>
#include <stdlib.h>
#include "common_hll.h"

typedef struct {
    CommonHLL commonHLL;
} HL3;

HL3* createHyperLogLog3(unsigned char p, unsigned char q);
void destroyHyperLogLog3(HL3* hll);

#endif // HL3_H


