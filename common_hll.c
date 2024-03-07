#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> 
#include "xxhash.h"
#include "common_hll.h"

size_t sizeOfHLL(CommonHLL *hll) {
    size_t size = sizeof(*hll); 
    size += (sizeof(unsigned char) * (1 << hll->p)); 
    return size;
}

uint64_t asm_log2(const uint64_t x) {
    if (x == 0) return 64;
    uint64_t tmp = x;
    int n = 0;
    while ((tmp & (1ULL << 63)) == 0) {
        n = n + 1;
        tmp = tmp << 1;
    }
#ifdef DEBUG_ASM
    printf("Nombre de zéros de tête pour %d: \n", n);
    print_binary(x);
    printf("\n");
#endif
    return n;
}

double alpha(size_t m) {
    double alpha;
    switch (m) {
        case 16:
            alpha = 0.673;
            break;
        case 32:
            alpha = 0.697;
            break;
        case 64:
            alpha = 0.709;
            break;
        default:
            alpha = 0.7213 / (1.0 + 1.079 / m);
            break;
    }
    return alpha;
}

double estimate_cardinality(const CommonHLL* hll) {
    double estimate = 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < (1 << hll->p); i++) {
        sum += (double)1 / (1 << hll->registers[i]);
    }

    // Utiliser la formule standard HyperLogLog
    double nb_cell_pow = (1 << (2 * hll->p)); // ALPHA_INF * (1 << (hll->p)) * (1 << (hll->p));
#ifdef DEBUG
    printf("nb_cell_pow : %f\n", nb_cell_pow);
#endif
    estimate = nb_cell_pow / sum;
#ifdef DEBUG
    printf("Estimation de la cardinalité : %f\n", estimate);
#endif

    return estimate / 0.72134;
}

void ajouter(CommonHLL* hll, const void* element, size_t longueur) {
    unsigned long hash = XXH64(element, longueur, 666);
    size_t index = hash % (1 << hll->p);
    int rang = (hash != 0) ? asm_log2(hash) : 64; // 64 est le nombre de bits dans uint64_t
    if (hll->registers[index] < rang) {
        hll->registers[index] = rang;
    }
}

uint8_t leading_zeros(uint64_t x) {
    uint8_t count = 0;
    for (int i = 63; i >= 0; --i) {
        if ((x & (1ULL << i)) == 0) {
            count++;
        } else {
            break;
        }
    }
    return count;
}


void handleOverflow(CommonHLL* hll,uint64_t value) {
    uint64_t maxValueAllowed = (1 << hll->q) - 1; 
    if (value > maxValueAllowed) {
        value = maxValueAllowed;
    }

    for (size_t i = 0; i < (1 << hll->p); i++) {
        if (hll->registers[i] == 0) {
            hll->registers[i] = value;
        }
    }
}


void merge(CommonHLL* dest, const CommonHLL* src) {
    size_t size = (1 << dest->p);
    for (size_t i = 0; i < size; i++) {
        if (src->registers[i] > dest->registers[i]) {
            dest->registers[i] = src->registers[i];
        }
    }
}

CommonHLL* createCommonHLL(unsigned char p, unsigned char q) {
    size_t size = (size_t)(1 << p);
    CommonHLL* hll = malloc(sizeof(CommonHLL));
    hll->p = p;
    hll->q = q;
    hll->registers = calloc(size, sizeof(unsigned char));
    hll->counts = calloc(q + 2, sizeof(int));
    hll->counts[0] = size;
    hll->minCount = 0;
    hll->registerValueFilter = ~((uint_fast64_t)0);
    return hll;
}

void destroyHLL(CommonHLL* hll) {
    free(hll->registers);
    free(hll->counts);
    free(hll);
}

