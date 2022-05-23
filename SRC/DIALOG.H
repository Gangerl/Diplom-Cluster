/************************************************************************
 * DIALOG.H  --  Windows-Dialoge
 *
 * (c) Wolfgang Lauffher
 *
 * August 1994
 ************************************************************************/

#include <windows.h>

/* exportierte Funktionen
 */
BOOL FAR PASCAL _export AboutDlgProc( HWND, UINT, UINT, LONG);
BOOL FAR PASCAL _export OptionenDlgProc( HWND, UINT, UINT, LONG);
BOOL FAR PASCAL _export WerteDlgProc( HWND, UINT, UINT, LONG);
BOOL FAR PASCAL _export WichtungDlgProc( HWND, UINT, UINT, LONG);
BOOL FAR PASCAL _export AusgabeDlgProc( HWND, UINT, UINT, LONG);
BOOL FAR PASCAL _export BrowseDlgProc( HWND, UINT, UINT, LONG);
BOOL FAR PASCAL _export SubBereicheDlgProc( HWND, UINT, UINT, LONG);

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

/* globale Funktionszeiger */
EXTERN FARPROC lpfnWichtungDlgProc;
EXTERN FARPROC lpfnBrowseDlgProc;
EXTERN FARPROC lpfnAusgabeDlgProc;
EXTERN FARPROC lpfnSubBereicheDlgProc;

/* ein paar Variablen zur Ausgabe */
EXTERN int ausgabe;		   // Ausgabe nach Clustern oder Werten
EXTERN char szHelpName[12]; // Name der Hilfe-Datei



