#include "xxhash.h"
#include "hl2.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> 


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

void insertHL2(HL2* hll, uint64_t hash_value) {
    uint32_t index = hash_value >> (64 - hll->commonHLL.p);
    uint8_t rho = leading_zeros(hash_value << (hll->commonHLL.p));

    if (rho > hll->commonHLL.registers[index]) {
        hll->commonHLL.registers[index] = rho;
    }
}

