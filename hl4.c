#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> // For INT_MAX
#include "xxhash.h"
#include "hl4.h"
#include "common_hll.h"

HL4* createHL4(unsigned char p, unsigned char q) {
	size_t size = (size_t)(1 << p);

	size_t nbCompteurs = 1 << p; // 2^p compteurs AUCUNE IDEE ici
	size_t nbSuperCompteurs = nbCompteurs / 16; //16 compteurs par super compteur

	HL4* hll = malloc(sizeof(HL4));
	hll->commonHLL.p = p;
	hll->commonHLL.q = q;
	hll->commonHLL.registers = calloc(size, sizeof(unsigned char));
	hll->commonHLL.counts = calloc(q + 2, sizeof(int));
	hll->commonHLL.counts[0] = size;
	hll->commonHLL.minCount = 0;
	hll->commonHLL.registerValueFilter = ~((uint_fast64_t)0);

	hll->compteurs = calloc(nbCompteurs, sizeof(uint8_t));
	hll->superCompteurs = calloc(nbSuperCompteurs, sizeof(uint8_t));

	hll->megaCompteur = 0; 

	return hll;
}

void destroyHL4(HL4* hll) {
	free(hll->commonHLL.registers);
	free(hll->commonHLL.counts);
	free(hll->compteurs);
	free(hll->superCompteurs);
	free(hll);
}

uint16_t calculate_counter_index(int zero_count, uint16_t num_counters_per_super) {
	int k = 0;
	while (num_counters_per_super >> k > zero_count) {
		++k;
	}
	return k % num_counters_per_super;
}




void insertHL4(HL4* hll, Bitstream* bitstream) {
	size_t num_counters_per_super = 16; // Since you're using 16 counters per super counter

	for (size_t i = 0; i < bitstream->size; ++i) {
		int zero_count = bitstream->stream[i];
		size_t counter_index = calculate_counter_index(zero_count, num_counters_per_super);

		if (counter_index >= (1 << hll->commonHLL.p)) {
			continue; // Skip if counter_index is out of bounds
		}

		if (hll->compteurs[counter_index] < zero_count) {
			hll->compteurs[counter_index] = zero_count;
		}

		// Calculate the index of the super counter
		size_t super_index = counter_index / num_counters_per_super;
		if (super_index < (1 << (hll->commonHLL.p - 4))) { // Assuming 16 counters per super counter
								   // Update the super counter if needed
			if (hll->superCompteurs[super_index] < zero_count) {
				hll->superCompteurs[super_index] = zero_count;
			}
		}
	}

}

