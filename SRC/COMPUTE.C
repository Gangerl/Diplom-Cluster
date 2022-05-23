/************************************************************************
 * COMPUTE.C  -  Rechenoperationen fuer das Projekt Diplom
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1995
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "menu.h"
#include "global.h"
#include "graphio.h"
#include "compute.h"
#include "werror.h"
#include "memory.h"
#include "util.h"
#include "tabu.h"

/* Sonstiges */
#define frand() ((double) rand() / RAND_MAX)


/* SCHRANKE fuer SA */

// Werte fuer ALPHA=0.9;  bei ALPHA=0.95 => doppelte Schrittanzahl!!
// bei ALPHA=0.975 => vierfache Schrittanzahl!!
// bei zu kleiner Schranke erscheint evtl. BC40RTL.DLL-Warning!
//
//#define SCHRANKECI 0.8 		// alpha = 0.9  =>  32 Schritte
#define SCHRANKECI 0.008	   	// alpha = 0.9  =>  75 Schritte
//#define SCHRANKECI 0.00008	// alpha = 0.9  =>  120 Schritte





/* rechnet die Werte in das Intervall [0,1] um
 */
void Modifiziere_Werte(void)
{
	int i,j ;
	float *maxdifferenz = (float *) malloc (werteanz * sizeof(float));

	/* Damit die Berechnung des Quotienten nicht jedesmal erfolgt */
	for (j=0; j<werteanz; j++)
		maxdifferenz[j] = (float)(big[j] - small[j]);

	/* Berechne skalierte Werte */
	for (i=0; i<anzahl; i++)
		for (j=0; j<werteanz; j++)
			modwert[i][j] = (float)(input[i][j] - small[j]) /
					 maxdifferenz[j];

	distanzen_aktuell = FALSE;		// neue Werte
	free(maxdifferenz);
}


/* Lokale Suche Simultaneous
 */
static int S_LocalSearch (void)
{
	MSG msg;					// fuer User break
	int n;						// zaehlt durch die Punkte
	int clusterneu,clusteralt;	// Cluster des Punktes 'point'
	float sumalt, sumneu;		// Zielfunktionswerte alt und neu
	unsigned int zaehler = 0;	// zaehlt die Schritte
	int *wer = (int*) malloc (anzahl * sizeof (int));
	int *aus = (int*) malloc (anzahl * sizeof (int));
	int *nach = (int*) malloc (anzahl * sizeof (int));

	sumneu = Summe(cluster);	// Zfkt-Wert der Anfangspartition
	do
	{
		for (n=0; n<anzahl; n++)		// init, wer verschoben wurde
			wer[n] = FALSE;
		sumalt = sumneu;
		zaehler++;						// jede Iteration zaehlen

		// erst alle Nachbarn berechnen
		for (n=0; n<anzahl; n++)
		{
			if (consider[n])
			{
				clusteralt = cluster->in_cluster[n];
				if ((clusterneu = Nachbarcluster(n, clusteralt)) != -1)
				{
					wer[n] = TRUE; aus[n] = clusteralt; nach[n] = clusterneu;
				}
			}
		}
		// dann verschieben
		for (n=0; n<anzahl; n++)
			if ((consider[n]) && (wer[n]))
				UpdateCluster(n, aus[n], nach[n]);
		Zaehlerstand_ausgeben(zaehler);
		sumneu = Summe(cluster);

		Zielfunktionswert_ausgeben(sumneu);

		/* User break ? */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{	if (msg.message == WM_LBUTTONDOWN)
			{
				strcpy(szmeldungstext, "Abbruch !");
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while (sumalt > sumneu);
	cluster->zfktwert = sumalt;
	free(wer); free(aus); free(nach);
	return zaehler;
}

/* Lokale Suche, modifizierte Version
 * Da 'UpdateCluster' direkt auf 'cluster' arbeitet ist dies schneller
 * als immer 'cluster' in eine 'test'-Loesung umzukopieren
 */
static int LocalSearch (void)
{
	MSG msg;					// fuer User break
	int n;						// zaehlt durch die Punkte
	int point;					// derzeitiger Punkt
	int clusterneu,clusteralt;	// Cluster des Punktes 'point'
	int erfolg;					// TRUE bei einer Verbesserung
	float sumalt, sumneu;		// Zielfunktionswerte alt und neu
	unsigned int zaehler = 0;	// zaehlt die Schritte

	sumalt = sumneu = globsum;			// Zfkt-Wert der Anfangspartition
	do
	{
		zaehler++;					// jede Iteration zaehlen
		while (!(consider[point = (rand() % anzahl)])) ;    // 0..anzahl-1
		n = 0;
		erfolg = FALSE;
		do
		{
			clusteralt = cluster->in_cluster[point];
			if ((clusterneu = Nachbarcluster(point, clusteralt)) != -1)
			{
				UpdateCluster(point, clusteralt, clusterneu);
				sumneu = SSumme(point, clusteralt, clusterneu);
				if (sumneu <= sumalt) 	// bessere Loesung
				{
					erfolg = TRUE;
					sumalt = sumneu;
					Paintit(point,clusterneu);
					Zielfunktionswert_ausgeben(sumneu);
					globsum = sumneu;
				}
				else				 	// andernfalls rueckgaengig machen
					UpdateCluster(point, clusterneu, clusteralt);
			}
			while ((++n<anzahl) && (! consider[point = (point+1) % anzahl]));
			Zaehlerstand_ausgeben(zaehler);
		}
		while ((n < anzahl) && (!erfolg));
		/* User break ? */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{	if (msg.message == WM_LBUTTONDOWN)
			{
				strcpy(szmeldungstext, "Abbruch !");
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while (n < anzahl);
	cluster->zfktwert = sumalt;
	return zaehler;
}


/* Iterative Improvement
 */
static void IterativeImprovement( void )
{
	MSG msg;					// fuer User break
	int n;						// zaehlt durch die Punkte
	int clusterneu,clusteralt;	// Cluster des Punktes 'point'
	int wer, alt, neu;			// welcher aus wem ist der beste?
	float sumalt, sumneu;		// Zielfunktionswerte alt und neu
	float sumtmp;				// Zielfunktionswert der Zwischenschritte
	unsigned int zaehler = 0;	// zaehlt die Schritte
	int erfolg;

	sumneu = globsum;
	do
	{
		sumalt = sumneu;
		zaehler++;					// jede Iteration zaehlen

		// durchlaufe alle Nachbarm und finde den Besten
		sumneu = realunendlich;
		erfolg = FALSE;
		for (n=0; n<anzahl; n++)
		if (consider[n])
		{
			clusteralt = cluster->in_cluster[n];
			if ((clusterneu = Nachbarcluster(n, clusteralt)) != -1)
			{
				UpdateCluster(n, clusteralt, clusterneu);
				sumtmp = SSumme(n, clusteralt, clusterneu);
				if (sumtmp < sumneu) 	// bessere Loesung
				{
					erfolg = TRUE;
					sumneu = sumtmp;
					wer    = n;
					alt    = clusteralt;
					neu    = clusterneu;
				}
				UpdateCluster(n, clusterneu, clusteralt);
			}
		}
		if (erfolg)
		{
			Zaehlerstand_ausgeben(zaehler);
			UpdateCluster(wer, alt, neu);
			globsum = sumneu;
			if (sumneu < sumalt) Zielfunktionswert_ausgeben(sumneu);
			Paintit(wer, neu);
		}

		/* User break ? */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{	if (msg.message == WM_LBUTTONDOWN)
			{
				strcpy(szmeldungstext, "Abbruch !");
				break;
			}
			DispatchMessage(&msg);
		}
	}
	while ( (sumalt > sumneu) && (erfolg) );
	cluster->zfktwert = sumalt;
}



/* Simulated Annealing mit lokaler Suche:
 * es werden jeweils lokale Optima verglichen
 * in cluster steht die Test-Clusterung
 *
 * Der uebergebene Funktionszeiger enthaelt entweder eine Single-
 * oder Simultaneous-Lokalopt-Routine
 */
static void SimulatedAnnealingLocalSearch ( int (*LokalFunktion)(void) )
{
	MSG msg;					// fuer User break
	int n;						// zaehlt durch die Punkte
	int point;					// aktueller Punkt
	int clusterneu,clusteralt;	// Cluster des Punktes 'point'
	int probierte, Probeanzahl=0;	// wenn alle durchprobiert, dann STOP
	int erfolg;
	float sumalt;				// Zielfunktionswerte alt und neu
	unsigned int zaehler = 0;	// zaehlt die Schritte
	float c = 0.0;

	(*LokalFunktion)();
	sumalt = cluster->zfktwert;
	LoesungsCopy(cluster, best);// derzeit best
	for (n=0; n<anzahl; n++)
		if (consider[n])
			Probeanzahl++;

	do	// laueft bis Stopkriterium
	{
		c = g(c, zaehler);    		// c aktualisieren
		zaehler++;					// jede Iteration zaehlen
		while (!consider[point = rand() % anzahl]) ;    // 0..anzahl-1
		n = 0;
		erfolg = FALSE;
		probierte = 0;				// Anzahl derer, die keinen Nachbarn haben
		do	// Auswahl eines Nachbarn
		{
			clusteralt = cluster->in_cluster[point];
			if ((clusterneu = Nachbarcluster(point, clusteralt)) != -1)
			{
				LoesungsCopy(cluster, test);
				UpdateCluster(point, clusteralt, clusterneu);
				globsum = SSumme(point, clusteralt, clusterneu);
				(*LokalFunktion)();
				if (AcceptSA(cluster->zfktwert, sumalt, c))
				{
					erfolg = TRUE;
					sumalt = cluster->zfktwert;
					Paintit(point,clusterneu);
					Zielfunktionswert_ausgeben(sumalt);
					if (sumalt < best->zfktwert)	// beste Loesung ?
					{
						LoesungsCopy(cluster, best);
						ClearDerzeitAusgabe();
						DerzeitAusgabe(sumalt);
						ClearSimAnAusgabe();
						SimAnAusgabe(zaehler,
							(float) (GetTickCount()-dwoldtime) / 1000L);
					}
				}
				else				 	// andernfalls rueckgaengig machen
					LoesungsCopy(test, cluster);
			}
			else	// hatte keinen Nachbarn
				probierte++;
			// naechster Punkt
			while ((++n<anzahl) && (! consider[point = (point+1) % anzahl]));
		}
		while ((n < anzahl) && (!erfolg) && (probierte < Probeanzahl));

		Zaehlerstand_ausgeben(zaehler);

		/* User break ? */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{	if (msg.message == WM_LBUTTONDOWN)
			{
				strcpy(szmeldungstext, "Abbruch !");
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while ( (c > SCHRANKECI) && (probierte < Probeanzahl) );
}


/* Simulated Annealing ohne lokale Suche
 * es wird nur ein Punkt verschoben un dann verglichen
 */
static void SimulatedAnnealing(int (*Akzeptiere)(float,float,float))
{
	MSG msg;					// fuer User break
	int n;						// zaehlt durch die Punkte
	int point;					// aktueller Punkt
	int clusterneu,clusteralt;	// Cluster des Punktes 'point'
	int probierte, Probeanzahl=0;	// wenn alle durchprobiert, dann STOP
	int erfolg;
	float sumalt, sumneu;		// Zielfunktionswerte alt und neu
	unsigned int zaehler = 0;	// zaehlt die Schritte
	float c = 0.0;

	cluster->zfktwert = sumalt = globsum;
	LoesungsCopy(cluster, best);// derzeit best

	// Probeanzahl = wieviele ueberhaupt considerd werden muessen
	for (n=0; n<anzahl; n++)
		if (consider[n])
			Probeanzahl++;

	do	// laueft bis Stopkriterium
	{
		c = g(c, zaehler);    		// c aktualisieren
		zaehler++;					// jede Iteration zaehlen
		while (!consider[point = rand() % anzahl]) ;    // 0..anzahl-1
		n = 0;
		erfolg = FALSE;
		probierte = 0;				// Anzahl derer, die keinen Nachbarn haben
		do	// Auswahl eines Nachbarn
		{
			clusteralt = cluster->in_cluster[point];
			if ((clusterneu = Nachbarcluster(point, clusteralt)) != -1)
			{
				UpdateCluster(point, clusteralt, clusterneu);
				sumneu = SSumme(point, clusteralt, clusterneu);
				if ((*Akzeptiere)(sumneu, sumalt, c))
				{
					erfolg = TRUE;
					sumalt = sumneu;
					Paintit(point,clusterneu);
					Zielfunktionswert_ausgeben(sumneu);
					cluster->zfktwert = sumneu;
					if (sumneu < best->zfktwert)	// beste Loesung ?
					{
						LoesungsCopy(cluster, best);
						ClearDerzeitAusgabe();
						DerzeitAusgabe(sumneu);
						ClearSimAnAusgabe();
						SimAnAusgabe(zaehler,
							(float) (GetTickCount()-dwoldtime) / 1000L);
					}
					globsum = sumneu;
				}
				else				 	// andernfalls rueckgaengig machen
					UpdateCluster(point, clusterneu, clusteralt);
			}
			else	// hatte keinen Nachbarn
				probierte++;
			// naechster Punkt
			while ((++n<anzahl) && (! consider[point = (point+1) % anzahl]));
		}
		while ((n < anzahl) && (!erfolg) && (probierte < Probeanzahl));

		Zaehlerstand_ausgeben(zaehler);

		/* User break ? */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{	if (msg.message == WM_LBUTTONDOWN)
			{
				strcpy(szmeldungstext, "Abbruch !");
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	while ( (c > SCHRANKECI) && (probierte < Probeanzahl) );
}


/* berechnet eine Loesung anhand des Algorithmus´ 'algorithmus',
 * einzige aus diesem Modul exportierte Funktion
 */
int Compute(void)
{
	int nachbartmp; 	// bei Algo == Combined nachbarschaft zwischenspeichern
	int i=0;

	/* echte Zufallszahlen */
	time_t tt, *t = &tt;
	if ( (tt = time(t))  == (time_t)-1) exit(1);
	srand((unsigned)(tt%30000));
	randomize();

	ClearSimAnAusgabe();
	ClearEndeMeldung();
	strcpy(szmeldungstext, "fertig !");	// Standard-Terminierung

	/* testen, ob ueberhaupt ein Punkt considerd ist! */
	while (i<anzahl)
		if (consider[i++]) break;
	if (i==anzahl)
		werror(hglobalwin, "Durch Subbereichs-Einschraenkung keine %s",
			"Punkte mehr, die betrachtet werden muessen");
	else
	{

	/* Anfangspartition bestimmen */
	compute_anfangspartition(cluster);

	/* Partitionierung als derzeit beste in 'best' speichern */

	gerechnet = TRUE;				// damit es bunter wird 18.10.95
	berechnetecluster = clusteranz; // fuer Paintit_all!!
	if ( !distanzen_aktuell )		// neue Werte fuer Berechnung
	{
		Berechne_Distanzen();		// Abstaende neu definieren
		if (nachbarschaft == IDD_NACHBARnaechster) BerechneNachbarn();
	}
	else if (!nachbaranz_aktuell) BerechneNachbarn();

	BerechneMaxUndSumme();		// Die Summen und Maxima initialisieren.

	globsum = cluster->zfktwert = Summe(cluster);
	LoesungsCopy(cluster, best);
	Paintit_all();					// Anfangspartition ausgeben

	ClearZeitAusgabe();
	ClearDerzeitAusgabe();
	dwoldtime = GetTickCount();

	/* Algorithmus waehlen */
	if (algorithmus == IDD_COMBINED)
	{
		nachbartmp = nachbarschaft;
		if (!nachbaranz_aktuell) BerechneNachbarn();
		nachbarschaft = IDD_NACHBARcentroid;
		S_LocalSearch();
		LoesungsCopy(cluster, best);
		globsum = cluster->zfktwert;
		nachbarschaft = IDD_NACHBARnaechster;
		LocalSearch();
		LoesungsCopy(cluster, best);
		nachbarschaft = nachbartmp;
	}
	else
	{
	if (simultaneous)
		if (algorithmus == IDD_LOCAL || algorithmus == IDD_ITERATIVE)
		{
			S_LocalSearch();
			LoesungsCopy(cluster, best);
		}
		else
			SimulatedAnnealingLocalSearch( (int (*)(void))S_LocalSearch );
	else
		if (algorithmus == IDD_TABU)
			TabuSearch();
		else if (algorithmus == IDD_THRESHOLD)
			SimulatedAnnealing( (int (*)(float, float, float))AcceptTA );
		else if (algorithmus == IDD_SIMANLOCAL)
			SimulatedAnnealingLocalSearch( (int (*)(void))LocalSearch );
		else if (algorithmus == IDD_SIMAN)
			SimulatedAnnealing( (int (*)(float,float,float))AcceptSA );
		else if (algorithmus == IDD_ITERATIVE)
		{
			IterativeImprovement();
			LoesungsCopy(cluster, best);
		}
		else
		{
			LocalSearch();
			LoesungsCopy(cluster, best);
		}
	}

	dwnewtime = GetTickCount();

	Zielfunktionswert_ausgeben(best->zfktwert);
	ZeitAusgeben((float)(dwnewtime - dwoldtime) / 1000L);
	EndeMeldung(szmeldungstext);
	Paintit_all();
	}  						// end else
	return clusteranz;
}


unsigned int zaehler;			// fuer Enumerate
float summe;					// fuer Enumerate
long schranke;			   		// Grenze der Enumertaion
float grenze;				    // Debug


/* rekursive Prozedur fuer totale Enumeration
 */
void Numerate (int point)
{
	MSG msg;					// fuer User break
	int i;

	for (i=0; i<clusteranz; i++)
	{
		cluster->cluster[i][cluster->anz_in_cluster[i]++] = point;
		if (++point < anzahl)	// weiter absteigen
		{
			Numerate(point);
			if (zaehler > schranke)
				return;
		}
		else	// Partition erreicht
		{
			zaehler++;
			if ((summe=Summe(cluster)) < best->zfktwert)
			{
				cluster->zfktwert = grenze = summe;
				LoesungsCopy(cluster, best);
				Zaehlerstand_ausgeben(zaehler);
				Zielfunktionswert_ausgeben(summe);
			}
		}
		point--;
		cluster->anz_in_cluster[i]--;
	}
	/* User break ? */
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{	if (msg.message == WM_LBUTTONDOWN)
		{
			strcpy(szmeldungstext, "Abbruch !");
			zaehler = schranke+1;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


/* berechnet per totaler Enumeration eine optimale Partition
 * ruft die rekursive Prozedur 'Numerate' auf
 */
int Enumerate(void)
{
	int i;

	ClearSimAnAusgabe();
	ClearEndeMeldung();
	strcpy(szmeldungstext, "fertig !");	// Standard-Terminierung

	gerechnet = TRUE;				// damit es bunter wird 18.10.95
	Paintit_all();
	berechnetecluster = clusteranz; // fuer Paintit_all!!
	if ( !distanzen_aktuell )		// neue Werte fuer Berechnung
	{
		Berechne_Distanzen();		// Abstaende neu definieren
		if (nachbarschaft == IDD_NACHBARnaechster) BerechneNachbarn();
	}
	else if (!nachbaranz_aktuell) BerechneNachbarn();

	/* initialisieren */
	zaehler = 0;

	/* nur 1/clusteranz des Suchraumes ist n"otig */
	schranke = exp((anzahl-1) * log(clusteranz));
	DBMSG(("schranke : %7d",schranke));
	best->zfktwert = realunendlich-10000.0;
	grenze = realunendlich-10000.5;
	sprintf(buf,"grenze: %14.2f",grenze);
	DBMSG(("%s",buf));

	/* Ausgangsposition: nichts zugeordnet */
	for (i=0; i<clusteranz; i++)
		cluster->anz_in_cluster[i] = 0;

	ClearZeitAusgabe();
	ClearDerzeitAusgabe();
	dwoldtime = GetTickCount();

	Numerate(0);					// los geht's

	dwnewtime = GetTickCount();
	Zaehlerstand_ausgeben(zaehler);
	ZeitAusgeben((float)(dwnewtime - dwoldtime) / 1000L);
	EndeMeldung(szmeldungstext);
	Paintit_all();
	Zielfunktionswert_ausgeben(best->zfktwert);
	return clusteranz;
}


/* faerbt die Punkte in horizontaler Richtung nach der Anzahl
 * der auftretenden Merkmale ein, sofern diese < MAXCLUSTER ist
 */
void einfaerben(Loesungszeiger anf)
{
	int i, cl, st;

	if ( big[whichgr[1]] - small[whichgr[1]] - 1  <  15 )
	{
		clusteranz = big[whichgr[1]] - small[whichgr[1]] - 1;

		for (i=0; i<clusteranz; i++)
			anf->anz_in_cluster[i] = 0;
		for (i=0; i<anzahl; i++)
		{
			cl = input[i][whichgr[1]] - small[whichgr[1]] - 1;
			st = anf->anz_in_cluster[cl];
			anf->cluster[cl][st] = i;
			anf->anz_in_cluster[cl]++;
			anf->in_cluster[i] = cl;
		}
	}
	else
		werror(hglobalwin, "Anzahl der Cluster zu gross: Aktion wird nicht ausgefuehrt!");
}

void Considering(void)
{
	int i,j,k;

	for (i=0; i<anzahl; i++)
	{
		consider[i] = 1;
		j = 0;
		while ( (consider[i]) && (j<werteanz) )
		{
			k=0;
			while ( (consider[i]) && (k<MAXSTRINGS) )
			{
				if (feldsubs[j][k])
					consider[i] = !( input[i][j] == k + OFFSET );
				k++;
			}
			j++;
		}

	}
}



