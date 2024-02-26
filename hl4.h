// hl4.h

#ifndef HL4_H
#define HL4_H

#include <stdint.h>
#include <stdlib.h>
#include "common_hll.h"

typedef struct {
	CommonHLL commonHLL;
	uint8_t* compteurs;      // Tableau pour les compteurs de base
	uint8_t* superCompteurs; // Tableau pour les super compteurs
	uint8_t megaCompteur;    // Mega compteur
} HL4;

HL4* createHL4(unsigned char p, unsigned char q);
void destroyHL4(HL4* hll);
void insertHL4(HL4* hll, Bitstream* bitstream);
#endif // HL4_H


