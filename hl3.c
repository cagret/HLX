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


struct {
        int8_t zeroCount;
        uint32_t index;
} *hits;

HL3* createHL3(unsigned char p, unsigned char q) {
	size_t size = (size_t)(1 << p);

	HL3* hll = malloc(sizeof(HL3));
	if (!hll) {
		return NULL;
	}
	hll->commonHLL.p = p;
	hll->commonHLL.q = q;
	hll->commonHLL.registers = calloc(size, sizeof(unsigned char));
	hll->commonHLL.counts = calloc(q + 2, sizeof(int));
	hll->commonHLL.counts[0] = size;
	hll->commonHLL.minCount = 0;
	hll->commonHLL.registerValueFilter = ~((uint_fast64_t)0);
	hll->bitstream = NULL;
	hll->M = 0;
	hll->Offset = 0;
	hll->max_value = 0;
	hll->Z_occurence = 0;
	hll->hitsSize = 0;

	return hll;
}

void destroyHL3(HL3* hll) {
	if (!hll) return;
	free(hll->commonHLL.registers);
	free(hll->commonHLL.counts);
	free(hll);
}
void insertHL3(CommonHLL* hll, Bitstream* bitstream) {
	if (!hll || !bitstream) return;
	//TODO
}
