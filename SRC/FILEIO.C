/************************************************************************
 * FILEIO:C  -  stdio-Routinen fuer das Projekt Diplomarbeit
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1994
 *
 ************************************************************************/

#include <windowsx.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include "global.h"
#include "fileio.h"
#include "werror.h"
#include "memory.h"

#define TRENNER ';'
#define DEFAULT 1	/* bei Strings : nichts */


FILE	*pfile;
LPSTR lpstrfile;
char	satz[255];
char    st[STRINGLAENGE];
static char    *help;
int	laenge;
HWND handle;

static int Lese_Zeile (void)
{
	int ok = TRUE;

	do {
		ok = fscanf(pfile,"%s",satz);
	} while (satz[0] == '#');				// Kommentarzeilen

	laenge = strlen(satz);
	return ok;
}


static int Lese_Header (void)
{
	int i;
	char wert[1];
	int pos, anf;

	/* anzahl */
	satz[0] ='\0';
	if (!Lese_Zeile()) return FALSE;
	anzahl = atoi(satz);

	/* Typen */
	satz[0] = '\0';
	if (!Lese_Zeile()) return FALSE;
	werteanz = laenge;

	/* allokieren */
	HoleSpeicher(anzahl, werteanz);
	New_Loesung(&cluster);
	New_Loesung(&test);
	New_Loesung(&best);
	DBMSG(("nach HoleSpeicher"));

	for (i=0; i<werteanz; i++)
	{
		wert[0] = satz[i];
		header[i] = atoi(wert);
		//header[i] = atoi(satz[i]); // no warning!
	}


	/* Titelzeile */
	satz[0] = '\0';
	if (!Lese_Zeile()) return FALSE;
	pos = 0;
	for (i=0; i<werteanz; i++)
	{
		anf = pos;
		while ((satz[pos] != TRENNER) && (pos < laenge) )
			(pos)++;
		help = &(satz[anf]);
		strncpy(title[i], help, pos-anf);
		if ((i < werteanz-1) && (pos >= laenge) )
			werror(hglobalwin, "Lesefehler: Falsche Titelzeile in der Datei");
		(pos)++;			/* uebers Semikolon */
	}
	return TRUE;
}


static int lese_int (int i, int j, int *pos)
{
	int anf = *pos;

	while ((satz[*pos] != TRENNER) && (*pos < laenge) )
		(*pos)++;
	help = &(satz[anf]);
	strnset(st, 0, STRINGLAENGE);
	strncpy(st, help, *pos-anf);
	input[i][j] = atoi(st);
	(*pos)++;
	return !( (j < werteanz-1) && (*pos >= laenge) );
}



static int lese_flo (int i, int j, int *pos)
{
	float floatwert;
	int anf = *pos;

	while ((satz[*pos] != TRENNER) && (*pos < laenge) )
		(*pos)++;

	help = &(satz[anf]);
	strnset(st, 0, STRINGLAENGE);
	strncpy(st, help, *pos-anf);
	floatwert = atof(st);
	input[i][j] = (int)(1000 * floatwert);
	(*pos)++;
	return !( (j < werteanz-1) && (*pos >= laenge) );
}


static int Kein_Wert (int i, int j, int *pos)
{
	if (satz[*pos] == TRENNER)
	{
		(*pos)++;
		return (input[i][j] = DEFAULT);
	}
	return FALSE;
}


static char * Lese_String_aus_satz (int *pos)
{
	int anf=*pos;

	while ((satz[*pos] != TRENNER) && (*pos < laenge) )
		(*pos)++;
	return &(satz[anf]);
}


static int lese_str (int i, int j, int *pos)
{
	int gef = FALSE;
	int k;
	int anf=*pos;

	if (!Kein_Wert(i,j,pos))
	{
		help = Lese_String_aus_satz(pos);
		strnset(st, 0, STRINGLAENGE);
		strncpy(st, help, *pos-anf);
		for (k=0; k<bislang[j]; k++)
			if (!strcmp(st,text[j][k])) 	/* d.h. vorhanden */
				input[i][j] = k+OFFSET, gef = TRUE;

		if (!gef)    				/* neuer Wert */
		{
			strcpy(text[j][bislang[j]], st);
			input[i][j] = bislang[j]++ + OFFSET;
		}
		(*pos)++;
	}
	return !( (j < werteanz-1) && (*pos >= laenge) );
}


static int Compute_big_and_small (void)
{
	int i,j;

	for (i=0; i<werteanz; i++)
	{
		big[i]   = 0;
		small[i] = unendlich;
		for (j=0; j<anzahl; j++)
		{
			if (input[j][i] > big[i])
				big[i] = input[j][i];
			if (input[j][i] < small[i])
				small[i] = input[j][i];
		}
		 /* evtl durch Null teilen */
		if ( (big[i] == 0) || (big[i] == small[i]) )
		{
			werror(hglobalwin, "Lesefehler: Feld %4d enthaelt keine signifikaten Daten",i);
			return FALSE;
		}
		small[i]--; big[i]++;		/* huebscher */
	}
	return TRUE;
}


void Readdatei(void)
{
	int i, j;
	int ok = TRUE;
	int pos;
	char szName[256];
	OPENFILENAME ofnTemp;
	DWORD Errval; // Error value
	char buf[5];  // Error buffer
	char Errstr[50]="GetOpenFileName returned Error #";
	char szTemp[] = "All Files (*.*)\0*.*\0Text Files (*.txt)\0*.txt\0";

	handle = hglobalwin;
	strcpy(szName,"");

	ofnTemp.lStructSize = sizeof( OPENFILENAME );
	ofnTemp.hwndOwner = hglobalwin; // An invalid hWnd causes non-modality
	ofnTemp.hInstance = 0;
	ofnTemp.lpstrFilter = (LPSTR)szTemp;  // See previous note concerning string
	ofnTemp.lpstrCustomFilter = NULL;
	ofnTemp.nMaxCustFilter = 0;
	ofnTemp.nFilterIndex = 1;
	ofnTemp.lpstrFile = (LPSTR)szName;  // Stores the result in this variable
	ofnTemp.nMaxFile = sizeof( szName );
	ofnTemp.lpstrFileTitle = NULL;
	ofnTemp.nMaxFileTitle = 0;
	ofnTemp.lpstrInitialDir = NULL;
	ofnTemp.lpstrTitle = "Datei lesen";  // Title for dialog
	ofnTemp.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	ofnTemp.nFileOffset = 0;
	ofnTemp.nFileExtension = 0;
	ofnTemp.lpstrDefExt = "*";
	ofnTemp.lCustData = NULL;
	ofnTemp.lpfnHook = NULL;
	ofnTemp.lpTemplateName = NULL;

	/*
	If the call to GetOpenFileName() fails you can call CommDlgExtendedError()
	to retrieve the type of error that occured.
	*/
	if(GetOpenFileName( &ofnTemp ) != TRUE)
	{
		Errval=CommDlgExtendedError();
		if(Errval!=0) // 0 value means user selected Cancel
		{
			sprintf(buf,"%ld",Errval);
			strcat(Errstr,buf);
			werror(hglobalwin, Errstr);
		}
	}

	/* File oeffnen */
	if ( (pfile = fopen(szName, "r")) == NULL)
		goto DATEI_NICHT_GEFUNDEN;

	/* Speicher freigeben, falls noetig */
	if (!first_time)
	{
		Zerstoere_Speicher();
		Dispose_Loesung(cluster);
		Dispose_Loesung(test);
		Dispose_Loesung(best);
	}
	/* Einfuehrende Zeilen lesen */
	if (!Lese_Header()) MessageBeep(0);
	DBMSG(("nach LeseHeader"));

	/* Komponenten lesen */
	for (i=0; i<anzahl; i++)
	{
		pos = 0;
		if (!Lese_Zeile()) MessageBeep(0);
		for (j=0; j<werteanz; j++)
		{
			switch (header[j])
			{
			case INT_: ok = lese_int(i,j,&pos);
			break;
			case STR_: ok = lese_str(i,j,&pos);
			break;
			case FLO_: ok = lese_flo(i,j,&pos);
			break;
			}
			if (!ok) {werror(hglobalwin, "Lesefehler: Daten entsprechen nicht der Vorschrift");}
		}
	}
	fclose(pfile);
	DBMSG(("nach fclose"));

	/* Daten brauchbar ? */
	if (!Compute_big_and_small())
		return ;

	/* alles klar */
	eingelesen = TRUE;
	return ;

	/* Label */
	LESEFEHLER:
	werror(hglobalwin, "Lesefehler");
	if (eingelesen)
		werror(hglobalwin, "arbeite auf alten Daten weiter");
	return;

	/* Label */
	DATEI_NICHT_GEFUNDEN:
	if (eingelesen)
		werror(hglobalwin, "arbeite auf alten Daten weiter");
	return;
}

