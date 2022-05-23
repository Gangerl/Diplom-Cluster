/************************************************************************
 * DIPLOM.C  --  Windows-Routinen
 *
 * (c) Wolfgang Lauffher
 *
 * August 1994
 ************************************************************************/

#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "menu.h"
#include "compute.h"
#include "init.h"
#include "fileio.h"
#include "graphio.h"
#include "memory.h"
#include "dialog.h"
#include "util.h"

HBITMAP     hBild;						// Titelbild
static char szAppName[]    = "Diplom" ;	// Name der Applikation

/* exportierte Funktionen
 */
long FAR PASCAL _export WndProc( HWND, UINT , UINT, LONG );


/* Drucker initialisieren
 */
HDC GetPrinterDC (void)
{
	static char szPrinter [80];
	char		*szDevice, *szDriver, *szOutput;

	GetProfileString ("windows", "device", ",,,", szPrinter, 80);
	if (NULL != (szDevice = strtok(szPrinter, "," )) &&
		NULL != (szDriver = strtok(NULL,      ", ")) &&
		NULL != (szOutput = strtok(NULL,      ", "))  )
		return CreateDC (szDriver, szDevice, szOutput, NULL);
	return 0;
}


/* MainWindow-Call
 */
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpszCmd, int nCmdShow)
{
	HWND        hwnd ;
	MSG         msg ;
	WNDCLASS    wndclass ;
	HANDLE	hAccel;

	lpszCmd = lpszCmd;	// keine Warnings!
	hAccel = LoadAccelerators(hInstance, szAppName);
	hBild  = LoadBitmap(hInstance, szAppName);

	if (!hPrevInstance)
	{
		wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
		wndclass.lpfnWndProc   = WndProc ;
		wndclass.cbClsExtra    = 0 ;
		wndclass.cbWndExtra    = 0 ;
		wndclass.hInstance     = hInstance ;
		wndclass.hIcon         = LoadIcon(hInstance, szAppName );
		wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
		wndclass.hbrBackground = GetStockObject(BLACK_BRUSH);
		wndclass.lpszMenuName  = szAppName;
		wndclass.lpszClassName = szAppName ;
		RegisterClass (&wndclass) ;

	 }

	hwnd = CreateWindow (szAppName,
		"Clustering",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		300,
		NULL,
		NULL,
		hInstance,
		NULL) ;

	hglobalwin = hwnd;		// fuer error
	ShowWindow (hwnd, nCmdShow) ;
	strcpy(szHelpName,"cluster.hlp");		// Help-Datei

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}

	return msg.wParam ;

}


/* Main-Window-Procedure
 */
long FAR PASCAL _export WndProc (HWND hwnd, UINT message,
				UINT wParam, LONG lParam)
{
	HMENU		hMenu;
	HANDLE	hCursor;
	PAINTSTRUCT	ps;
	RECT		rect;
	HDC 		hdcMem;
	static FARPROC lpfnAboutDlgProc;
	static FARPROC lpfnOptionenDlgProc;
	static FARPROC lpfnWerteDlgProc;
	int i, nhelp;

	switch (message)
	{
	case WM_COMMAND :
		hMenu = GetMenu(hwnd);
		switch(wParam)
		{
		case IDM_COMPUTE :
			if ( (eingelesen) && (anzco>0) )
			{
				hCursor = LoadCursor(NULL, IDC_WAIT);
				SetCursor(hCursor);
				hdc = GetDC(hwnd);
				gerechnet=FALSE;		// damit nach consider wieder
				Paintit_all();			// alle gemalt werdend
				berechnetecluster = Compute(),
				gerechnet = TRUE;
				ReleaseDC(hwnd,hdc);
				EnableMenuItem(hMenu, IDM_AUSWERTE, MF_ENABLED);
				EnableMenuItem(hMenu, IDM_AUSCLUSTER, MF_ENABLED);
			}
			return 0;
		case IDM_ENUMERATE :
			if ( (eingelesen) && (anzco>0) )
			{
				hCursor = LoadCursor(NULL, IDC_WAIT);
				SetCursor(hCursor);
				hdc = GetDC(hwnd);
				gerechnet=FALSE;		// damit nach consider wieder
				Paintit_all();			// alle gemalt werdend
				berechnetecluster = Enumerate(),
				gerechnet = TRUE;
				ReleaseDC(hwnd,hdc);
				EnableMenuItem(hMenu, IDM_AUSWERTE, MF_ENABLED);
				EnableMenuItem(hMenu, IDM_AUSCLUSTER, MF_ENABLED);
			}
			return 0;
		case IDM_OPTIONEN:
			DialogBox(hInstance, "Optionen", hwnd, lpfnOptionenDlgProc);
			return 0;
		case IDM_WERTE :
			DialogBox(hInstance, "Werte", hwnd, lpfnWerteDlgProc);
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case IDM_OPEN :
			Readdatei();
			if (eingelesen)
			{
				Modifiziere_Werte();
				SetWindowPos(hwnd,0,0,0,FENSTERGROESSE+100,
					FENSTERGROESSE, SWP_NOACTIVATE | SWP_NOMOVE |
					SWP_NOZORDER);
				EnableMenuItem(hMenu, IDM_COMPUTE,MF_ENABLED);
				EnableMenuItem(hMenu, IDM_WERTE, MF_ENABLED);
				EnableMenuItem(hMenu, IDM_LOESCHEN, MF_ENABLED);
				EnableMenuItem(hMenu, IDM_PRINT, MF_ENABLED);
				EnableMenuItem(hMenu, IDM_CLIP, MF_ENABLED);
				if (anzahl < 31)
					EnableMenuItem(hMenu, IDM_ENUMERATE, MF_ENABLED);
				else
					EnableMenuItem(hMenu, IDM_ENUMERATE, MF_DISABLED);
				first_time = FALSE;		// fuer mallocs + free
				gerechnet = FALSE;		// fuer Bildschirmdarstellunsg
				if (nachbaranz >= anzahl) nachbaranz = anzahl -1;
										// bei kl. Dateien sonst evtl. Fatal!
			}
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		case IDM_KKREUZ :
			mit_KKreuz = !mit_KKreuz;
			if (!mit_KKreuz) CheckMenuItem (hMenu, IDM_KKREUZ, MF_UNCHECKED);
			else             CheckMenuItem (hMenu, IDM_KKREUZ, MF_CHECKED);
			return 0;
		case IDM_LOESCHEN :
			hdc = GetDC(hwnd);
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc,0,0,1500,1200);
			ReleaseDC(hwnd, hdc);
			return 0;
		case IDM_ABOUT :
			DialogBox(hInstance, "About", hwnd, lpfnAboutDlgProc);
			return 0;
		case IDM_INHALT :
			WinHelp(hglobalwin, szHelpName, HELP_INDEX, 0);
			return 0;
		case IDM_TASTEN :
			WinHelp(hglobalwin, szHelpName, HELP_CONTEXT, Tasten);
			return 0;
		case IDM_HELP :
			WinHelp(hglobalwin, szHelpName, HELP_HELPONHELP, 0);
			return 0;
		case IDM_PRINT :
			hdc = GetPrinterDC();
			if(hdc)
			{
				nhelp = winmaxy;
				winmaxy = GetDeviceCaps(hdc, HORZRES);
				winmaxy -= winmaxy/4;
				if (Escape(hdc, STARTDOC, sizeof szAppName -1,
					szAppName, NULL) > 0)
				{
					SaveDC(hdc);
					Paintit_all();
					Zielfunktionswert_ausgeben(best->zfktwert);
					RestoreDC(hdc, -1);
					if (Escape(hdc, NEWFRAME, 0, NULL, NULL) >0)
						Escape(hdc, ENDDOC, 0, NULL, NULL);
				}
				DeleteDC(hdc);
				winmaxy = nhelp;
			}
			hdc = GetDC(hwnd);
			ReleaseDC(hwnd, hdc);
			return 0;
		case IDM_CLIP :
			hdc = GetDC(hwnd);
			hdcMem = CreateCompatibleDC(hdc);
			hBild = CreateCompatibleBitmap(hdc,winmaxy + 160, winmaxy);
			if (hBild)
			{
				SelectObject(hdcMem, hBild);
				BitBlt(hdcMem, 0,0, winmaxy+160, winmaxy, hdc,
					0, 0, SRCCOPY);
				OpenClipboard(hwnd);
				EmptyClipboard();
				SetClipboardData(CF_BITMAP, hBild);
				CloseClipboard();
			}
			else	MessageBeep(0);
			DeleteObject(hdcMem);
			ReleaseDC(hwnd, hdc);
			return 0;
		case IDM_END :
			SendMessage(hwnd, WM_CLOSE, 0,0L);
			return 0;
		case IDM_AUSCLUSTER :
		case IDM_AUSWERTE :
			ausgabe = (wParam == IDM_AUSCLUSTER) ? IDM_AUSCLUSTER : IDM_AUSWERTE;
			DialogBox(hInstance, "Ausgabe", hwnd, lpfnAusgabeDlgProc);
			return 0;
		}
		break;
	case WM_PAINT:
		if (!eingelesen)
		{
			hdc = BeginPaint( hwnd, &ps );
			GetClientRect( hwnd , &rect );
			hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hBild);
			SetMapMode(hdcMem, GetMapMode(hdc));
			BitBlt(hdc, 0,0, 640, 480, hdcMem,
				 0, 0, SRCCOPY);
			DeleteObject(hdcMem);
			EndPaint( hwnd, &ps );
		}
		else
		{
			hdc = BeginPaint(hwnd, &ps);
			Paintit_all();
			EndPaint( hwnd, &ps );
			if (gerechnet)
				Zielfunktionswert_ausgeben(best->zfktwert=Summe(best));
		}
		return 0;
	case WM_RBUTTONDOWN:
		SendMessage(hwnd, WM_COMMAND, IDM_COMPUTE, lParam);
		return 0;
	case WM_MBUTTONDOWN:
		DialogBox(hInstance, "Wichtung", hwnd, lpfnWichtungDlgProc);
		return 0;
	case WM_SIZE :
		winmaxy = HIWORD(lParam);
		if (LOWORD(lParam) < winmaxy + 100)
			SetWindowPos(hwnd,0,0,0,winmaxy+100,winmaxy,
			  SWP_NOACTIVATE | SWP_NOMOVE |SWP_NOZORDER);
		UpdateWindow(hwnd);
		return 0;
	case WM_CREATE :
		Init();
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
		lpfnAboutDlgProc =
			MakeProcInstance ((FARPROC) AboutDlgProc, hInstance);
		lpfnOptionenDlgProc =
			MakeProcInstance ((FARPROC) OptionenDlgProc, hInstance);
		lpfnWichtungDlgProc =
			MakeProcInstance ((FARPROC) WichtungDlgProc, hInstance);
		lpfnWerteDlgProc =
			MakeProcInstance ((FARPROC) WerteDlgProc, hInstance);
		lpfnAusgabeDlgProc =
			MakeProcInstance ((FARPROC) AusgabeDlgProc, hInstance);
		lpfnBrowseDlgProc =
			MakeProcInstance ((FARPROC) BrowseDlgProc, hInstance);
		lpfnSubBereicheDlgProc =
			MakeProcInstance ((FARPROC) SubBereicheDlgProc, hInstance);
		if (!mit_KKreuz) CheckMenuItem (hMenu, IDM_KKREUZ, MF_UNCHECKED);
		else             CheckMenuItem (hMenu, IDM_KKREUZ, MF_CHECKED);
		DBMSG((""));
		DBMSG(("Programm Clustering gestartet"));
		return 0;
	case WM_DESTROY:
		FreeProcInstance(lpfnAboutDlgProc);
		FreeProcInstance(lpfnBrowseDlgProc);
		FreeProcInstance(lpfnWerteDlgProc);
		FreeProcInstance(lpfnOptionenDlgProc);
		FreeProcInstance(lpfnAusgabeDlgProc);
		FreeProcInstance(lpfnAboutDlgProc);
		FreeProcInstance(lpfnWichtungDlgProc);
		for (i=0; i<15; i++)
		{
			DeleteObject(pen[i]);
			DeleteObject(brush[i]);
		}
		DeleteObject(hBild);
		if (eingelesen)
		{
			Zerstoere_Speicher();
			Dispose_Loesung(cluster);
			Dispose_Loesung(best);
			Dispose_Loesung(test);
		}
		WinHelp(hglobalwin, szHelpName, HELP_QUIT,0);
		PostQuitMessage(0);
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}



