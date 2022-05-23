/**************************************************************************
 * TYPES.H  -  globale Typen
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1994
 **************************************************************************/

#include "defines.h"				// um MAXCLUSTER zu kennen

#ifndef TYPES
#define TYPES

typedef struct {
	int **cluster;					// Array ueber Cluster und Punkten darin
	int anz_in_cluster[MAXCLUSTER];	// wieviele Punkte in jedem Cluster
	int *in_cluster;				// Array ueber alle Punkte: in welchem Cl.
	float zfktwert;					// Zielfunktionswert dieser Loesung
	} Loesung;

typedef Loesung * Loesungszeiger;

#endif
