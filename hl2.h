// hl2.h

#ifndef HL2_H
#define HL2_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    unsigned char p;
    unsigned char q;
    unsigned char* registers;
    int* counts;
    unsigned char minCount;
    uint_fast64_t registerValueFilter;
} HyperLogLog;

size_t sizeOfHLL(HyperLogLog *hll);
HyperLogLog* createHyperLogLog2(unsigned char p, unsigned char q);
uint64_t asm_log2(const uint64_t x);
double estimate_cardinality(const HyperLogLog* hll);
void destroyHyperLogLog(HyperLogLog* hll);
void ajouter(HyperLogLog* hll, const void* element, size_t longueur);
void merge(HyperLogLog* dest, const HyperLogLog* src);
int calculerRang(uint64_t hash);

#endif // HL2_H


