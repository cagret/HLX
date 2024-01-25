#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <limits.h> // For INT_MAX
#include "xxhash.h"
#include "common_hll.h"
#include "hl2.h"


HL2* createHL2(unsigned char p, unsigned char q) {
	size_t size = (size_t)(1 << p);
	HL2* hll = malloc(sizeof(HL2));
	hll->commonHLL.p = p;
	hll->commonHLL.q = q;
	hll->commonHLL.registers = calloc(size, sizeof(unsigned char));
	hll->commonHLL.counts = calloc(q + 2, sizeof(int));
	hll->commonHLL.counts[0] = size;
	hll->commonHLL.minCount = 0;
	hll->commonHLL.registerValueFilter = ~((uint_fast64_t)0);
	return hll;
}

void destroyHL2(HL2* hll) {
	free(hll->commonHLL.registers);
	free(hll->commonHLL.counts);
	free(hll);
}

void insertHL2(CommonHLL* hll, Bitstream* bitstream) {
//TODO
}
