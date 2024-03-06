#ifndef HL3_H
#define HL3_H

#include "common_hll.h"
#include <stdint.h>

typedef struct {
    CommonHLL commonHLL;
} HL3;

HL3* createHL3(unsigned char p, unsigned char q, size_t sketch_size);
void destroyHL3(HL3* hl3);
//double hl3_get_cardinality(HL3* hl3);
void insertHL3(HL3* hl3, uint64_t x);

#endif /* HL3_H */

