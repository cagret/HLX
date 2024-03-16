#include "hl3.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif


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
 * Handles overflow in an HL3 structure.
 *
 * @param hl3 A pointer to the HL3 structure.
 */
void handleOverflowHL3(HL3* hl3) {
	if (hl3->commonHLL.registers[0] > (1 << hl3->num_bits_per_counter) - 1) {
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
}


/**
 * Inserts a hash value into an HL3 structure.
 *
 * @param hl3 A pointer to the HL3 structure.
 * @param x The hash value to insert.
 */
void insertHL3(HL3* hl3, uint64_t hash) {
	size_t index = hash % (1 << hl3->commonHLL.p);
	//uint32_t index = x >> (64 - hl3->commonHLL.p);
	//index &= (1 << hl3->commonHLL.p) - 1;
	int rang = (hash != 0) ? asm_log2(hash) : 64;
	if (rang < (1 << hl3->num_bits_per_counter) - 1) {
		if (hl3->commonHLL.registers[index] < rang) {
			hl3->commonHLL.registers[index] = rang;
		}
	} else {
		hl3->errors_count++;
		hl3->super_counter++;
	}
}

