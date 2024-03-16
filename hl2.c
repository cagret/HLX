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


/**
 * Destroys an HL2 structure and frees its memory.
 *
 * @param hl2 A pointer to the HL2 structure to destroy.
 */
void destroyHL2(HL2* hl2) {
    if (hl2 != NULL) {
        printf("DESTROY HL2");
	destroyHLL(&(hl2->commonHLL));
        free(hl2);
    }
}


void insertHL2(HL2* hll, uint64_t hash) {
    size_t index = hash % (1 << hll->commonHLL.p);
    int rang = (hash != 0) ? asm_log2(hash) : 64;
        if (hll->commonHLL.registers[index] < rang) {
                hll->commonHLL.registers[index] = rang;
        }
}

