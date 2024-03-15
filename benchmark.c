#include "hl2.h"
#include "hl3.h"
#include "xxhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SEED 42
#define DEBUG

uint64_t generate_hash(uint64_t key) {
	return XXH64(&key, 8, SEED);
}


void benchmark_hl2(size_t sketch_size, size_t num_bits, uint64_t* hashes, size_t num_hashes) {
        HL2* hl2 = createHL2(sketch_size, num_bits);

#ifdef DEBUG
        printf("DEBUG: Création HL2 :\n");
        printf("DEBUG: Taille du sketch: %zu, Bits par cellule: %zu\n", sketch_size, num_bits);
        printf("DEBUG: Adresse de hl2: %p\n", (void*)hl2);
        if (hl2 != NULL) {
                printf("DEBUG: Contenu initial du sketch HL2 :\n");
                displayHL2(hl2);
        } else {
                printf("DEBUG: hl2 est NULL après création\n");
        }
#endif

        for (size_t i = 0; i < num_hashes; ++i) {
                uint64_t hash = generate_hash(hashes[i]);

#ifdef DEBUG
                printf("DEBUG: Insertion du hash %lu dans HL2\n", hash);
#endif

                insertHL2(hl2, hash);

#ifdef DEBUG
                printf("DEBUG: Contenu du sketch HL2 après insertion du hash %lu :\n", hash);
                displayHL2(hl2);
#endif
        }

        double cardinality = estimate_cardinality(&(hl2->commonHLL));
        printf("HL2 - Taille du sketch: %zu, Bits par cellule: %zu, Cardinalité estimée: %f\n", sketch_size, num_bits, cardinality);

#ifdef DEBUG
        printf("DEBUG: Contenu final du sketch HL2 :\n");
        displayHL2(hl2);
#endif

        destroyHL2(hl2);
}


size_t benchmark_hl3(size_t sketch_size, size_t num_bits, uint64_t* hashes, size_t num_hashes) {
	HL3* hl3 = createHL3(sketch_size, num_bits, num_bits);
	size_t overflow_count = 0;
	for (size_t i = 0; i < num_hashes; ++i) {
		uint64_t hash = generate_hash(hashes[i]);
		insertHL3(hl3, hash);
		if (hl3->errors_count > overflow_count) {
			overflow_count = hl3->errors_count;
		}
	}

	double cardinality = estimate_cardinality_hl3(hl3);
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
	size_t q = 5, nb_of_hashs = 100000;

	while ((opt = getopt(argc, argv, "p:q:n:")) != -1) {
		switch (opt) {
			case 'q':
				q = atoi(optarg);
				break;
			case 'n':
				nb_of_hashs = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s -q <q_value> -n <number_of_hashs>\n", argv[0]);
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
	size_t sketch_sizes[] = {5, 10, 15, 20};
	size_t num_bits_array[] = {3, 4, 5};

	for (size_t i = 0; i < sizeof(sketch_sizes) / sizeof(sketch_sizes[0]); i++) {
		size_t sketch_size = (1 << sketch_sizes[i]);
		printf("\nBenchmark pour sketch de taille %zu :\n", sketch_size);

		benchmark_hl2(sketch_size, q, hashes, nb_of_hashs);

		for (size_t j = 0; j < sizeof(num_bits_array) / sizeof(num_bits_array[0]); j++) {
			size_t num_bits = num_bits_array[j];
			size_t num_errors = benchmark_hl3(sketch_size, num_bits, hashes, nb_of_hashs);
			printf("Nombre total de dépassements de capacité pour hl3b%zu : %zu\n", num_bits, num_errors);
		}
	}

	free(hashes);
	return 0;
}



