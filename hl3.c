#include "hl3.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

HL3* createHL3(unsigned char p, unsigned char q, unsigned char num_bits_per_counter) {
    HL3* hl3 = malloc(sizeof(HL3));
    if (hl3 == NULL) {
        perror("Erreur lors de l'allocation de mémoire pour HL3");
        exit(EXIT_FAILURE);
    }
    hl3->commonHLL = *createCommonHLL(p, q);
    hl3->num_bits_per_counter = num_bits_per_counter;
    hl3->errors_count = 0;
    return hl3;
}

void destroyHL3(HL3* hl3) {
    destroyHLL(&(hl3->commonHLL));
}

void displayHL3(HL3* hl3) {
    printf("Offset: %d", hl3->commonHLL.q);
    uint8_t maximum = 0;
    for (size_t lih = 0; lih < (1 << hl3->commonHLL.p); ++lih) {
        printf(" %d", hl3->commonHLL.registers[lih]);
        if (maximum < hl3->commonHLL.registers[lih]) {
            maximum = hl3->commonHLL.registers[lih];
        }
    }
    printf(" MAX: %d\n", maximum);
}


void insertHL3(HL3* hl3, uint64_t x) {
    uint32_t index = x >> (64 - hl3->commonHLL.p);
    uint8_t rho = asm_log2(x << (hl3->commonHLL.p));
    uint64_t MAX_REGISTER_VALUE = (1 << hl3->num_bits_per_counter) - 1;

    if (rho > hl3->commonHLL.registers[index]) {
        if (rho >= MAX_REGISTER_VALUE) {
            rho = MAX_REGISTER_VALUE - 1;
	    hl3->errors_count++;
        }
        hl3->commonHLL.registers[index] = rho;
    }
}
