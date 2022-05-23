/**************************************************************************
 * GLOBAL.H  -  globaler Header
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1994
 **************************************************************************/

#include <windows.h>
#include "defines.h"
#include "types.h"					// enthaelt bereits defines.h

#ifndef GLOBAL
#define GLOBAL

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

/* Windows-Ausgabe */
EXTERN HANDLE hglobalwin;			// handle aufs mainwindow
EXTERN HDC 	  hdc;					// handle aufs mainwindow
EXTERN HANDLE  hInstance;			// Instance der Applikation;
EXTERN HBRUSH brush[MAXCLUSTER+1];	// Array von Malfarben
EXTERN HPEN   pen[MAXCLUSTER+1];	// Array von Schriftfarben
EXTERN HPEN   textpen;				// Farbe der Schrift (weiss oder schwarz)
EXTERN short  winmaxy;				// Groesse des Fensters in y-Richtung
EXTERN char buf[25];				// Hilfsstring fuer Debug und Dialoge

/* Bool´s */
EXTERN int first_time;				// zeigt, ob zum ersten Mal Datei geoeffnet
EXTERN int eingelesen;        		// wenn true war einlesen erfolgreich
EXTERN int gerechnet;				// ob schon eine Berechung vorliegt
EXTERN int distanzen_aktuell;		// bei Veraenderung der Werte wichtig
EXTERN int nachbaranz_aktuell;		// Wenn Anzahl vergroessert wurde wichtig
EXTERN int mit_KKreuz;				// Design-Frage
EXTERN int einzufaerben;			// Special: einfaerben nach y-Koordinaten
EXTERN int simultaneous;			// Nachbarschaft

/* Paramter */
EXTERN int anzahl;			    	/* Anzahl der Records i    */
EXTERN int werteanz;          		/* # der gelesenen Werte   */
EXTERN int clusteranz;        		/* gewaehlte # der Cluster */
EXTERN int berechnetecluster;		// Rueckgabe aus 'compute', Anzeige ok?
EXTERN int anfangspartition;  		/* aufsteigend modulo      */
EXTERN int nachbarschaft;     		/* 1= naechster cluster    */
EXTERN int algorithmus;				// definiert, welcher Algorithmus
EXTERN int nachbaranz;				// Anzahl der Nachbarn bei nearest cluster
EXTERN int tabulistenlaenge;		// Laenge der Tabu-Liste
EXTERN float tabufaktor;			// Faktor fuer das aspiration criterion


/* Anzeige und Berechnung */
EXTERN int anzco;             	    // # der gewaehlten Werte
EXTERN int anzgr;             		// zur Ausgabe
EXTERN int *whichco; 		    	// welche Werte zur Berechnung /
EXTERN int whichgr[3];		    	// Ausgabe ausgewaehlt wurden
EXTERN float mittel[MAXWERTE];		// aktuell berechneter Centroid: Werte
EXTERN float bestes_mittel[MAXWERTE];	// bester Centroid: Werte
EXTERN int anzahlsubs;				// Anzahl der markierten Unterbereiche
EXTERN int **feldsubs;				// welche Unterbereiche sind gesetzt
EXTERN int *consider;				// wird Datensatz beruecksichtigt?
EXTERN int **nachbar;				// Nachbarn
EXTERN DWORD dwoldtime, dwnewtime;	// Zeiten zur Anzeige
EXTERN char szmeldungstext[15];		// enthaelt Ende-Meldung der Berechnung
EXTERN float globsum;				// aktuelle globale Summe!

/* einlesen */
EXTERN int   *header;				/* Typen zum einlesen      */
EXTERN char  title[110][STRINGLAENGE];	/* enthaelt die Ueberschriften  */
EXTERN int   **input;		 	    	/* alle Daten              */
EXTERN float **modwert; 			/* Werte zwischen 0 & 1    */
EXTERN int   *big;		 	    	/* groesste Werte          */
EXTERN int   *small; 			    	/* kleinste Werte          */
EXTERN char	 text[MAXWERTE][MAXSTRINGS][STRINGLAENGE];  // strings der Eingabe
EXTERN int 	 bislang[MAXWERTE];     // anzahl der verschiedenen strings

/* Zielfunktionen */
EXTERN int   WICHTUNG[6];

/* Clusterloesungen */
EXTERN Loesungszeiger cluster,			/* aktuelle Clusteranordnung */
					  best,				/* beste */
					  test;				/* testende */

/* Farbtabelle */
EXTERN long  ctable[MAXCLUSTER+1];      	/* Farben 		   */

#endif
