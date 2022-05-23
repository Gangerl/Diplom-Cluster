/************************************************************************
 * DIALOG.C  --  Windows-Dialoge
 *
 * (c) Wolfgang Lauffher
 *
 * 1995
 ************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"
#include "global.h"
#include "compute.h"
#include "dialog.h"
#include "graphio.h"


/* About-Dialog-Procedure
 */
BOOL FAR PASCAL _export AboutDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	lParam = lParam;	// keine warning
	switch(message)
	{
	case WM_INITDIALOG :
		return TRUE;
	case WM_COMMAND :
		switch (wParam)
		{
		case IDOK :
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		}
	break;
	}
	return FALSE;
}

/* Wichtung-Dialog-Procedure
 */
BOOL FAR PASCAL _export WichtungDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	int i,j;
	char tmp[30];
	static HANDLE hwList[6];

	lParam = lParam;	// keine warning
	switch(message)
	{
	case WM_INITDIALOG :
		for (i=IDD_WSS; i<=IDD_Wmaxmax; i++)
		{
			hwList[i-IDD_WSS] = GetDlgItem(hDlg, i);
			SendMessage(hwList[i-IDD_WSS], LB_RESETCONTENT,0,0);
			for (j=0; j<21; j++)
			{
				sprintf(tmp, "%3d", j);
				SendMessage(hwList[i-IDD_WSS], LB_ADDSTRING,0,(LONG)(LPSTR)tmp);
			}
			SendMessage(hwList[i-IDD_WSS], WM_SETREDRAW, TRUE, 0L);
			SendMessage(hwList[i-IDD_WSS], LB_SETCURSEL, WICHTUNG[i-IDD_WSS], 0);
		}
		return TRUE;
	case WM_COMMAND :
		switch (wParam)
		{
		case IDOK :
			for (i=IDD_WSS; i<=IDD_Wmaxmax; i++)
			{
				WICHTUNG[i-IDD_WSS] =
					(int) SendMessage(hwList[i-IDD_WSS],LB_GETCURSEL, 0, 0L);
			}
			// aktuellen Zielfunktionswert ausgeben
			if (gerechnet)
				InvalidateRect(hglobalwin, NULL, TRUE);
			EndDialog(hDlg, 0);
			return TRUE;
		case IDD_WICHTUNGHELP:
			WinHelp(hglobalwin, szHelpName, HELP_CONTEXT, Zielfunktionen__Dialog);
			return 0;
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		}
	break;
	}
	return FALSE;
}


/* Optionen-Dialog-Procedure
 */
BOOL FAR PASCAL _export OptionenDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	HANDLE 	hwClusterLB,
			hwNachbaranzLB,
			hwTabulisteLB,
			hwTabufaktor;
	int 		i;
	int 		merker;
	char		tmp[30];
	static int 	localsimultaneous,
				localnachbar,
				alte_nachbaranz, 	// bei neuem, groesserem Wert neu berechnen!
				localalgo;
	float		localtabufaktor;

	lParam = lParam;
	hwClusterLB = GetDlgItem(hDlg, IDD_CLUSTER);		// LB=ListBox
	hwNachbaranzLB = GetDlgItem(hDlg, IDD_NACHBARanz);
	hwTabulisteLB = GetDlgItem(hDlg, IDD_TABUliste);
	hwTabufaktor = GetDlgItem(hDlg, IDD_TABUfaktor);

	switch(message)
	{
	case WM_INITDIALOG :
		alte_nachbaranz   = nachbaranz;
		localtabufaktor   = tabufaktor;
		localsimultaneous = simultaneous;
		localnachbar      = nachbarschaft;
		localalgo	   	  = algorithmus;

		/* Initialisieren */
		CheckDlgButton(hDlg, IDD_SIMULTANEOUS, localsimultaneous);
		CheckRadioButton(hDlg, IDD_NACHBARnaechster,
			IDD_NACHBARrandom, nachbarschaft);
		CheckRadioButton(hDlg, IDD_LOCAL,
			IDD_TABU, algorithmus);
		SendMessage(hwClusterLB, LB_RESETCONTENT,0,0);
		SendMessage(hwNachbaranzLB, LB_RESETCONTENT,0,0);
		SendMessage(hwTabulisteLB, LB_RESETCONTENT,0,0);

		/* Abhaengigkeitem pruefen */
		if (localnachbar != IDD_NACHBARnaechster )
		{
			EnableWindow(hwNachbaranzLB, FALSE);
			EnableWindow(GetDlgItem(hDlg, IDD_TEXTNACHBAR), FALSE);
		}
		if (localsimultaneous)
		{
			EnableWindow(GetDlgItem(hDlg,IDD_TABU), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_THRESHOLD), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_SIMAN), FALSE);
			EnableWindow(hwTabulisteLB, FALSE);
			EnableWindow(hwTabufaktor,  FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_NACHBARrandom), FALSE);
		}

		switch (localalgo)
		{
		case IDD_TABU :
		case IDD_SIMAN :
		case IDD_THRESHOLD :
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), FALSE);
		}

		if (localalgo != IDD_TABU)
		{
			EnableWindow(hwTabulisteLB, FALSE);
			EnableWindow(hwTabufaktor, FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), FALSE);
		}


		/* Listbox clusteranz */
		for (i=2; i < MAXCLUSTER; i++)
		{
			sprintf(tmp, "%3d", i);
			SendMessage(hwClusterLB, LB_ADDSTRING,0,(LONG)(LPSTR)tmp);
		}
		SendMessage(hwClusterLB, WM_SETREDRAW, TRUE, 0L);
		SendMessage(hwClusterLB, LB_SETCURSEL, clusteranz-2, 0);
		SetFocus(hwClusterLB);

		/* Listbox nachbaranz */
		if (nachbaranz == (MAXNACHBARANZ > anzahl ? anzahl : MAXNACHBARANZ) )
			nachbaranz = 0;
		if (anzahl > 0)
			SendMessage(hwNachbaranzLB, LB_ADDSTRING,0,(LONG)(LPSTR)"alle");
		for (i=1; i < (MAXNACHBARANZ > anzahl ? anzahl : MAXNACHBARANZ); i++)
		{
			sprintf(tmp, "%3d", i);
			SendMessage(hwNachbaranzLB, LB_ADDSTRING,0,(LONG)(LPSTR)tmp);
		}
		SendMessage(hwNachbaranzLB, WM_SETREDRAW, TRUE, 0L);
		SendMessage(hwNachbaranzLB, LB_SETCURSEL, nachbaranz, 0);

		/* Listbox Tabulistenlaenge */
		merker = (anzahl -1) * clusteranz;
		for (i=1; i <= (MAXTABULISTENLAENGE > merker ? merker : MAXTABULISTENLAENGE); i++)
		{
			sprintf(tmp, "%3d", i);
			SendMessage(hwTabulisteLB, LB_ADDSTRING,0,(LONG)(LPSTR)tmp);
		}
		SendMessage(hwTabulisteLB, WM_SETREDRAW, TRUE, 0L);
		SendMessage(hwTabulisteLB, LB_SETCURSEL, tabulistenlaenge-1, 0);

		/* Edittext Tabufaktor = aspiration criterion factor */
		sprintf(tmp, "%1.2f",localtabufaktor);
		SendMessage(hwTabufaktor, WM_SETTEXT,0,LONGADDR(tmp));

		return TRUE;
	case WM_COMMAND :
		switch (wParam)
		{
		case IDOK :
			/* 3 Listboxen */
			clusteranz  = 2 + (int) SendMessage(hwClusterLB,LB_GETCURSEL, 0, 0L);
			nachbaranz  = (int) SendMessage(hwNachbaranzLB, LB_GETCURSEL, 0, 0L);
			if (nachbaranz == 0)	// bei 0 auf alle, bzw. MAX setzen
				nachbaranz = (MAXNACHBARANZ > anzahl ? anzahl : MAXNACHBARANZ);
			if (nachbaranz > alte_nachbaranz)	// nur dann neue Nachbarn berechnen
				nachbaranz_aktuell = FALSE;
			tabulistenlaenge = (int) SendMessage(hwTabulisteLB, LB_GETCURSEL, 0, 0L) +1;

			/* 1 Edittext */
			SendMessage(hwTabufaktor, WM_GETTEXT, sizeof(tmp), LONGADDR(tmp));
			tabufaktor = atof(tmp);

			/* die Autoradiobuttons */
			nachbarschaft = localnachbar;
			algorithmus   = localalgo;

			/* 1 Checkbox */
			simultaneous = localsimultaneous;

			EndDialog(hDlg, TRUE);
			return TRUE;
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		case IDD_OPTIONENHELP :
			WinHelp(hglobalwin, szHelpName, HELP_CONTEXT, Optionen__Dialog);
			return TRUE;
		case IDD_NACHBARnaechster :
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), TRUE);
			EnableWindow(hwNachbaranzLB, TRUE);
			EnableWindow(GetDlgItem(hDlg, IDD_TEXTNACHBAR), TRUE);
			localnachbar = wParam;
			CheckRadioButton(hDlg, IDD_NACHBARnaechster,
			IDD_NACHBARrandom, localnachbar);
			return 0;
		case IDD_NACHBARrandom :
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), FALSE);
			EnableWindow(hwNachbaranzLB, FALSE);
			EnableWindow(GetDlgItem(hDlg, IDD_TEXTNACHBAR), FALSE);
			localnachbar = wParam;
			CheckRadioButton(hDlg, IDD_NACHBARnaechster,
			IDD_NACHBARrandom, localnachbar);
			return 0;
		case IDD_NACHBARcentroid :
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), TRUE);
			EnableWindow(hwNachbaranzLB, FALSE);
			EnableWindow(GetDlgItem(hDlg, IDD_TEXTNACHBAR), FALSE);
			localnachbar = wParam;
			CheckRadioButton(hDlg, IDD_NACHBARnaechster,
			IDD_NACHBARrandom, localnachbar);
			return 0;
				EnableWindow(GetDlgItem(hDlg,IDD_TABU), FALSE);
		case IDD_ITERATIVE :
		case IDD_SIMANLOCAL :
		case IDD_COMBINED :
			localalgo = wParam;
			CheckRadioButton(hDlg, IDD_LOCAL,IDD_TABU, localalgo);
			EnableWindow(hwTabufaktor,  FALSE);
			EnableWindow(hwTabulisteLB, FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), TRUE);
			return 0;
		case IDD_TABU :
			localalgo = wParam;
			CheckRadioButton(hDlg, IDD_LOCAL,IDD_TABU, localalgo);
			EnableWindow(hwTabulisteLB, TRUE);
			EnableWindow(hwTabufaktor,  TRUE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), TRUE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), FALSE);
			return 0;
		case IDD_LOCAL :
		case IDD_SIMAN :
		case IDD_THRESHOLD :
			localalgo = wParam;
			CheckRadioButton(hDlg, IDD_LOCAL,IDD_TABU, localalgo);
			EnableWindow(hwTabulisteLB, FALSE);
			EnableWindow(hwTabufaktor,  FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDD_SIMULTANEOUS), FALSE);
			return 0;
		case IDD_SIMULTANEOUS :
			localsimultaneous = !localsimultaneous;
			CheckDlgButton (hDlg, IDD_SIMULTANEOUS, localsimultaneous);
			if (localsimultaneous)
			{
				EnableWindow(GetDlgItem(hDlg,IDD_TABU), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_LOCAL), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_THRESHOLD), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_SIMAN), FALSE);
				EnableWindow(hwTabulisteLB, FALSE);
				EnableWindow(hwTabufaktor,  FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), FALSE);
				EnableWindow(GetDlgItem(hDlg,IDD_NACHBARrandom), FALSE);
			}
			else
			{
				EnableWindow(GetDlgItem(hDlg,IDD_LOCAL), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDD_TABU), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDD_THRESHOLD), TRUE);
				EnableWindow(GetDlgItem(hDlg,IDD_SIMAN), TRUE);
				if (localalgo == IDD_TABU)
				{
					EnableWindow(hwTabulisteLB, TRUE);
					EnableWindow(hwTabufaktor,  TRUE);
					EnableWindow(GetDlgItem(hDlg,IDD_TEXTFAKTOR), TRUE);
					EnableWindow(GetDlgItem(hDlg,IDD_TEXTLISTE), TRUE);
				}
				EnableWindow(GetDlgItem(hDlg,IDD_NACHBARrandom), TRUE);
			}
			return 0;
		case IDD_WICHTUNG :
			DialogBox(hInstance, "Wichtung", hDlg, lpfnWichtungDlgProc);
			return 0;
		case IDD_NACHBARanz :
		case IDD_TABUliste :
		case IDD_TABUfaktor:
			return 0;
		}
	break;
	}
	return FALSE;
}


/* Werte-Dialog-Procedure
 */
BOOL FAR PASCAL _export WerteDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	static int	wertetake = FALSE;
	int 	i, help;
	HANDLE hwListgr1, hwListgr2, hwListgr3,
		 hwListco1, hwListco2, hwListco3, hwListco;

	lParam = lParam;
	hwListgr1 = GetDlgItem(hDlg, IDD_LISTEGR1);
	hwListgr2 = GetDlgItem(hDlg, IDD_LISTEGR2);
	hwListgr3 = GetDlgItem(hDlg, IDD_LISTEGR3);
	hwListco  = GetDlgItem(hDlg, IDD_LISTECO );
	hwListco1 = GetDlgItem(hDlg, IDD_LISTECO1);
	hwListco2 = GetDlgItem(hDlg, IDD_LISTECO2);
	hwListco3 = GetDlgItem(hDlg, IDD_LISTECO3);
	switch(message)
	{
	case WM_INITDIALOG :
		CheckDlgButton(hDlg, IDD_WERTETAKE, wertetake);
		CheckDlgButton(hDlg, IDD_SPECIAL, einzufaerben);
		/* ListeGR1 */
		SendMessage(hwListgr1, CB_RESETCONTENT,0,0);
		for (i=0; i < werteanz; i++)
			SendMessage(hwListgr1, CB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		SendMessage(hwListgr1, CB_SETCURSEL, whichgr[0], 0);
		/* ListeGR2 */
		SendMessage(hwListgr2, CB_RESETCONTENT,0,0);
		SendMessage(hwListgr2, CB_ADDSTRING, 0, (LONG)(LPSTR)"[kein]");
		for (i=0; i < werteanz; i++)
			SendMessage(hwListgr2, CB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		help = anzgr > 1 ? whichgr[1]+1 : 0;
			SendMessage(hwListgr2, CB_SETCURSEL, help, 0);
		/* ListeGR3 */
		SendMessage(hwListgr3, CB_RESETCONTENT,0,0);
		SendMessage(hwListgr3, CB_ADDSTRING, 0, (LONG)(LPSTR)"[kein]");
		for (i=0; i < werteanz; i++)
			SendMessage(hwListgr3, CB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		help = anzgr > 2 ? whichgr[2]+1 : 0;
			SendMessage(hwListgr3, CB_SETCURSEL, help, 0);
		/* ListeCO */
		SendMessage(hwListco, LB_RESETCONTENT,0,0);
		for (i=0; i < werteanz; i++)
			SendMessage(hwListco, LB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		for (i=3; i < anzco; i++)
			SendMessage(hwListco, LB_SETSEL, 1, MAKELONG(whichco[i], 0));
		/* ListeCO1 */
		SendMessage(hwListco1, CB_RESETCONTENT,0,0);
		for (i=0; i < werteanz; i++)
			SendMessage(hwListco1, CB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
			SendMessage(hwListco1, CB_SETCURSEL, whichco[0], 0);
		/* ListeCO2 */
		SendMessage(hwListco2, CB_RESETCONTENT,0,0);
		SendMessage(hwListco2, CB_ADDSTRING, 0, (LONG)(LPSTR)"[kein]");
		for (i=0; i < werteanz; i++)
			SendMessage(hwListco2, CB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		help = anzco > 1 ? whichco[1]+1 : 0;
			SendMessage(hwListco2, CB_SETCURSEL, help, 0);
		/* ListeCO3 */
		SendMessage(hwListco3, CB_RESETCONTENT,0,0);
		SendMessage(hwListco3, CB_ADDSTRING, 0, (LONG)(LPSTR)"[kein]");
		for (i=0; i < werteanz; i++)
			SendMessage(hwListco3, CB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		help = anzco > 2 ? whichco[2]+1 : 0;
			SendMessage(hwListco3, CB_SETCURSEL, help, 0);
		EnableWindow(hwListco1,!wertetake);
		EnableWindow(hwListco2,!wertetake);
		EnableWindow(hwListco3,!wertetake);
		EnableWindow(hwListco ,!wertetake);
		return TRUE;
	case WM_COMMAND :
		switch (wParam)
		{
		case IDOK :
			/* Werte fuer Ausgabe */
			whichgr[0] = (int)SendMessage(hwListgr1, CB_GETCURSEL, 0, 0L);
			if ( (help = (int)SendMessage(hwListgr2, CB_GETCURSEL, 0, 0L))
				!= 0)
			{
				whichgr[1] = help-1;
				anzgr = 2;
				if ( (help = (int)SendMessage(hwListgr3,
					CB_GETCURSEL, 0, 0L)) != 0)
				{
					whichgr[2] = help -1;
					anzgr = 3;
				}
			}
			else anzgr = 1;
			/* Werte fuer Berechnung */
			whichco[0] = (int)SendMessage(hwListco1, CB_GETCURSEL, 0, 0L);
			if ( (help = (int)SendMessage(hwListco2, CB_GETCURSEL, 0, 0L))
				!= 0)		// mehr als einer
			{
				whichco[1] = help-1;
				anzco = 2;
				if ( (help = (int)SendMessage(hwListco3,
					CB_GETCURSEL, 0, 0L)) != 0)		// mehr als zwei
				{
					whichco[2] = help -1;
					// evtl. noch mehr in der Listbox?
					anzco = 3 + (int)SendMessage(hwListco,
						LB_GETSELITEMS, MAXCLUSTER, (LONG)(LPSTR)(&whichco[3]));
				}
			}
			else anzco = 1;
			EndDialog(hDlg, 0);
			distanzen_aktuell = FALSE;
			return TRUE;
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		case IDD_WERTEHELP:
			WinHelp(hglobalwin, szHelpName, HELP_CONTEXT, Werte__Dialog);
			return 0;
		case IDD_WERTERESET:
			SendMessage(hDlg, WM_INITDIALOG, WM_INITDIALOG, lParam);
			return 0;
		case IDD_WERTETAKE :
			wertetake = !wertetake;
			CheckDlgButton(hDlg, IDD_WERTETAKE, wertetake);
			if (wertetake)
			{
				SendMessage(hwListco1, CB_SETCURSEL, (int)SendMessage(
					hwListgr1, CB_GETCURSEL, 0, 0L), 0);
				SendMessage(hwListco2, CB_SETCURSEL, (int)SendMessage(
					hwListgr2, CB_GETCURSEL, 0, 0L), 0);
				SendMessage(hwListco3, CB_SETCURSEL, (int)SendMessage(
					hwListgr3, CB_GETCURSEL, 0, 0L), 0);
				SendMessage(hwListco, CB_SETCURSEL, 0, 0);
			}
			EnableWindow(hwListco1,!wertetake);
			EnableWindow(hwListco2,!wertetake);
			EnableWindow(hwListco3,!wertetake);
			EnableWindow(hwListco ,!wertetake);
			return 0;
		case IDD_SPECIAL:
			einzufaerben = !einzufaerben;
			CheckDlgButton(hDlg, IDD_SPECIAL, einzufaerben);
			if (einzufaerben)
			{
				einfaerben(best);
				InvalidateRect(hglobalwin, NULL, TRUE);
			}
			return TRUE;
		case IDD_SUBBEREICHE :
			DialogBox(hInstance, "SubBereiche", hDlg, lpfnSubBereicheDlgProc);
			return 0;
		case IDD_LISTECO :
			return 0;
		case IDD_LISTEGR1 :
			if (wertetake)
				SendMessage(hwListco1, CB_SETCURSEL, (int)SendMessage(
					hwListgr1, CB_GETCURSEL, 0, 0L), 0);
			return 0;
		case IDD_LISTEGR2 :
			if (wertetake)
				SendMessage(hwListco2, CB_SETCURSEL, (int)SendMessage(
					hwListgr2, CB_GETCURSEL, 0, 0L), 0);
			return 0;
		case IDD_LISTEGR3 :
			if (wertetake)
				SendMessage(hwListco3, CB_SETCURSEL, (int)SendMessage(
					hwListgr3, CB_GETCURSEL, 0, 0L), 0);
			return 0;
		case IDD_LISTECO1 :
		case IDD_LISTECO2 :
		case IDD_LISTECO3 :
			return 0;
		}
	break;
	}
	return FALSE;
}


/* Cluster-Ausgabe fuer nachfolgende Prozedur
 */
void Idm__auscluster(HANDLE hwBrowse)
{
	int i,j,k;
	char zahl[20], tmp[100];

	SendMessage(hwBrowse, LB_RESETCONTENT,0,0);
	strcpy(tmp,"Cluster\t lfd# ");
	for (i=0; i<(anzco<6?anzco:5); i++)
	{
		strcat (tmp,"\t ");
		strncat(tmp,title[whichco[i]],7);
	}
	SendMessage(hwBrowse, LB_ADDSTRING, 0, (LONG)(LPSTR)tmp);
	for (i=0; i < clusteranz; i++)
	  for (j=0; j < best->anz_in_cluster[i]; j++)
	  {
		strcpy(tmp,itoa(i,zahl,10));
		strcat(tmp,"\t ");
		strcat(tmp,itoa(input[best->cluster[i][j]][0],zahl,10));
		for (k=0; k<(anzco<6?anzco:5); k++)
		{
			strcat(tmp,"\t ");
			strcat(tmp,itoa(input[best->cluster[i][j]][whichco[k]],zahl,10));
		}
		SendMessage(hwBrowse, LB_ADDSTRING,0,(LONG)(LPSTR)tmp);
	  }
}


/* Werte-Ausgabe fuer nachfolgende Prozedur
 */
void Idm__auswerte (HANDLE hwBrowse)
{
	int i,j;
	char tmp[200],zahl[20];

	SendMessage(hwBrowse, LB_RESETCONTENT,0,0);
	strcpy (tmp,"lfd# ");
	for (i=0; i<(anzco<6?anzco:5); i++)
	{
		strcat (tmp,"\t ");
		strncat(tmp,title[whichco[i]],7);
	}
	strcat(tmp,"\t Cluster");
	SendMessage(hwBrowse, LB_ADDSTRING, 0, (LONG)(LPSTR)tmp);
	for (i=0; i < anzahl; i++)
	  if (consider[i])
	{
		strcpy(tmp,itoa(input[i][0],zahl,10));
		for (j=0; j<(anzco<6?anzco:5); j++)
		{
			strcat(tmp,"\t ");
			strcat(tmp,itoa(input[i][whichco[j]],zahl,10));
		}
		strcat(tmp,"\t ");
		strcat(tmp,itoa(best->in_cluster[i],zahl,10));
		SendMessage(hwBrowse, LB_ADDSTRING,0,(LONG)(LPSTR)tmp);
	}
}


/* Ausgabe-Dialog-Procdure
 */
BOOL FAR PASCAL _export AusgabeDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	HANDLE hwBrowse;
	const char szauscluster[40] = "CLUSTERING: Ausgabe nach Clustern";
	const char szauswerte[40]   = "CLUSTERING: Ausgabe nach lfd's";

	lParam = lParam;	// keine warning
	hwBrowse = GetDlgItem(hDlg, IDD_BROWSE); // Handle auf Ausgabebox
	switch(message)
	{
	case WM_INITDIALOG :
		switch (ausgabe)
		{
		case IDM_AUSCLUSTER :
			Idm__auscluster(hwBrowse);
			SendMessage(hDlg,WM_SETTEXT,0,LONGADDR(szauswerte));
			return TRUE;
		case IDM_AUSWERTE :
			SendMessage(hDlg,WM_SETTEXT,0,LONGADDR(szauscluster));
			Idm__auswerte(hwBrowse);
		}
		return TRUE;
	case WM_COMMAND :
		switch (wParam)
		{
		case IDD_SWAP :		// Ausgabe umschalten
			ausgabe = (ausgabe == IDM_AUSCLUSTER) ? IDM_AUSWERTE : IDM_AUSCLUSTER;
			if (ausgabe == IDM_AUSCLUSTER)
			{
				SendMessage(hDlg,WM_SETTEXT,0,LONGADDR(szauscluster));
				Idm__auscluster(hwBrowse);
			}
			else
			{
				SendMessage(hDlg,WM_SETTEXT,0,LONGADDR(szauswerte));
				Idm__auswerte(hwBrowse);
			}
			return 0;
		case IDD_HILFE:
			WinHelp(hglobalwin, szHelpName, HELP_CONTEXT, Ansicht__Dialog);
			return 0;
		case IDOK :
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		}
	case WM_SETFOCUS :
	case WM_CHAR :
		return 0;
	}
	return FALSE;
}


/* Browse-Procedure
 */
BOOL FAR PASCAL _export BrowseDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	lParam = lParam;	// keine warning
	switch(message)
	{
	case WM_SETFOCUS:
	case WM_CHAR:
		return 0;
	}
	return CallWindowProc(lpfnAusgabeDlgProc, hDlg, message, wParam, lParam);
}



/* SubBereiche-Procedure
 */
BOOL FAR PASCAL _export SubBereicheDlgProc( HWND hDlg, UINT message,
						  UINT wParam, LONG lParam)
{
	int i,j;
	static int welcher;
	int nummern[MAXSTRINGS];
	HANDLE hwmerkmal, hwsub;
	static int feldsubslocal[MAXWERTE][MAXSTRINGS];

	lParam = lParam;			// keine warning!
	hwmerkmal = GetDlgItem(hDlg, IDD_MERKMAL);
	hwsub     = GetDlgItem(hDlg, IDD_SUBMERKMAL);
	switch(message)
	{
	case WM_INITDIALOG :
		/* Merkmal */
		SendMessage(hwmerkmal, LB_RESETCONTENT,0,0);
		for (i=0; i < werteanz; i++)
			SendMessage(hwmerkmal, LB_ADDSTRING,0,(LONG)(LPSTR)title[i]);
		/* Sub */
		SendMessage(hwsub, LB_RESETCONTENT,0,0);
		welcher = whichco[1];
		if (header[welcher] == STR_)
		{
			for (i=0; i < bislang[welcher]; i++)
				SendMessage(hwsub, LB_ADDSTRING,0,(LONG)(LPSTR)text[welcher][i]);
			for (i=0; i<bislang[welcher]; i++)
//				if (feldsubs[welcher][i])
					SendMessage(hwsub, LB_SETSEL, feldsubs[welcher][i], MAKELONG(i,0));
		}
		SendMessage(hwmerkmal, LB_SETCURSEL, welcher, 0);

		// feldsubslocal neu setzen
		for (i=0; i<werteanz; i++)
			for (j=0; j < MAXSTRINGS; j++)
				feldsubslocal[i][j] = feldsubs[i][j];
		return TRUE;
	case WM_COMMAND :
		switch (wParam)
		{
		case IDOK :
			anzahlsubs = (int)SendMessage(hwsub, LB_GETSELITEMS,
				MAXSTRINGS,(LONG)(LPSTR)(nummern));
			for (i=0; i<MAXSTRINGS; i++)
				feldsubslocal[welcher][i] = 0;
			for (i=0; i<anzahlsubs; i++)
				feldsubslocal[welcher][nummern[i]] = 1;
			for (i=0; i<werteanz; i++)
				for (j=0; j < MAXSTRINGS; j++)
					feldsubs[i][j] = feldsubslocal[i][j];
			Considering();
			gerechnet=FALSE;	// damit alle gezeichnet
			Paintit_all();		// werden
		case IDCANCEL :
			EndDialog(hDlg, 0);
			return TRUE;
		case IDD_SUBHELP:
			WinHelp(hglobalwin, szHelpName, HELP_CONTEXT, Unterbereiche__Dialog);
			return 0;
		case IDD_MERKMAL :
			anzahlsubs = (int)SendMessage(hwsub, LB_GETSELITEMS,
				MAXSTRINGS,(LONG)(LPSTR)(nummern));
			// feldsubs neu berechnen
			for (i=0; i<MAXSTRINGS; i++)
				feldsubslocal[welcher][i] = 0;
			for (i=0; i<anzahlsubs; i++)
				feldsubslocal[welcher][nummern[i]] = 1;

			welcher = (int)SendMessage(hwmerkmal, LB_GETCURSEL, 0, 0L);
			SendMessage(hwsub, LB_RESETCONTENT,0,0);
			if (header[welcher] == STR_)
			{
				for (i=0; i<bislang[welcher]; i++)
					SendMessage(hwsub, LB_ADDSTRING,0,(LONG)(LPSTR)text[welcher][i]);
				SendMessage(hwsub, WM_SETREDRAW, TRUE, 0L);
				for (i=0; i<bislang[welcher]; i++)
				{
					SendMessage(hwsub, LB_SETSEL, feldsubslocal[welcher][i], MAKELONG(i, 0));
					i=i;
				}
			}
			return 0;
		case IDD_SUBMERKMAL :
			return 0;
		}
	break;
	}
	return FALSE;
}


