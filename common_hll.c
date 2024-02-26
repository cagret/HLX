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
//#define DEBUG

size_t sizeOfHLL(CommonHLL *hll) {
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

Bitstream* generateBitstreamForElement(const void* element, size_t length, int N) {
    // Génération du hash comme précédemment
    XXH64_hash_t hash = XXH64(element, length, 0);
    Bitstream* bitstream = createBitstream();

    if (!bitstream || !(bitstream->stream = malloc(sizeof(uint8_t) * 64))) {
        printf("Erreur : échec lors de la création ou l'allocation du Bitstream\n");
        destroyBitstream(bitstream);
        return NULL;
    }

#ifdef DEBUG
	printf("Génération du Bitstream pour le hash : %llu\n", (unsigned long long)hash);
#endif
    bitstream->size = 0;
    int zeroCount = 0;

    for (int i = 63; i >= 0; --i) {
        if (!(hash & (1ULL << i))) { zeroCount++; }
        else {
            // Ajouter au Bitstream si zeroCount est significatif par rapport à N
            if (zeroCount > N) {
                bitstream->stream[bitstream->size++] = zeroCount;
            }
#ifdef DEBUG
			printf("Bitstream [%zu] = %d\n", bitstream->size - 1, zeroCount);
#endif
            zeroCount = 0; // Réinitialisation pour la prochaine séquence
        }
    }

    // Ne pas oublier de vérifier après la boucle, au cas où le hash se termine par des zéros
    if (zeroCount > N) {
        bitstream->stream[bitstream->size++] = zeroCount;
    }
#ifdef DEBUG
	printf("Taille finale du Bitstream : %zu\n", bitstream->size);
#endif

    return bitstream;
}



// Création d'un Bitstream
Bitstream* createBitstream() {
	Bitstream* bitstream = malloc(sizeof(Bitstream));
	if (!bitstream) return NULL;
	bitstream->stream = NULL;
	bitstream->size = 0;
	return bitstream;
}

// Destruction d'un Bitstream
void destroyBitstream(Bitstream* bitstream) {
	if (bitstream) {
		free(bitstream->stream);
		free(bitstream);
	}
}

// Ajouter un élément à l'HyperLogLog
void ajouter(CommonHLL* hll, const void* element, size_t longueur) {
	unsigned long hash = XXH64(element, longueur, 666);
	size_t index = hash % (1 << hll->p);
	int rang = (hash != 0) ? asm_log2(hash) : 64; // 64 est le nombre de bits dans uint64_t
	if (hll->registers[index] < rang) {
		hll->registers[index] = rang;
	}
}

// Fusionner deux HyperLogLogs
void merge(CommonHLL* dest, const CommonHLL* src) {
	size_t size = (1 << dest->p);
	for (size_t i = 0; i < size; i++) {
		if (src->registers[i] > dest->registers[i]) {
			dest->registers[i] = src->registers[i];
		}
	}
}


double estimate_cardinality(const CommonHLL* hll) {
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


