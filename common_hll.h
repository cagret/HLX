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


uint8_t leading_zeros(uint64_t x);
size_t sizeOfHLL(CommonHLL *hll);
uint64_t asm_log2(const uint64_t x);
double estimate_cardinality(const CommonHLL* hll);
void ajouter(CommonHLL* hll, const void* element, size_t longueur);
void merge(CommonHLL* dest, const CommonHLL* src);
int calculerRang(uint64_t hash);
double alpha(size_t m);
void handleOverflow(CommonHLL* hll,uint64_t value);

CommonHLL* createCommonHLL(unsigned char p, unsigned char q);
void destroyHLL(CommonHLL* hll);

#endif // COMMON_HLL_H

