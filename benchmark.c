#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include "hl2.h"
#include "hl3.h"
#include "xxhash.h"

//#define DEBUG

#ifdef DEBUG
#define PRINT_DEBUG(...) printf(__VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif

uint64_t* generateRandomHashes(int num_hashes) {
	uint64_t* hashes = (uint64_t*)malloc(num_hashes * sizeof(uint64_t));
	if (hashes == NULL) {
		perror("Failed to allocate memory for hashes");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < num_hashes; i++) {
		hashes[i] = XXH64(&i, sizeof(int), 0);
	}
	return hashes;
}



void printBinHash(uint64_t hash) {
	for (int j = 63; j >= 0; j--) {
		printf("%lu", (hash >> j) & 1);
		if (j % 8 == 0 && j != 0) {
			printf(" "); // Pour la lisibilité, ajoute un espace tous les 8 bits
		}
	}
	uint64_t leading_zeros = asm_log2(hash);
	printf(" | Leading zeros: %lu\n", leading_zeros);
}

void printHashes(uint64_t* hashes, int num_hashes) {
	for (int i = 0; i < num_hashes; i++) {
		printf("Hash %d: ", i);
		printBinHash(hashes[i]);
	}
}


int main(int argc, char *argv[]) {
	int p = 10; 
	int q = 8;  
	int num_hashes = 1000000;
	int option;

	while ((option = getopt(argc, argv, "p:q:n:")) != -1) {
		switch (option) {
			case 'p':
				p = atoi(optarg);
				break;
			case 'q':
				q = atoi(optarg);
				break;
			case 'n':
				num_hashes = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: %s -p [int] -q [int] -n [int]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}



	printf("p: %d\n", p);
	printf("q: %d\n", q);
	printf("Total number of generated hashes: %d\n\n", num_hashes);
	uint64_t* hashes = generateRandomHashes(num_hashes);
	
	HL2* hl2 = createHL2(p,q);
	if (hl2 == NULL) {
		PRINT_DEBUG("Erreur lors de la création du sketch HL2\n");
	}
	PRINT_DEBUG("HL2 créé avec succès\n");

	HL3* hl3b3 = createHL3(p,q, 3);
	if (hl3b3 == NULL) {
		PRINT_DEBUG("Erreur lors de la création du sketch HL3b3\n");
	}
	PRINT_DEBUG("HL3 avec 3 bits par cellule créé avec succès\n");

	HL3* hl3b4 = createHL3(p,q, 4);
	if (hl3b4 == NULL) {
		PRINT_DEBUG("Erreur lors de la création du sketch HL3b4\n");
	}
	PRINT_DEBUG("HL3 avec 4 bits par cellule créé avec succès\n");

	HL3* hl3b5 = createHL3(p,q, 5);
	if (hl3b5 == NULL) {
		PRINT_DEBUG("Erreur lors de la création du sketch HL3b5\n");
	}
	PRINT_DEBUG("HL3 avec 5 bits par cellule créé avec succès\n");

	for (int j = 0; j < num_hashes; j++) {
		PRINT_DEBUG("Insérer hachage %lu dans les sketches HL2 et HL3\n", hashes[j]);
#ifdef DEBUG
		printBinHash(hashes[j]);
#endif	
		insertHL2(hl2, hashes[j]);
		insertHL3(hl3b3, hashes[j]);
		insertHL3(hl3b4, hashes[j]);
		insertHL3(hl3b5, hashes[j]);
	}


	double estimate_hl2 = estimate_cardinality((const CommonHLL*)hl2);
	double estimate_hl3b5 = estimate_cardinality((const CommonHLL*)hl3b5);
	double estimate_hl3b4 = estimate_cardinality((const CommonHLL*)hl3b4);
	double estimate_hl3b3 = estimate_cardinality((const CommonHLL*)hl3b3);

	printf("HL2 estimate: %.2f\n", estimate_hl2);
	printf("HL3b5 estimate: %.2f, errors: %ld, super: %d\n", estimate_hl3b5, hl3b5->errors_count, hl3b5->super_counter);
	printf("HL3b4 estimate: %.2f, errors: %ld, super: %d\n", estimate_hl3b4, hl3b4->errors_count, hl3b4->super_counter);
	printf("HL3b3 estimate: %.2f, errors: %ld, super: %d\n", estimate_hl3b3, hl3b3->errors_count, hl3b3->super_counter);
	// Libérez la mémoire allouée pour les sketches
	PRINT_DEBUG("FREE HL2 !\n");
	destroyHL2(hl2);
	PRINT_DEBUG("FREE HL3B3 !\n");
	destroyHL3(hl3b3);
	PRINT_DEBUG("FREE HL3B4 !\n");
	destroyHL3(hl3b4);
	PRINT_DEBUG("FREE HL3B5 !\n");
	destroyHL3(hl3b5);

	printf("\n");


	PRINT_DEBUG("FREE Hashes !\n");
	free(hashes);
	PRINT_DEBUG("DONE : FREE Hashes !\n");

	return 0;
}
