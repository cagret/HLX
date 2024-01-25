#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> // For INT_MAX
#include "xxhash.h"
#include "hl3.h"
#include "common_hll.h"

HL3* createHL3(unsigned char p, unsigned char q) {
	HL3* hll = malloc(sizeof(HL3));
	if (!hll) return NULL;
	hll->commonHLL = createCommonHLL(p, q);
	hll->bitstream = createBitstream();
	hll->Offset = 0;
	hll->Z_occurence = 0;  // Initialiser en fonction de votre logique
	hll->max_value = 3;    // À ajuster selon vos besoins
	hll->hits = NULL;
	hll->hitsSize = 0;
	return hll;
}

void destroyHL3(HL3* hll) {
	destroyHLL(hll->commonHLL);
	destroyBitstream(hll->bitstream);
	free(hll->hits);
	free(hll);
}


/*
void get_hashes(HL3* hll, uint64_t key, uint minZeroes) {
    free(hll->hits);
    hll->hits = NULL;
    hll->hitsSize = 0;

    uint64_t seed = 666;
    uint32_t hash_id = 0;
    uint8_t remaining_bits = 63;
    XXH64_hash_t hash = XXH64(&key, sizeof(key), seed);
    uint8_t supplementaryzeroes = 0;

    while (hash_id < hll->M) {
        if (hash == 0) {
            supplementaryzeroes = remaining_bits + 1;
            remaining_bits = 63;
            hash = XXH64(&key, sizeof(key), ++seed);
        }

        int64_t rl = remaining_bits - asm_log2(hash) + supplementaryzeroes;
        supplementaryzeroes = 0;
        if (rl / 2 > minZeroes) {
            hll->hits = realloc(hll->hits, (hll->hitsSize + 1) * sizeof(*(hll->hits)));
            hll->hits[hll->hitsSize].zeroCount = rl / 2 - minZeroes;
            hll->hits[hll->hitsSize].index = hash_id;
            hll->hitsSize++;
        }
        remaining_bits -= (rl + 1);
        hash -= (uint64_t)1 << (remaining_bits + 1);
        hash_id++;
    }
}
*/
void insertHL3(CommonHLL* hll, Bitstream* bitstream) {
//TODO
}

