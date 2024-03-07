#include "hl2.h"
#include "hl3.h"
#include "xxhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SEED 42
#define M 64
#define DEBUG

uint64_t generate_hash(uint64_t key) {
	return XXH64(&key, 8, SEED);
}

void benchmark_hl2(size_t sketch_size, size_t num_bits, uint64_t* hashes) {
	HL2* hl2 = createHL2(sketch_size, num_bits);
	for (size_t i = 0; i < sketch_size; ++i) {
		uint64_t hash = generate_hash(hashes[i]);
		insertHL2(hl2, hash);
	}
	double cardinality = estimate_cardinality(&(hl2->commonHLL));
	printf("HL2 - Taille du sketch: %zu, Bits par cellule: %zu, Cardinalité estimée: %f\n", sketch_size, num_bits, cardinality);

#ifdef DEBUG
	printf("DEBUG: Contenu du sketch HL2 :\n");
	displayHL2(hl2);
#endif

	destroyHL2(hl2);
}

size_t benchmark_hl3(size_t sketch_size, size_t num_bits, uint64_t* hashes) {
	HL3* hl3 = createHL3(6, num_bits, sketch_size);
	size_t overflow_count = 0;
	for (size_t i = 0; i < sketch_size; ++i) {
		uint64_t hash = generate_hash(hashes[i]);
		uint8_t rho = asm_log2(hash << (hl3->commonHLL.p));
		uint64_t MAX_REGISTER_VALUE = (1 << hl3->num_bits_per_counter) - 1;

#ifdef DEBUG
		printf("DEBUG: Hash pour l'élément %zu : %lu\n", i, hash);
		printf("DEBUG: Valeur de rho pour l'élément %zu : %hhu\n", i, rho);
		printf("DEBUG: Valeur MAX_REGISTER_VALUE : %lu\n", MAX_REGISTER_VALUE);
#endif

		if (rho >= MAX_REGISTER_VALUE) {
			overflow_count++;
#ifdef DEBUG
			printf("Overflow detected at index %zu with hash value %lu\n", i, hash);
#endif
			handleOverflow(&(hl3->commonHLL), rho);
		}
		insertHL3(hl3, hash);
	}

	double cardinality = estimate_cardinality(&(hl3->commonHLL));
	printf("HL3 - Taille du sketch: %zu, Bits par cellule: %zu, Cardinalité estimée: %f\n", sketch_size, num_bits, cardinality);
	printf("Nombre de dépassements de capacité: %zu\n", overflow_count);

#ifdef DEBUG
	printf("DEBUG: Contenu du sketch HL3 :\n");
	displayHL3(hl3);
#endif

	destroyHL3(hl3);
	return overflow_count;
}

int main(int argc, char *argv[]) {
	int opt;
	size_t p = 6, q = 5, nb_of_hashs = 100000;

	while ((opt = getopt(argc, argv, "p:q:n:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi(optarg);
				break;
			case 'q':
				q = atoi(optarg);
				break;
			case 'n':
				nb_of_hashs = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s -p <p_value> -q <q_value> -n <number_of_hashs>\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	srand((unsigned int)time(NULL));
	uint64_t* hashes = malloc(nb_of_hashs * sizeof(uint64_t));
	for (uint32_t i = 0; i < nb_of_hashs; i++) {
		hashes[i] = rand();
	}

#ifdef DEBUG
	printf("Nombre total de hash générés : %lu\n", nb_of_hashs);
#endif
	size_t sketch_size = (size_t)p;
	//size_t num_bits = (size_t)q;
	size_t num_bits[] = {3, 4, 5};
	size_t num_errors = 0;

	printf("\nBenchmark pour sketch de taille %zu :\n", sketch_size);
	benchmark_hl2(sketch_size, q, hashes);
	for (int i = 0; i < sizeof(num_bits) / sizeof(num_bits[0]); i++) {
		num_errors = benchmark_hl3(sketch_size, num_bits[i], hashes);
		printf("Nombre total de dépassements de capacité pour hl3b%zu : %zu\n", num_bits[i], num_errors);
	}
	free(hashes);
	return 0;
}

