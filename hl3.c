#include "hl3.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**
 * Creates a new HL3 structure.
 *
 * @param p The precision parameter.
 * @param q The number of bits per register.
 * @param num_bits_per_counter The number of bits per counter.
 * @return A pointer to the newly created HL3 structure.
 */
HL3* createHL3(unsigned char p, unsigned char q, unsigned char num_bits_per_counter) {
    HL3* hl3 = malloc(sizeof(HL3));
    if (hl3 == NULL) {
        perror("Error allocating memory for HL3");
        exit(EXIT_FAILURE);
    }
    hl3->commonHLL = *createCommonHLL(p, q);
    hl3->num_bits_per_counter = num_bits_per_counter;
    hl3->max_register_value = (1 << num_bits_per_counter) - 1;
    hl3->errors_count = 0;
    hl3->min_count = 0;
    hl3->offset = 0;
    hl3->z_occurence = 0;

    return hl3;
}

/**
 * Destroys an HL3 structure and frees its memory.
 *
 * @param hl3 A pointer to the HL3 structure to destroy.
 */
void destroyHL3(HL3* hl3) {
    if (hl3 != NULL) {
        printf("DESTROY HL3");
    	destroyHLL(&(hl3->commonHLL));
	free(hl3);
    }
}

/**
 * Displays the content of an HL3 structure.
 *
 * @param hl3 A pointer to the HL3 structure to display.
 */
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

/**
 * Handles overflow in an HL3 structure.
 *
 * @param hl3 A pointer to the HL3 structure.
 */
void handleOverflowHL3(HL3* hl3) {
    hl3->min_count++;
    hl3->errors_count += hl3->commonHLL.counts[0];

    for (size_t i = 0; i < (1 << hl3->commonHLL.p); i++) {
        if (hl3->commonHLL.registers[i] > 0) {
            hl3->commonHLL.registers[i]--;
        }
    }

    for (size_t i = 0; i <= hl3->commonHLL.q; i++) {
        hl3->commonHLL.counts[i] = 0;
    }
    for (size_t i = 0; i < (1 << hl3->commonHLL.p); i++) {
        hl3->commonHLL.counts[hl3->commonHLL.registers[i]]++;
    }
}

uint8_t increment_offset(HL3* hl3, bool force) {
    uint8_t result = 0;
    if (force) {
        hl3->offset++;
        result++;
        hl3->z_occurence = 0;
        for (uint32_t i = 0; i < (1 << hl3->commonHLL.p); ++i) {
            if (hl3->commonHLL.registers[i] != 0) {
                hl3->commonHLL.registers[i]--;
            }
            if (hl3->commonHLL.registers[i] == 0) {
                hl3->z_occurence++;
            }
        }
    }
    while (hl3->z_occurence == 0) {
        hl3->offset++;
        result++;
        for (uint32_t i = 0; i < (1 << hl3->commonHLL.p); ++i) {
            hl3->commonHLL.registers[i]--;
            if (hl3->commonHLL.registers[i] == 0) {
                hl3->z_occurence++;
            }
        }
    }
    return result;
}

/**
 * Inserts a hash value into an HL3 structure.
 *
 * @param hl3 A pointer to the HL3 structure.
 * @param x The hash value to insert.
 */
void insertHL3(HL3* hl3, uint64_t x) {
    uint32_t index = x >> (64 - hl3->commonHLL.p);
    index &= (1 << hl3->commonHLL.p) - 1;
    int rang = (x != 0) ? asm_log2(x) : hl3->num_bits_per_counter;
    if (rang >= hl3->num_bits_per_counter) {
        rang = hl3->num_bits_per_counter - 1;
    }
    unsigned char current_value = hl3->commonHLL.registers[index];
    unsigned char new_value = (1 << rang) - 1;
    if (new_value > current_value) {
        if (current_value > 0) {
            hl3->commonHLL.counts[current_value]--;
        }
        hl3->commonHLL.registers[index] = new_value;
        hl3->commonHLL.counts[new_value]++;
    } else {
        while (new_value > hl3->max_register_value) {
            increment_offset(hl3, true);
            new_value -= increment_offset(hl3, true);
        }
    }
}

/**
 * Counts the number of overestimated cells in an HL3 structure.
 *
 * This function counts the number of cells in an HL3 structure that have a value
 * greater than the maximum register value allowed by the number of bits per counter.
 * These cells are considered overestimated.
 *
 * @param hll A pointer to the HL3 structure.
 * @return The number of overestimated cells in the HL3 structure.
 */
int countOverestimatedCells(HL3* hll) {
    int overestimated_cells = 0;
    int max_register_value = (1 << hll->num_bits_per_counter) - 1;
    int m = 1 << hll->commonHLL.p;

    for (int i = 0; i < m; i++) {
        if (hll->commonHLL.registers[i] > max_register_value) {
            overestimated_cells++;
        }
    }

    return overestimated_cells;
}
