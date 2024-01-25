#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common_hll.h"
#include "hl2.h"
#include "hl3.h"
#include "hl4.h"


typedef CommonHLL* (*CreateHLLFunc)(unsigned char, unsigned char);

void benchmarkHLL(CreateHLLFunc createFunc, unsigned char p, unsigned char q, size_t nombre_elements, size_t* memoryUsage, double* timeSpent) {
	clock_t start = clock();
	size_t nb_elements = nombre_elements;
	CommonHLL* hll = createFunc(p, q);
	for (uint64_t i = 1; i <= nb_elements; i++) {
		ajouter(hll, &i, sizeof(i));
		if (i % 100000000 == 0) {
			printf("Éléments insérés : %lu / %lu\n", i, nb_elements);
		}
	}

	*memoryUsage = sizeOfHLL(hll);
	double cardinalite_estimee = estimate_cardinality(hll);
	printf("Cardinalité estimée: %f\n", cardinalite_estimee);

	destroyHyperLogLog(hll);
	clock_t end = clock();
	*timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
}

int main(int argc, char *argv[]) {
	int p = 10; 
	int q = 8; 

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-p") == 0) {
			if (i + 1 < argc) {
				p = atoi(argv[++i]);
			} else {
				fprintf(stderr, "Option -p nécessite un argument.\n");
				return 1;
			}
		} else if (strcmp(argv[i], "-q") == 0) {
			if (i + 1 < argc) {
				q = atoi(argv[++i]);
			} else {
				fprintf(stderr, "Option -q nécessite un argument.\n");
				return 1;
			}
		}
	}

	if (p == -1 || q == -1) {
		fprintf(stderr, "Les paramètres -p et -q sont requis.\n");
		fprintf(stderr, "Exemple : ./benchmark -p 12 -q 8\n");
		return 1;
	}
	size_t nombre_elements = 10000000;
	CreateHLLFunc functions[] = {(CreateHLLFunc)createHyperLogLog2, (CreateHLLFunc)createHyperLogLog3, (CreateHLLFunc)createHyperLogLog4};
	const char* names[] = {"HL2", "HL3", "HL4"};
	size_t memoryUsages[3];
	double timeSpents[3];

	for (int i = 0; i < 3; i++) {
		benchmarkHLL(functions[i], p, q, nombre_elements, &memoryUsages[i], &timeSpents[i]);
		printf("%s utilise %zu octets de mémoire, Temps d'exécution: %f secondes\n", names[i], memoryUsages[i], timeSpents[i]);
	}

	return 0;
}

