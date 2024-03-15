#include "xxhash.h"
#include "hl2.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> 
#include <string.h> 

#define DEBUG

HL2* createHL2(unsigned char p, unsigned char q) {
    HL2* hll = malloc(sizeof(HL2));
    if (hll == NULL) {
        perror("Erreur lors de l'allocation de mémoire pour HL2");
        exit(EXIT_FAILURE);
    }
    hll->commonHLL = *createCommonHLL(p, q);
    return hll;
}

void destroyHL2(HL2* hll) {
    destroyHLL(&(hll->commonHLL));
}

void displayHL2(HL2* hl2) {
    printf("Offset: %d\n", hl2->commonHLL.q);
    uint8_t maximum = 0;
    for (size_t lih = 0; lih < (1 << hl2->commonHLL.p); ++lih) {
        printf(" %d", hl2->commonHLL.registers[lih]);
        if (maximum < hl2->commonHLL.registers[lih]) {
            maximum = hl2->commonHLL.registers[lih];
        }
    }
    printf(" MAX: %d\n", maximum);
}


void insertHL2(HL2* hll, uint64_t hash_value) {
    uint32_t index = hash_value >> (64 - hll->commonHLL.p);
    index = index & ((1 << hll->commonHLL.p) - 1);

#ifdef DEBUG
    printf("DEBUG: hash_value: %lu\n", hash_value);
    printf("DEBUG: index: %u\n", index);
#endif

    uint8_t rho = asm_log2(hash_value << (hll->commonHLL.p));

#ifdef DEBUG
    printf("DEBUG: rho: %u\n", rho);
#endif

    uint8_t currentValue = hll->commonHLL.registers[index];

    if (rho > currentValue) {
        hll->commonHLL.registers[index] = rho;
    } else if (rho == currentValue && rho != 0) {
        CommonHLL src;
        src.p = hll->commonHLL.p;
        src.registers = calloc((1 << src.p), sizeof(uint8_t));
        src.registers[index] = rho;

        merge(&(hll->commonHLL), &src);
        free(src.registers);
    }
}

