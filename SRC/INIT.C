/************************************************************************
 *
 * INIT.C  -  Init-Prozeduren fuer das Projekt Diplom
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1994
 ************************************************************************/

#include <stdlib.h>
#include "global.h"
#include "init.h"

void Init (void)
{
	int i;

	winmaxy           = FENSTERGROESSE;
	eingelesen        = FALSE;				/* noch keine Datei    	   */
	first_time		  = TRUE;				// zur Speicher-Allokierung
	distanzen_aktuell = FALSE;
	nachbaranz_aktuell= FALSE;
	mit_KKreuz        = FALSE;
	clusteranz        = INIT_CLUSTERANZ;	/* gewaehlte # der Cluster */
	berechnetecluster = FALSE;
	nachbarschaft     = INIT_NACHBARSCHAFT;		/* 1= naechster cluster */
	algorithmus 	  = INIT_ALGORITHMUS;
	gerechnet  	      = FALSE;
	einzufaerben	  = FALSE;
	anzahlsubs		  = INIT_ANZAHLSUBS;
	simultaneous	  = FALSE;
	nachbaranz		  = INIT_NACHBARANZ;
	tabulistenlaenge  = INIT_TABULISTENLAENGE;
	tabufaktor		  = INIT_TABUFAKTOR;

	anzgr             = INIT_ANZGR;			/* zur Ausgabe		   */
	anzco             = anzgr;				/* # der gewaehlten Werte  */

	whichgr[0] = INIT_WHICHGR0;
	whichgr[1] = INIT_WHICHGR1;
	whichgr[2] = INIT_WHICHGR2;

	WICHTUNG[0] = INIT_WICHTUNG0;
	WICHTUNG[1] = INIT_WICHTUNG1;
	WICHTUNG[2] = INIT_WICHTUNG2;
	WICHTUNG[3] = INIT_WICHTUNG3;
	WICHTUNG[4] = INIT_WICHTUNG4;
	WICHTUNG[5] = INIT_WICHTUNG5;

	ctable[7] = RGB(128,0,0);	// dunkelrot
	ctable[8] = RGB(0,128,0);	// dunkelgruen
	ctable[9] = RGB(0,0,128);	// dunkelblau
	ctable[3] = RGB(128,128,128);	// mittelgrau
	ctable[4] = RGB(255,255,0);	// gelb
	ctable[10] = RGB(128,0,128);	// lila
	ctable[11] = RGB(0,128,128);	// dunkelcyan
	ctable[0] = RGB(255,0,0);	// hellrot
	ctable[1] = RGB(0,255,0);	// hellgruen
	ctable[2] = RGB(0,0,255);	// hellblau
	ctable[12] = RGB(70,70,20);	// braun
	ctable[5] = RGB(255,0,255);	// magenta
	ctable[6] = RGB(0,255,255);	// cyan
	ctable[13] = RGB(0,0,0);	// schwarz
	ctable[14] = RGB(255,255,255);// weiss
	ctable[15] = RGB(170,170,170);// hellgrau

	for (i=0; i<=MAXCLUSTER; i++)
		brush[i] = CreateSolidBrush(ctable[i]);
	for (i=0; i<=MAXCLUSTER; i++)
		pen[i] = CreatePen(PS_SOLID,1,ctable[i]);
}


