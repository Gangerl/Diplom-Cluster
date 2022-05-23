/************************************************************************
 * UTIL.C  -  Hilfs-Rechenoperationen fuer das Projekt Diplom
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1995
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "menu.h"
#include "global.h"
#include "util.h"
#include "werror.h"

/* 2-D Gitter fuer euklidische Abstaende */
#define dist(i,j) ((i)>(j) ? distanz[((i)-1)*(i)/2+(j)] \
			   : distanz[((j)-1)*(j)/2+(i)])
/* Sonstiges */
#define frand() ((double) rand() / RAND_MAX)

/* Startwert SA fuer c */
#define START   20

/* Faktor fuer SA */
//#define ALPHA	0.9
//#define ALPHA	0.95
//#define ALPHA 0.965
#define ALPHA	0.975


float *distanz;			// 2-D Gitter! (als Dreieckmsmatrix gespeichert)

float clmaximum[MAXCLUSTER];		// Maxima in den Clustern
float clsumme  [MAXCLUSTER];		// Summen in den Clustern



/* berechnet die  naechsten Punkte zu allen Punkten (nachbaranz viele)
*/
void BerechneNachbarn (void)
{
	int i,j,k,l;

	for (i=0; i<anzahl; i++)							// fuer alle Daten
	  if (consider[i])
	  {
		for (j=0; j<nachbaranz; j++)					// Initialisieren
			if (consider[j])
				nachbar[i][j] = (i+j+1) % anzahl;
		for (j=0; j<anzahl; j++)						// alle Kombinationen
		{
			if ( (i != j) && (consider[j]) )
			{
				for (k=0; k<nachbaranz; k++)		   	// Liste von Nachbarn
				{
					if (dist(i,j) < dist(i,nachbar[i][k]))  // naeherer Nachbar
					{
						for (l=nachbaranz-1; l>k; l--)	// verschiebe Liste
							nachbar[i][l] = nachbar[i][l-1];
						nachbar[i][k] = j;
						break;
					}
					if (nachbar[i][k] == j) break;		// schon drin
				}
			}
		}
	  }
	nachbaranz_aktuell = TRUE;
}


/* kopiert eine Loesung in eine andere
 */
void LoesungsCopy ( Loesungszeiger alt, Loesungszeiger neu )
{
	int i,j;

	/* kopiert die clusterverteilung */
	for (i=0; i<clusteranz; i++)
		for (j=0; j<alt->anz_in_cluster[i]; j++)
			neu->cluster[i][j] = alt->cluster[i][j];

	/* Anz_inclustercopy */
	for (i=0; i<clusteranz; i++)
		neu->anz_in_cluster[i] = alt->anz_in_cluster[i];

	/* Clustercopy */
	for (i=0; i<anzahl; i++)
		neu->in_cluster[i] = alt->in_cluster[i];

	/* fktwert kopieren */
	neu->zfktwert = alt->zfktwert;
}



/* Bestimmt eine zufallsverteilte Anfangspartition
 */
void compute_anfangspartition ( Loesungszeiger anf )
{
	int i, cl, st;

	for (i=0; i<clusteranz; i++)
		anf->anz_in_cluster[i] = 0;
	for (i=0; i<anzahl; i++)
		if (consider[i])
		{
			cl = rand() % clusteranz;
			st = anf->anz_in_cluster[cl];
			anf->cluster[cl][st] = i;
			anf->anz_in_cluster[cl]++;
			anf->in_cluster[i] = cl;
		}
}


/* berechnet euklidische Distanzen zwischen allen Feldern anhand der Werte
 * whichco[0], whichco[1], whichco[2], ...
 * und speichert diese in dem eindimensionalem(!) Array 'distanz'
 */
void Berechne_Distanzen(void)
{
	int i,j,k ;
	float help;

	/* Speicher allokieren */
	if (first_time)
		distanz = (float *)	malloc(( anzahl * (anzahl-1) /2) * sizeof(float) );
	else
		distanz = (float *)	realloc (
			distanz, ( anzahl * (anzahl-1) /2) * sizeof(float) );
	for (i=1; i< anzahl; i++)
		for (j=0; j<i; j++)
		{
			help = 0.0;
			for (k=0; k<anzco; k++)
			help = help + (modwert[j][whichco[k]]-modwert[i][whichco[k]] )*
					(modwert[j][whichco[k]]-modwert[i][whichco[k]] );
			distanz[(i-1)*i/2+j] = sqrt(help);
		}
	distanzen_aktuell = TRUE;
}


void BerechneMaxUndSumme (void)
{
	int i,k,l;
	float help;

	// jetzt die Maxima
	for (i=0; i<clusteranz; i++)
	{
		help = 0.0;
		for (k=1; k<cluster->anz_in_cluster[i]; k++)
			for (l=0; l<k; l++)
				if (dist(cluster->cluster[i][k],cluster->cluster[i][l]) > help)
					help = dist(cluster->cluster[i][k],cluster->cluster[i][l]);
		clmaximum[i] = help;
	}

	// jetzt die Summen
	for (i=0; i<clusteranz; i++)
	{
		help = 0.0;
		for (k=1; k<cluster->anz_in_cluster[i]; k++)
			for (l=0; l<k; l++)
				help += dist(cluster->cluster[i][k],cluster->cluster[i][l]);
		clsumme[i] = help;
	}
}


float F_Abstand (float *mitte, int n)
{
	int k;
	float summe = 0.0;

	for (k=0; k<anzco; k++)
		summe += pow(mitte[k] - modwert[n][whichco[k]], 2);

	return summe;
}


/* bestimmt den Nachbarcluster anhand der Variable 'nachbarschaft'
 * von Punkt 'n' in Cluster 'clust'
 */
int Nachbarcluster (int n, int clust)
{
	int i,j,k;
	int naechster_cluster;

	float distanz=realunendlich;
	float abstand;
	float mitte[5];

	switch (nachbarschaft)
	{
	case IDD_NACHBARnaechster:
		/* Cluster, aus dem ein Punkt der naechste zum aktuellen ist,
		   verwendet die Funktion 'BerechneNachbarn' */
		i = 0;

		DBMSG(("n=  %2d", n));
		while (i < nachbaranz) {
		DBMSG(("i: %2d n: %2d",i,nachbar[n][i]));
			if (cluster->in_cluster[nachbar[n][i]] != clust)
				return cluster->in_cluster[nachbar[n][i]];
			i++;
		}
		return -1;
	case IDD_NACHBARcentroid:
		/* naechster Gruppencentroid  */
		for (i=0; i<clusteranz; i++)				// durch alle cluster
		{
			// initialisieren der Mitten mit 0
			for (k=0; k<anzco; k++)
				mitte[k] = 0.0;

			// aufsummieren der Punkte in dem Cluster i
			for (j=0; j<cluster->anz_in_cluster[i]; j++)
			{
			  if (consider[cluster->cluster[i][j]])
				for (k=0; k<anzco; k++)
					mitte[k] += modwert[cluster->cluster[i][j]][whichco[k]];
			}
			// dividieren durch die Anzahl der Elemente
			for (k=0; k<anzco; k++)
			{
			  if (cluster->anz_in_cluster[i] != 0)
				mitte[k] = mitte[k] / (float)cluster->anz_in_cluster[i];
			}
			// schauen, ob dies der bessere Cluster
			if ( (abstand = F_Abstand(mitte, n)) < distanz)
			{
				distanz = abstand;
				naechster_cluster = i;
			}
		 }
		if (naechster_cluster == clust) return -1;
		break;
	case IDD_NACHBARrandom:
		/* wie der Name sagt .. */
		while ( (naechster_cluster = rand() % clusteranz )== clust ) ;
		break;
	}
	return naechster_cluster;
}


/* berechnet den Zielfunktionswert fuer die aktuelle Partition, die in
 * 'cluster[][]' gespeichert ist, durchlaeuft 'zielfunkton'.
 */
float Summe (Loesungszeiger cl)
{
	int   i,k,l;
	float help,s,summe = 0.0,
		count;

	for (i=0; i<clusteranz; i++)
		if (cl->anz_in_cluster[i] < 2)		// Cluster leer == Bloedsinn
			return realunendlich;

	if (WICHTUNG[1])            		// S S^2
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cl->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					help +=   pow(dist(cl->cluster[i][k],cl->cluster[i][l]),2);
			s += help;
		}
		summe += WICHTUNG[1] * s;
	}
	if (WICHTUNG[0])            		// S S
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cl->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					help += dist(cl->cluster[i][k],cl->cluster[i][l]);
			s += help;
		}
		summe += WICHTUNG[0] * s;
	}
	if (WICHTUNG[2])            		// S 1/N*S
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			count = 0.0;
			help  = 0.0;
			for (k=1; k<cl->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
				{
					help += dist(cl->cluster[i][k],cl->cluster[i][l]);
					count++;
				}
			if (cl->anz_in_cluster[i] > 1)
				s += help/count;
		}
		summe += WICHTUNG[2] * s;
	}
	if (WICHTUNG[3])            		// max S
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help  = 0.0;
			for (k=1; k<cl->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					help += dist(cl->cluster[i][k],cl->cluster[i][l]);
			if (help > s) s=help;
		}
		summe += WICHTUNG[3] * s;
	}
	if (WICHTUNG[4])            		// S max
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cl->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					if (dist(cl->cluster[i][k],cl->cluster[i][l]) > help)
						help = dist(cl->cluster[i][k],cl->cluster[i][l]);
			s += help;
		}
		summe += WICHTUNG[4] * s;
	}
	if (WICHTUNG[5])            		// max max
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cl->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					if (dist(cl->cluster[i][k],cl->cluster[i][l]) > help)
						help = dist(cl->cluster[i][k],cl->cluster[i][l]);
			if (help > s) s=help;
		}
		summe += WICHTUNG[5] * s;
	}

	return summe;
}


/* Punkt 'wen' aus Cluster 'alt' nach 'neu' verschieben
 */
void UpdateCluster (int wen, int alt, int neu)
{
	int i=0;
	DBMSG(("%3d %d %d",wen,alt,neu));
	while (cluster->cluster[alt][i] != wen) i++;
	cluster->cluster[alt][i] = cluster->cluster[alt][cluster->anz_in_cluster[alt]-1];
	cluster->anz_in_cluster[alt]--;
	cluster->cluster[neu][cluster->anz_in_cluster[neu]] = wen;
	cluster->anz_in_cluster[neu]++;
	cluster->in_cluster[wen] = neu;
}


/*
 *
 * SIMULATED ANNEALING
 *
 */

/* wird Nachbar loesneu von loesalt akzeptiert ?
 */
int AcceptSA(float sumneu, float sumalt, float c)
{
		return (frand() < exp(-(sumneu-sumalt)/c));
}


/* wird Nachbar loesneu von loesalt akzeptiert ?
 */
int AcceptTA(float sumneu, float sumalt, float c)
{
		return (sumneu-sumalt < c);
}


/* Kontrollfunktion g
*/
float g(float c,int i)
{
	  if ( i == 0 )           /* Startwert */
			return START;			/* MW : 20 liefern  */
	  else                    /* c linear verringern */
			return ALPHA * c;
}







/* berechnet den Zielfunktionswert fuer die aktuelle Partition, die in
 * 'cluster[][]' gespeichert ist, durchlaeuft 'zielfunkton'.
 *
 * beruecksichtigt dabei aber nur die Aenderung zu globsum!!
 *
 * Parameter:
 * wer : verschobener Punkt
 * alt : alter Cluster
 * neu : neuer Cluster
 */
float SSumme (int wer, int alt, int neu)
{
	int   i,k,l;
	float help, s, summe = 0.0;

	for (i=0; i<clusteranz; i++)
		if (cluster->anz_in_cluster[i] < 2)		// Cluster leer == Bloedsinn
			return realunendlich;

	// Anfangswert bei den ersten 3 Zfkt's anpassen
	if (WICHTUNG[0] || WICHTUNG[1] || WICHTUNG[2])
		summe = globsum;

	if (WICHTUNG[0])            		// S S
	{
		s = 0.0;
		for (i=0; i<cluster->anz_in_cluster[alt]; i++)		// subtrahieren
			s -= dist(cluster->cluster[alt][i],wer);
		for (i=0; i<cluster->anz_in_cluster[neu]-1; i++)	// addieren
			s += dist(cluster->cluster[neu][i],wer);
		summe += WICHTUNG[0] * s;
	}
	if (WICHTUNG[1])            		// S S^2
	{
		s = 0.0;
		for (i=0; i<cluster->anz_in_cluster[alt]; i++)		// subtrahieren
			s -= pow( dist(cluster->cluster[alt][i],wer), 2);
		for (i=0; i<cluster->anz_in_cluster[neu]-1; i++)	// addieren
			s += pow( dist(cluster->cluster[neu][i],wer), 2);
		summe += WICHTUNG[1] * s;
	}
	if (WICHTUNG[2])            		// S 1/N*S
	{
		s    = 0.0;
		help = 0.0;
		for (i=0; i<cluster->anz_in_cluster[alt]; i++)		// subtrahieren
			s -= dist(cluster->cluster[alt][i],wer);
		if (cluster->anz_in_cluster[alt] > 0)
			s = s / (float) cluster->anz_in_cluster[alt]+1;
		for (i=0; i<cluster->anz_in_cluster[neu]-1; i++)	// addieren
			help += dist(cluster->cluster[neu][i],wer);
		if (cluster->anz_in_cluster[neu] > 2)
			help = help / (float) cluster->anz_in_cluster[neu];
			s += help;
		summe += WICHTUNG[2] * s;
	}
	if (WICHTUNG[3])            		// max S
	{

	// Summen neu berechnen

		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help  = 0.0;
			for (k=1; k<cluster->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					help += dist(cluster->cluster[i][k],cluster->cluster[i][l]);
			if (help > s) s=help;
		}
		summe += WICHTUNG[3] * s;
	}
	if (WICHTUNG[4])            		// S max
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cluster->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					if (dist(cluster->cluster[i][k],cluster->cluster[i][l]) > help)
						help = dist(cluster->cluster[i][k],cluster->cluster[i][l]);
			s += help;
		}
		summe += WICHTUNG[4] * s;
	}
	if (WICHTUNG[5])            		// max max
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cluster->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					if (dist(cluster->cluster[i][k],cluster->cluster[i][l]) > help)
						help = dist(cluster->cluster[i][k],cluster->cluster[i][l]);
			if (help > s) s=help;
		}
		summe += WICHTUNG[5] * s;
	}

	return summe;
}


/*
 * siehe SSumme, Unterschied ist:
 * Die Punkte sind noch nicht verschoben worden!
 */
float TSumme(int wer, int alt, int neu)
{
	int   i,k,l;
	float help, s, summe = 0.0;

	for (i=0; i<clusteranz; i++)
		if (cluster->anz_in_cluster[i] <= 1)		// Cluster leer == Bloedsinn
			return realunendlich;

	summe = globsum;

	if (WICHTUNG[0])            		// S S
	{
		s = 0.0;
		for (i=0; i<cluster->anz_in_cluster[alt]; i++)		// subtrahieren
		  if (cluster->cluster[alt][i] != wer)
			s -= dist(cluster->cluster[alt][i],wer);
		for (i=0; i<cluster->anz_in_cluster[neu]; i++)	// addieren
			s += dist(cluster->cluster[neu][i],wer);
		summe += WICHTUNG[0] * s;
	}
	if (WICHTUNG[1])            		// S S^2
	{
		s = 0.0;
		for (i=0; i<cluster->anz_in_cluster[alt]; i++)		// subtrahieren
		  if (cluster->cluster[alt][i] != wer)
			s -= pow( dist(cluster->cluster[alt][i],wer), 2);
		for (i=0; i<cluster->anz_in_cluster[neu]; i++)	// addieren
			s += pow( dist(cluster->cluster[neu][i],wer), 2);
		summe += WICHTUNG[1] * s;
	}
	if (WICHTUNG[2])            		// S 1/N*S
	{
		s    = 0.0;
		help = 0.0;
		for (i=0; i<cluster->anz_in_cluster[alt]; i++)		// subtrahieren
		  if (cluster->cluster[alt][i] != wer)
			s -= dist(cluster->cluster[alt][i],wer);
		s = s / (float) cluster->anz_in_cluster[alt];

		for (i=0; i<cluster->anz_in_cluster[neu]; i++)	// addieren
			help += dist(cluster->cluster[neu][i],wer);
		if (cluster->anz_in_cluster[neu] > 2)
			help = help / (float) cluster->anz_in_cluster[neu]+1;
			s += help;
		summe += WICHTUNG[2] * s;
	}
	if (WICHTUNG[3])            		// max S
	{
	// Summen neu berechnen
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help  = 0.0;
			for (k=1; k<cluster->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					help += dist(cluster->cluster[i][k],cluster->cluster[i][l]);
			if (help > s) s=help;
		}
		summe += WICHTUNG[3] * s;
	}
	if (WICHTUNG[4])            		// S max
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cluster->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					if (dist(cluster->cluster[i][k],cluster->cluster[i][l]) > help)
						help = dist(cluster->cluster[i][k],cluster->cluster[i][l]);
			s += help;
		}
		summe += WICHTUNG[4] * s;
	}
	if (WICHTUNG[5])            		// max max
	{
		s = 0.0;
		for (i=0; i<clusteranz; i++)
		{
			help = 0.0;
			for (k=1; k<cluster->anz_in_cluster[i]; k++)
				for (l=0; l<k; l++)
					if (dist(cluster->cluster[i][k],cluster->cluster[i][l]) > help)
						help = dist(cluster->cluster[i][k],cluster->cluster[i][l]);
			if (help > s) s=help;
		}
		summe += WICHTUNG[5] * s;
	}

	return summe;
}

