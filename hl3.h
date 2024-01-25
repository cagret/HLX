// hl3.h

#ifndef HL3_H
#define HL3_H

#include <stdint.h>
#include <stdlib.h>
#include "common_hll.h"

typedef struct {
	CommonHLL* commonHLL;
	Bitstream* bitstream;
	uint32_t M;
	uint8_t Offset;
	uint8_t max_value;     // Valeur maximale pour chaque registre
	uint32_t Z_occurence;  // Nombre d'occurrences de zéro
			       // Structure pour stocker les hits calculés
	struct {
		int8_t zeroCount;
		uint32_t index;
	} *hits;
	uint32_t hitsSize;     // Taille du tableau hits
} HL3;

HL3* createHL3(unsigned char p, unsigned char q);
void destroyHL3(HL3* hll3);
void insertHL3(CommonHLL* hll, Bitstream* bitstream);
#endif // HL3_H


