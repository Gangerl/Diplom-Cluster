/************************************************************************
 * GRAPHIO.H  -  Ausgabe der Grafik fuer das Projekt Diplomarbeit
 *
 * (c) Wolfgang Lauffher
 *
 * Mai/August 1994
 ************************************************************************/

void EndeMeldung(char *);
void ClearEndeMeldung(void);

void Zielfunktionswert_ausgeben(float);
void Zaehlerstand_ausgeben (int);

void Textausxy(int, int, char *);

void Paintit(int, int );
void Paintit_all(void);

void ClearZeitAusgabe(void);
void ZeitAusgeben(float);

void ClearSimAnAusgabe(void);
void SimAnAusgabe(int, float);

void ClearDerzeitAusgabe(void);
void DerzeitAusgabe(float);

