#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "xxhash.h"
#include "common_hll.h"
#include "hl2.h"
#include "hl3.h"
//#define DEBUG

// Fonction pour générer des données aléatoires pour le benchmark
void generate_data(uint64_t* data, size_t num_elements) {
    for (size_t i = 0; i < num_elements; ++i) {
        data[i] = rand();
    }
}

// Fonction pour effectuer le benchmark pour HL2
void benchmark_hl2(size_t sketch_size, size_t num_bits) {
    HL2* hl2 = createHL2(6, num_bits); // p = 6 pour HL2
    uint64_t* data = malloc(sketch_size * sizeof(uint64_t));
    generate_data(data, sketch_size);

    // Insérer les données dans HL2
    for (size_t i = 0; i < sketch_size; ++i) {
        insertHL2(hl2, data[i]);
    }

    // Estimer la cardinalité de l'ensemble
    double cardinality = estimate_cardinality(&(hl2->commonHLL));
    printf("HL2 - Taille du sketch: %zu, Bits par cellule: %zu, Cardinalité estimée: %f\n", sketch_size, num_bits, cardinality);
    destroyHL2(hl2);
    free(data);
#ifdef DEBUG
    printf("DEBUG: Tout a été destroy\n");
#endif
}

// Fonction pour effectuer le benchmark pour HL3
void benchmark_hl3(size_t sketch_size, size_t num_bits) {
    HL3* hl3 = createHL3(6, num_bits, sketch_size); // p = 6 pour HL3
    uint64_t* data = malloc(sketch_size * sizeof(uint64_t));
    generate_data(data, sketch_size);

    // Insérer les données dans HL3
    for (size_t i = 0; i < sketch_size; ++i) {
        insertHL3(hl3, data[i]);
    }

    // Estimer la cardinalité de l'ensemble
    double cardinality = estimate_cardinality(&(hl3->commonHLL));
    printf("HL3 - Taille du sketch: %zu, Bits par cellule: %zu, Cardinalité estimée: %f\n", sketch_size, num_bits, cardinality);

    destroyHL3(hl3);
    free(data);
}

int main() {
    srand((unsigned int)time(NULL));

    // Définir les tailles de sketch et les nombres de bits par cellule à tester
    size_t sketch_sizes[] = {5, 10, 15, 20};
    size_t num_bits[] = {3, 4, 5};

    // Effectuer le benchmark pour HL2 et HL3 avec différentes configurations
    for (size_t i = 0; i < sizeof(sketch_sizes) / sizeof(sketch_sizes[0]); ++i) {
        for (size_t j = 0; j < sizeof(num_bits) / sizeof(num_bits[0]); ++j) {
            printf("\nBenchmark pour sketch de taille %zu et %zu bits par cellule :\n", sketch_sizes[i], num_bits[j]);
            benchmark_hl2(sketch_sizes[i], num_bits[j]);
            benchmark_hl3(sketch_sizes[i], num_bits[j]);
        }
    }

    return 0;
}
