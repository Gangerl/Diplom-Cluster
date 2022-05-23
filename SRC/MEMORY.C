/************************************************************************
 * memory.c  -  Speicherroutinen fuer das Projekt Diplomarbeit
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1995, Oktober 1996
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "global.h"
#include "werror.h"
#include "memory.h"


/* Speicherfunktionen fuer Windoews */

/* wmalloc imitiert malloc
 */
VOID *wmalloc(LONG cbData)
{
	HANDLE hgm;
	hgm = GlobalAlloc(GMEM_MOVEABLE, cbData);
	if (!hgm) return NULL;
	return GlobalLock(hgm);
}

/* wfree imitiert free
 */
VOID wfree(void * pbData)
{
	HANDLE hgm = GlobalHandle(HIWORD(pbData));
	GlobalUnlock(hgm);
	GlobalFree(hgm);
}

/* wwrealloc imitiert wrealloc
 */
VOID  *wrealloc(void  * pbData, LONG cbData)
{
	HANDLE hgm = GlobalHandle(HIWORD(pbData));
	GlobalUnlock(hgm);
	GlobalReAlloc(hgm, cbData, GMEM_MOVEABLE);
	if (!hgm) return NULL;
	return GlobalLock(hgm);
}


/* allokiert den noetigen Speicherplatz fuer (fast) alle
 * dynamisch erzeugten Felder
 */
void HoleSpeicher (int anzahl, int werteanz)
{
	int i,j;

	DBMSG(("in HoleSpeicher"));
	/* input = eingelesene Daten als Integers */
	input   = (int  **) wmalloc (anzahl * sizeof(int *));
	if (!input)
		wError(hglobalwin, "Kein Speicher in  input");
	for (i=0; i<anzahl; i++)
	{
		input[i]   = (int  *) wmalloc (werteanz * sizeof(int));
		if (!input[i])
			wError(hglobalwin, "Kein Speicher in input %4d",i);
	}
	DBMSG(("nach Speicher fuer input"));

	/* modwert = eingelesene Daten als floats zwischen 0 und 1 */
	modwert = (float  **) wmalloc (anzahl * sizeof(float *));
	if (!modwert)
		wError(hglobalwin, "Kein Speicher in modwert");
	for (i=0; i<anzahl; i++)
	{
		modwert[i] = (float  *) wmalloc (werteanz * sizeof(float));
		if (!modwert[i])
			wError(hglobalwin, "Kein Speicher in modwert %4d",i);
	}
	DBMSG(("nach Speicher fuer modwert"));

	/* nachbar = naechste Punkte zu diesem */
	nachbar = (int  **) wmalloc (anzahl * sizeof(int *));
	if (!nachbar)
		wError(hglobalwin, "Kein Speicher in nachbar");
	for (i=0; i<anzahl; i++)
	{
		nachbar[i] = (int  *) wmalloc (MAXNACHBARANZ * sizeof(int));
		if (!nachbar[i])
			wError(hglobalwin, "kein Speicher in nachbar[i]: %2d",i);
	}


	/* whichco = Werte fuer Berechnung, maximal werteanz viele */
	whichco = (int  *) wmalloc (werteanz * sizeof(int));
	if (!whichco)
		wError(hglobalwin, "Kein Speicher in whichco");
	// whichco mit den Daten der Anzeige initialisieren
	whichco[0] = whichgr[0];
	whichco[1] = whichgr[1];
	whichco[2] = whichgr[2];

	/* big, small = groesste und kleinste Werte */
	big = (int  *) wmalloc (werteanz * sizeof(int));
	small = (int  *) wmalloc (werteanz * sizeof(int));
	if (!big || !small)
		wError(hglobalwin, "Kein Speicher in big oder small");

	/* header = Typen der Merkmale (int, float, string) */
	header  = (int  *) wmalloc (werteanz * sizeof(int));
	if (!header)
		wError(hglobalwin, "Kein Speicher in header");

	/* consider = Wird Punkt bei Einschraenkungen betrachtet? */
	consider = (int  *) wmalloc (anzahl * sizeof(int));
	if (!consider)
		wError(hglobalwin, "Kein Speicher in consider");
	// consider setzen, d.h. alle werden betrachtet
	for (i=0; i<anzahl; i++)
		consider[i] = 1;


	/* feldsubs = Bereichseinschraenkungen: welche werden angezeigt? */
	feldsubs = (int  **) wmalloc (werteanz * sizeof(int *));
	if (!feldsubs)
		wError(hglobalwin, "Kein Speicher in feldsubs");
	for (i=0; i < werteanz; i++)
	{
		feldsubs[i] = (int  *) wmalloc (sizeof(int));
		if (!feldsubs[i])
			wError(hglobalwin, "Kein Speicher in feldsubsi");
	}
	// feldsusbs setzen
	for (i=0; i<werteanz; i++)
		for (j=0; j < MAXSTRINGS; j++)
			feldsubs[i][j] = 0;

}


/* Legt Speicher fuer eine Clusterloesung an und gibt diesen zurueck
 */
void New_Loesung(Loesungszeiger *cl)
{
	int i;

	*cl = (Loesungszeiger) wmalloc (sizeof(Loesung));
	if (!(*cl))
		wError(hglobalwin, "Kein Speicher in New_Loesung 1");
	(**cl).in_cluster = (int *) wmalloc (anzahl * sizeof(int));
	(**cl).cluster = (int **) wmalloc (MAXCLUSTER * sizeof(int *));
	if (!(**cl).in_cluster || !(**cl).cluster)
		wError(hglobalwin, "Kein Speicher in New_Loesung 2");
	for (i=0; i<MAXCLUSTER; i++)
	{
		(**cl).cluster[i] = (int *) wmalloc (anzahl * sizeof(int));
		if (!(**cl).cluster[i])
			wError(hglobalwin, "Kein Speicher in New_Loesung 3");
	}

	(**cl).cluster[0][0] = 0; // Initialisierung
}


/* Gibt Speicher einer Clusterloesung frei
 */
void Dispose_Loesung(Loesungszeiger cl)
{
	int i;
	for (i=0; i<MAXCLUSTER; i++)
		wfree(cl->cluster[i]);
	wfree(cl->cluster);
	wfree(cl->in_cluster);
	wfree(cl);
}

/* gibt allen globalen dynamischen Speicher wieder frei
 */
void Zerstoere_Speicher (void)
{
	int i;
	for (i=0; i<anzahl; i++)
	{
		wfree(input[i]);
		wfree(modwert[i]);
		wfree(nachbar[i]);
	}
	wfree(input);
	wfree(modwert);
	wfree(nachbar);
	wfree(whichco);
	wfree(big);
	wfree(small);
	wfree(header);
	wfree(consider);
	wfree(feldsubs);
}




