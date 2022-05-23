/**************************************************************************
 * DEFINES.H  -  globale defines
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1994
 **************************************************************************/

#ifndef DEFINES
#define DEFINES

#define MAXCLUSTER  MAXCOLOR
#define MAXCOLOR    15 			/* max. Farb/Cluster */
#define MAXSTRINGS 6
#define MAXWERTE 30
#define MAXNACHBARANZ 20
#define MAXTABULISTENLAENGE 100
#define STRINGLAENGE 20
#define FENSTERGROESSE 495

#define unendlich     32767
#define realunendlich 1e+8

#define OFFSET 2
#define INT_ 0		/* beliebige Integer */
#define STR_ 2		/* String oder nichts */
#define FLO_ 1		/* Floats */

#define LONGADDR(p)	(LONG)(LPSTR)(p)


/* debug - Information ? */
#define NDEBUG
#if !defined(NDEBUG)
	// set macros to real debugging function
	VOID FAR _cdecl DbgWriteMsg(LPSTR, ...);
	#define DBMSG(Msg) DbgWriteMsg Msg
#else
	// set macros to null function
	#define DBMSG(Msg)
#endif  // !NDEBUG


#endif
