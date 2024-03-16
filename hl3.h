#ifndef HL3_H
#define HL3_H

#include "common_hll.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    CommonHLL commonHLL;
    unsigned char num_bits_per_counter;
    uint64_t errors_count;
    uint64_t min_count;
    uint64_t max_register_value;
    unsigned char super_counter;
} HL3;


HL3* createHL3(unsigned char p, unsigned char q, unsigned char num_bits_per_counter);
void destroyHL3(HL3* hl3);
void insertHL3(HL3* hl3, uint64_t x);
void handleOverflowHL3(HL3* hl3);
int countOverestimatedCells(HL3* hll);

#endif /* HL3_H */

