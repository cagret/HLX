// common_hll.h
#ifndef COMMON_HLL_H
#define COMMON_HLL_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t *stream;
    size_t size;
} Bitstream;

typedef struct {
    unsigned char p;
    unsigned char q;
    unsigned char* registers;
    int* counts;
    unsigned char minCount;
    uint_fast64_t registerValueFilter;
    Bitstream bitstream; 
} CommonHLL;

Bitstream* createBitstream();
Bitstream* generateBitstreamFromHash(uint64_t hash);
Bitstream* generateBitstreamForElement(const void* element, size_t length);
void destroyBitstream(Bitstream* bitstream);
void updateBitstream(Bitstream* bitstream, uint64_t hash);

size_t sizeOfHLL(CommonHLL *hll);
void print_binary(uint64_t x);
uint64_t asm_log2(const uint64_t x);
double estimate_cardinality(const CommonHLL* hll);
void ajouter(CommonHLL* hll, const void* element, size_t longueur);
void merge(CommonHLL* dest, const CommonHLL* src);
int calculerRang(uint64_t hash);

CommonHLL* createCommonHLL(unsigned char p, unsigned char q);
void destroyHLL(CommonHLL* hll);

#endif // COMMON_HLL_H

