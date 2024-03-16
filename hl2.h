// hl2.h
#ifndef HL2_H
#define HL2_H

#include "common_hll.h"
#include <stdint.h>

typedef struct {
    CommonHLL commonHLL;
} HL2;

HL2* createHL2(unsigned char p, unsigned char q);
void destroyHL2(HL2* hll);
void insertHL2(HL2* hll, uint64_t hash_value);
#endif /* HL2_H */

