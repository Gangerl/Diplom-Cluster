/************************************************************************
 * debug.c - debugging auf Com-Schnittstelle
 * Header in ddefines.h per Makro
 * Aufruf:  DBMSG(( ))
 *
 * (c) Wolfgang Lauffher
 *
 * August 1995
 ************************************************************************/

#include <windows.h>

VOID FAR _cdecl DbgWriteMsg(LPSTR pszFmt, ...)
{
	char szMsg[128];
	wvsprintf(szMsg, pszFmt, (LPSTR)&pszFmt+sizeof(pszFmt));
	OutputDebugString(szMsg);
	OutputDebugString("\r\n");
}


