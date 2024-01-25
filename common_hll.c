#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> // For INT_MAX
#include "xxhash.h"
#include "common_hll.h"

#define ALPHA_INF (1.0 / (2.0 * log(2.0)))
//#define DEBUG_ASM
#define DEBUG

size_t sizeOfHLL(HyperLogLog *hll) {
    size_t size = sizeof(*hll); // Taille de la structure HyperLogLog elle-même
    size += (sizeof(unsigned char) * (1 << hll->p)); // Taille du tableau de registres
    // Ajoutez ici la taille de tout autre composant dynamique
    return size;
}

void print_binary(uint64_t x) {
	for (int i = 63; i >= 0; i--) {
		uint64_t mask = 1ULL << i;
		uint64_t bit = (x & mask) >> i;
		putchar('0' + bit);
		if (i % 4 == 0 && i > 0) {
			putchar(' ');
		}
	}
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


// Ajouter un élément à l'HyperLogLog
void ajouter(HyperLogLog* hll, const void* element, size_t longueur) {
	unsigned long hash = XXH64(element, longueur, 666);
	size_t index = hash % (1 << hll->p);
	int rang = (hash != 0) ? asm_log2(hash) : 64; // 64 est le nombre de bits dans uint64_t
	if (hll->registers[index] < rang) {
		hll->registers[index] = rang;
	}
}

// Fusionner deux HyperLogLogs
void merge(HyperLogLog* dest, const HyperLogLog* src) {
	size_t size = (1 << dest->p);
//TODO #pragma omp parallel for default(none) shared(dest, src, size) private(i)
	for (size_t i = 0; i < size; i++) {
		if (src->registers[i] > dest->registers[i]) {
			dest->registers[i] = src->registers[i];
		}
	}
}


double estimate_cardinality(const HyperLogLog* hll) {
	double estimate = 0.0;
	double sum = 0.0;
	for (size_t i = 0; i < (1<<hll->p); i++) {
		sum += (double)1/(1<<hll->registers[i]);
	}

	// Utiliser la formule standard HyperLogLog
	double nb_cell_pow = (1<<(2*hll->p));//ALPHA_INF * (1 << (hll->p)) * (1 << (hll->p));
#ifdef DEBUG
	printf("nb_cell_pow : %f\n", nb_cell_pow);
#endif
	estimate = nb_cell_pow/sum;
#ifdef DEBUG
	printf("Estimation de la cardinalité : %f\n", estimate);
#endif

	return estimate/0.72134;
}

int main(int argc, char *argv[]) {
	return 0;
}


