/*
 * werror.c
 *
 * Sun Jul 16 20:25:17 GMT 1995
 * Wolfgang Lauffher
 */

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "werror.h"

/* werror: gib eine Fehlermeldung aus */
void werror (HANDLE hwnd, char *fmt, ...)
{
	va_list args;
	char fehler[100];

	va_start(args, fmt);
	vsprintf(fehler, fmt, args);
	va_end(args);
	MessageBox(hwnd, fehler, "Warning:", MB_OK);
}


/* wError: gib eine Fehlermeldung aus und verende! */
void wError (HANDLE hwnd, char *fmt, ...)
{
	va_list args;
	char fehler[100];

	va_start(args, fmt);
	vsprintf(fehler, fmt, args);
	va_end(args);
	MessageBox(hwnd, fehler, "Warning:", MB_OK);
	exit(EXIT_FAILURE);
}
