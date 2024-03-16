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
    uint64_t offset;
    uint32_t z_occurence;
} HL3;


HL3* createHL3(unsigned char p, unsigned char q, unsigned char num_bits_per_counter);
void destroyHL3(HL3* hl3);
void displayHL3(HL3* hl3);
void insertHL3(HL3* hl3, uint64_t x);
void get_hashes(const uint64_t key, uint minZeroes);
uint8_t increment_offset(HL3* hl3, bool force);
void insert_key(uint64_t x);
void handleOverflowHL3(HL3* hl3);
int countOverestimatedCells(HL3* hll);

#endif /* HL3_H */

