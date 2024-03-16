#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> 
#include "xxhash.h"
#include "common_hll.h"

//#define DEBUG
/**
 * Creates a new CommonHLL structure.
 *
 * @param p The precision parameter.
 * @param q The number of bits per register.
 * @return A pointer to the newly created CommonHLL structure.
 */
CommonHLL* createCommonHLL(unsigned char p, unsigned char q) {
    size_t size = (size_t)(1 << p);
    CommonHLL* hll = malloc(sizeof(CommonHLL));
    if (hll == NULL) {
        perror("Error allocating memory for CommonHLL");
        exit(EXIT_FAILURE);
    }
    hll->p = p;
    hll->q = q;
    hll->registers = calloc(size, sizeof(unsigned char));
    hll->counts = calloc(q + 2, sizeof(int));
    if (hll->registers == NULL || hll->counts == NULL) {
        perror("Error allocating memory for CommonHLL registers or counts");
        exit(EXIT_FAILURE);
    }
    hll->counts[0] = size;
    hll->minCount = 0;
    hll->registerValueFilter = ~((uint_fast64_t)0);
    return hll;
}

/**
 * Finds the position of the highest set bit in a 64-bit unsigned integer, essentially calculating log2(x).
 *
 * This function quickly figures out how many times you can 'shift' the number x to the left before its highest
 * set bit becomes the leftmost bit in the number. This count is the log2 of x. It's a handy trick for performance-sensitive code.
 *
 * Special case: If x is 0, the function returns 64. This is because log2(0) is undefined, but here, we use 64 to indicate the special case.
 *
 * @param x The 64-bit number to find the log2 of. If x is 0, returns 64 as a special handling case.
 * @return  The log2 of x, measured by the shift count needed to make the highest set bit the leftmost bit. Returns 64 for x = 0.
 */
uint64_t asm_log2(const uint64_t x) {
        uint64_t y;
        asm("\tbsr %1, %0\n" : "=r"(y) : "r"(x));
        return 63-y;
}

/**
 * Destroys a CommonHLL structure and frees its memory.
 *
 * @param hll A pointer to the CommonHLL structure to destroy.
 */
void destroyHLL(CommonHLL* hll) {
    if (hll != NULL) {
#ifdef DEBUG
        printf("DestroyHLL registers: ");
        for (size_t i = 0; i < (1 << hll->p); i++) {
            printf("%d ", hll->registers[i]);
        }
        printf("\n");

        printf("DestroyHLL counts: ");
        for (size_t i = 0; i < hll->q + 2; i++) {
            printf("%d ", hll->counts[i]);
        }
        printf("\n");
#endif

        free(hll->registers);
        free(hll->counts);
    }
}


/**
 * Merges two CommonHLL structures.
 *
 * @param dest A pointer to the destination CommonHLL structure.
 * @param src A pointer to the source CommonHLL structure.
 */
void merge(CommonHLL* dest, const CommonHLL* src) {
    size_t size = (1 << dest->p);
    for (size_t i = 0; i < size; i++) {
        if (src->registers[i] > dest->registers[i]) {
            dest->registers[i] = src->registers[i];
        }
    }
}


/**
 * Estimates the cardinality of a CommonHLL structure.
 *
 * This function calculates the estimated cardinality of a set represented by a CommonHLL structure.
 * It uses the harmonic mean of the inverse powers of 2 of the register values to estimate the cardinality.
 *
 * @param hll A pointer to the CommonHLL structure.
 * @return The estimated cardinality of the set.
 */
double estimate_cardinality(const CommonHLL* hll) {
    double estimate = 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < (1 << hll->p); i++) {
        sum += (double)1 / (1 << hll->registers[i]);
    }

    double nb_cell_pow = (1 << (2 * hll->p));
    estimate = nb_cell_pow / sum;

    return estimate / 0.72134;
}
