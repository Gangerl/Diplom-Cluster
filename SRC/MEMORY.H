/************************************************************************
 * memory.h  -  Speicherroutinen fuer das Projekt Diplomarbeit
 *
 * (c) Wolfgang Lauffher
 *
 * Mai 1995
 *
 ************************************************************************/

#include "types.h"

/* allokiert den noetigen Speicherplatz fuer (fast) alle
 * dynamisch erzeugten Felder
 */
void HoleSpeicher (int, int);

/* Legt Speicher fuer eine Clusterloesung an und gibt diesen zurueck
 */
void New_Loesung(Loesungszeiger *);

/* Gibt Speicher einer Clusterloesung frei
 */
void Dispose_Loesung(Loesungszeiger);

/* gibt allen globalen dynamischen Speicher wieder frei
 */
void Zerstoere_Speicher (void);

void FAR * wmalloc(LONG);
void wfree(void FAR *);


