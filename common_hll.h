// common_hll.h
#ifndef COMMON_HLL_H
#define COMMON_HLL_H

#include <stdlib.h>
#include <stdint.h>


typedef struct {
    unsigned char p;
    unsigned char q;
    unsigned char* registers;
    int* counts;
    unsigned char minCount;
    uint_fast64_t registerValueFilter;
} CommonHLL;

size_t sizeOfHLL(HyperLogLog *hll);
uint64_t asm_log2(const uint64_t x);
double estimate_cardinality(const HyperLogLog* hll);
void ajouter(HyperLogLog* hll, const void* element, size_t longueur);
void merge(HyperLogLog* dest, const HyperLogLog* src);
int calculerRang(uint64_t hash);


#endif // COMMON_HLL_H

