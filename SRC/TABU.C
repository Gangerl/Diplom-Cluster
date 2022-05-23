/************************************************************************
 * TABU.C  -  Alles zur Tabu-Suche  / Projekt Diplom
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1995
 ************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "graphio.h"
#include "util.h"
#include "tabu.h"


int tabuliste[MAXTABULISTENLAENGE][2];	// Tabu-Liste
int tabupos;							// akt. Position in der Tabu-Liste


/* zeigt, ob diese Konfiguration tabu ist */
static int Tabu(int wer, int wen)
{
	int i;

	for (i=0; i<tabulistenlaenge; i++)
		if (  (tabuliste[i][0] == wer) && (tabuliste[i][1] == wen) )
			return TRUE;

	// keine Uebereinstimmung gefunden
	return FALSE;
}


/* zeigt, ob Aspiration Kriterium erfuellt ist
static int Aspirate(int point, int clneu, int clalt, float alt)
{
	float ergebnis;

	UpdateCluster(point, clneu, clalt);
	ergebnis = SSumme(point, clneu, clalt);
	UpdateCluster(point, clalt, clneu);
	return ( (ergebnis * tabufaktor) < alt );
} */


/* loescht die Eintraege in der Tabu-Liste */
static void ResetTabuliste(void)
{
	int i;

	tabupos = 0;
	for (i=0; i<tabulistenlaenge; i++)
		tabuliste[i][0] = -1;
}


/* nimmt loesung in liste auf */
void UpdateTabuliste(int wer, int wen)
{
	tabupos = (tabupos + 1) % tabulistenlaenge;
	tabuliste[tabupos][0] = wer;
	tabuliste[tabupos][1] = wen;
}


void TabuSearch (void)
{
	MSG msg;					// fuer User break
	int i,n;						// zaehlt durch die Punkte
	int point;					// aktueller Punkt
	int clusterneu,clusteralt;	// Cluster des Punktes 'point'
	int erfolg;
	unsigned int zaehler = 0;	// zaehlt die Schritte
	int letztererfolg = 0;
	int gezaehlte, nummer;
	int bestpoint, bestalt, bestneu;
	float sumtmp, sumbest;
	int *liste1 = (int*) malloc( (anzahl /4) * sizeof(int));
	int *liste2 = (int*) malloc( (anzahl /4) * sizeof(int));
	int *liste3 = (int*) malloc( (anzahl /4) * sizeof(int));

	sumbest = cluster->zfktwert = globsum;
	LoesungsCopy(cluster, best);// derzeit best
	ResetTabuliste();

	do	// laueft bis Stopkriterium
	{
		letztererfolg++;			// wann das letzte mal eine Lsg. akzeptiert
		zaehler++;					// jede Iteration zaehlen
		while (!consider[point = rand() % anzahl]) ;    // 0..anzahl-1
		n = 0;

		// bilde eine Liste von (anzahl / 5) Kandidaten, sofern soviele ex.
		gezaehlte = 0;
		nummer = 0;
		do
		{
			gezaehlte++;
			clusteralt = cluster->in_cluster[point];
			if (  ((clusterneu = Nachbarcluster(point, clusteralt)) != -1)
			   && ( (!Tabu(point, clusterneu))
				  // || Aspirate(point, clusterneu, clusteralt, sumbest)
				  )
			   )
			{
				liste1[nummer] = point;
				liste2[nummer] = clusteralt;
				liste3[nummer] = clusterneu;
				nummer++;
			}
			while (  (++gezaehlte<anzahl)
				  && (! consider[point = (point+1) % anzahl]) );
		}
		while ( (gezaehlte < anzahl) && (nummer < anzahl / 5) );
		// jetzt haben wir ein Array liste, das bis nummer voll ist mit
		// Kandidaten, wobei nummer maximal (anzahl / 5) ist

		// W"ahle nun den besten aus dieser Liste aus
		sumbest = realunendlich;
		erfolg = FALSE;
		for (i=0; i<nummer; i++)
		{
			UpdateCluster(liste1[i], liste2[i], liste3[i]);
			sumtmp = SSumme(liste1[i], liste2[i], liste3[i]);
			if (sumtmp < sumbest)
			{
				erfolg = TRUE;
				sumbest = sumtmp;
				bestpoint = liste1[i];
				bestneu   = liste2[i];
				bestalt   = liste3[i];
			}
			UpdateCluster(liste1[i], liste3[i], liste2[i]);
		}

		// mache dies zur aktuellen Loesung
		if (erfolg)
		{
			UpdateCluster(bestpoint, bestneu, bestalt);
			globsum = sumbest;
			UpdateTabuliste(bestpoint, bestneu);
			cluster->zfktwert = sumbest;
			Paintit(bestpoint,bestneu);
			Zielfunktionswert_ausgeben(sumbest);
			if (sumbest < best->zfktwert)	// beste Loesung ?
			{
				letztererfolg = 0;
				LoesungsCopy(cluster, best);
				ClearDerzeitAusgabe();
				DerzeitAusgabe(sumbest);
				ClearSimAnAusgabe();
				SimAnAusgabe(zaehler,
					(float) (GetTickCount()-dwoldtime) / 1000L);
			}
		}
		// naechster Punkt
		while ((++n<anzahl) && (! consider[point = (point+1) % anzahl]));

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
	while ( (n<anzahl) && (letztererfolg < 5*anzahl) );

	free(liste1); free(liste2); free(liste3);
}
















