/************************************************************************
 * UTIL.H  -  Hilfs-Rechenoperationen fuer das Projekt Diplom
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1994
 ************************************************************************/

#include "types.h"

/* berechnet die 5 naechsten Punkte */
void BerechneNachbarn (void);

/* kopiert eine Loesung in eine andere */
void LoesungsCopy ( Loesungszeiger, Loesungszeiger);

/* Bestimmt eine zufallsverteilte Anfangspartition */
void compute_anfangspartition (Loesungszeiger);

/* berechnet euklidische Distanzen zwischen allen Feldern anhand der Werte
 * whichco[0], whichco[1], whichco[2], ...
 * und speichert diese in dem eindimensionalem(!) Array 'distanz' */
void Berechne_Distanzen(void);

/* Abstand zweier Punkte
 * nicht in real moeglich , da a,b nicht in modwert vorhanden ! */
float Entfernung (float *, float *);

/* bestimmt den Nachbarcluster anhand der Variable 'nachbarschaft'
 * von Punkt 'n' in Cluster 'clust' */
int Nachbarcluster (int, int);

/* berechnet den Zielfunktionswert fuer die aktuelle Partition, die in
 * 'cluster[][]' gespeichert ist, durchlaeuft 'zielfunkton'. */
float Summe (Loesungszeiger);

/* Punkt 'wen' aus Cluster 'alt' nach 'neu' verschieben */
void UpdateCluster (int, int, int);

/* wird Nachbar loesneu von loesalt akzeptiert ? */
int AcceptSA(float, float, float);

/* wird Nachbar loesneu von loesalt akzeptiert ? */
int AcceptTA(float, float, float);

/* Kontrollfunktion g */
float g(float, int);

float SSumme (int, int, int);

void BerechneMaxUndSumme(void);

float TSumme (int, int, int);



