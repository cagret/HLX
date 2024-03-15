#include "hl2.h"
#include "hl3.h"
#include "xxhash.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SEED 42
#define DEBUG

/**
 * Generates a hash value for a given key.
 *
 * @param key The key to hash.
 * @return The generated hash value.
 */
uint64_t generate_hash(uint64_t key) {
    return XXH64(&key, 8, SEED);
}

/**
 * Benchmarks the HL2 algorithm.
 *
 * @param sketch_size The size of the sketch.
 * @param num_bits The number of bits per register.
 * @param hashes The array of hash values.
 * @param num_hashes The number of hash values.
 */
void benchmark_hl2(size_t sketch_size, size_t num_bits, uint64_t* hashes, size_t num_hashes) {
    clock_t start = clock();

    HL2* hl2 = createHL2(sketch_size, num_bits);
    for (size_t i = 0; i < num_hashes; ++i) {
        uint64_t hash = generate_hash(hashes[i]);
        insertHL2(hl2, hash);
    }
    double cardinality = estimate_cardinality(&(hl2->commonHLL));

    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("HL2 - Sketch size: %zu, Bits per register: %zu, Estimated cardinality: %f, Time: %f seconds\n",
           sketch_size, num_bits, cardinality, elapsed_time);

#ifdef DEBUG
    printf("DEBUG: HL2 sketch content:\n");
    displayHL2(hl2);
#endif

    destroyHL2(hl2);
    hl2 = NULL;
}

/**
 * Benchmarks the HL3 algorithm.
 *
 * @param sketch_size The size of the sketch.
 * @param num_bits The number of bits per register.
 * @param hashes The array of hash values.
 * @param num_hashes The number of hash values.
 * @return The number of overflows.
 */
size_t benchmark_hl3(size_t sketch_size, size_t num_bits, uint64_t* hashes, size_t num_hashes) {
    clock_t start = clock();

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

    clock_t end = clock();
    double elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("HL3 - Sketch size: %zu, Bits per register: %zu, Estimated cardinality: %f, Time: %f seconds\n",
           sketch_size, num_bits, cardinality, elapsed_time);
    printf("Number of overflows: %zu\n", overflow_count);

#ifdef DEBUG
    printf("DEBUG: HL3 sketch content:\n");
    displayHL3(hl3);
#endif

    destroyHL3(hl3);
    hl3 = NULL;
    return overflow_count;
}

/**
 * Main function to run the benchmarks.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return The exit status of the program.
 */
int main(int argc, char *argv[]) {
    int opt;
    size_t q = 5, nb_of_hashes = 100000;

    while ((opt = getopt(argc, argv, "q:n:")) != -1) {
        switch (opt) {
            case 'q':
                q = atoi(optarg);
                break;
            case 'n':
                nb_of_hashes = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -q <q_value> -n <number_of_hashes>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    srand((unsigned int)time(NULL));
    uint64_t* hashes = malloc(nb_of_hashes * sizeof(uint64_t));
    if (hashes == NULL) {
        perror("Error allocating memory for hashes");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < nb_of_hashes; i++) {
        hashes[i] = rand();
    }

#ifdef DEBUG
    printf("Total number of generated hashes: %lu\n", nb_of_hashes);
#endif

    size_t sketch_sizes[] = {5, 10, 15, 20};
    size_t num_bits_array[] = {3, 4, 5};

    for (size_t i = 0; i < sizeof(sketch_sizes) / sizeof(sketch_sizes[0]); i++) {
        size_t sketch_size = (1 << sketch_sizes[i]);
        printf("\nBenchmark for sketch size %zu:\n", sketch_size);

        benchmark_hl2(sketch_size, q, hashes, nb_of_hashes);

        for (size_t j = 0; j < sizeof(num_bits_array) / sizeof(num_bits_array[0]); j++) {
            size_t num_bits = num_bits_array[j];
            size_t num_errors = benchmark_hl3(sketch_size, num_bits, hashes, nb_of_hashes);
            printf("Total number of overflows for hl3b%zu: %zu\n", num_bits, num_errors);
        }
    }

    free(hashes);
    return 0;
}
