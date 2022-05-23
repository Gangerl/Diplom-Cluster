/*
 * werror.h
 *
 * Sun Jul 16 20:25:17 GMT 1995
 * Wolfgang Lauffher
 */

#include <windows.h>

/* werror: gib eine Fehlermeldung aus */
void werror (HANDLE, char *, ...);

/* wError: gib eine Fehlermeldung aus und verende! */
void wError (HANDLE, char *, ...);

