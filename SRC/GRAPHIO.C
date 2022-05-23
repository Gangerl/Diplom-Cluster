/************************************************************************
 * GRAPHIO.C  -  Ausgabe der Grafik fuer das Projekt Diplomarbeit
 *
 * (c) Wolfgang Lauffher
 *
 * Mai/August 1995
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "graphio.h"

#define TOLERANZ    3
#define X_FI        -3.1415926536/6
#define Y_FI        -3.1415926536/4

int winx, winy, winz; 		// Groesse des Fensters
int barbreite ;			// Groesse der 'Bars'

static void Trans (int *x, int *y, int *z)
{
	int h;
	h = *y;
	*y = (*y-winy/2)*cos(X_FI) - (*z-winz/2)*sin(X_FI) + (winy / 2);
	*z = (*z-winz/2)*cos(X_FI) + (h-winy/2)*sin(X_FI)  + (winz / 2);
	*x = (*x-winx/2)*cos(Y_FI) - (*z-winz/2)*sin(Y_FI) + (winx / 2);
}


static void bar(long c, short x1, short y1, short x2, short y2)
{
	SelectObject(hdc, brush[c]);
	SelectObject(hdc, pen[c]  );
	Rectangle(hdc, x1,y1,x2+barbreite,y2+barbreite);
}


void Textausxy(int x, int y, char *s)
{
	SetTextColor(hdc, ctable[MAXCOLOR]);
	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, x, y, s, strlen(s));
}


void ClearEndeMeldung(void)
{
	bar(13,winx+20,winmaxy-20,winx+100,winmaxy-2); // schwarz
}


void EndeMeldung(char *s)
{
	Textausxy(winx+20, winmaxy-20, s);
}


static void line(short x1, short y1, short x2, short y2)
{
	SelectObject(hdc, pen[MAXCOLOR]);
	MoveTo(hdc, x1, y1);
	LineTo(hdc, x2, y2);
}

static void Legende(void)
{
	int x,y;
	char  st[30], text1[30];

	x = winx+TOLERANZ+5;
	y = winy / 2 - 18;
	Textausxy(x,y,"Funktionswert:");
	Textausxy(x,y+50,"Zaehler:");
	y = 10;
	line(x-5, 0, x-5, winmaxy);
	line(x-4, 0, x-4, winmaxy);
	Textausxy(x,y,"    nach rechts : "); y+=20;
	Textausxy(x,y,title[whichgr[0]]); y+=20;
	strcpy (st,"von ");
	sprintf(text1,"%d",small[whichgr[0]]);
	strcat(st, text1);
	strcat(st, " bis ");
	sprintf(text1,"%d",big[whichgr[0]]);
	strcat(st, text1);
	Textausxy(x,y,st); y+=30;
	if (anzgr > 1)
	{
		Textausxy(x,y,"    nach oben : "); y+=20;
		Textausxy(x,y,title[whichgr[1]]); y+=20;
		strcpy (st,"von ");
		sprintf(text1,"%d",small[whichgr[1]]);
		strcat(st, text1);
		strcat(st, " bis ");
		sprintf(text1,"%d",big[whichgr[1]]);
		strcat(st, text1);
		Textausxy(x,y,st); y+=30;
	}
	if (anzgr > 2)
	{
		Textausxy(x,y,"    nach hinten : "); y+=20;
		Textausxy(x,y,title[whichgr[2]]); y+=20;
		strcpy (st,"von ");
		sprintf(text1,"%d",small[whichgr[2]]);
		strcat(st, text1);
		strcat(st, " bis ");
		sprintf(text1,"%d",big[whichgr[2]]);
		strcat(st, text1);
		Textausxy(x,y,st); y+=30;
	}
}


static void KKreuz (void)
{
	int x1,y1,x2,y2,z1,z2 ;

	y1 = winy / 2;
	x1 = winx / 2;

	line(0, winmaxy-y1, winx ,winmaxy-y1 );	/* 1D */
	SetPixel(hdc, winx-1,winmaxy-y1-1,ctable[MAXCOLOR]);
	SetPixel(hdc, winx-2,winmaxy-y1-2,ctable[MAXCOLOR]);
	SetPixel(hdc, winx-3,winmaxy-y1-3,ctable[MAXCOLOR]);
	SetPixel(hdc, winx-1,winmaxy-y1+1,ctable[MAXCOLOR]);
	SetPixel(hdc, winx-2,winmaxy-y1+2,ctable[MAXCOLOR]);
	SetPixel(hdc, winx-3,winmaxy-y1+3,ctable[MAXCOLOR]);
	if (anzgr > 1)
	{                   		/* 2D */
		line(x1, winmaxy, x1, winmaxy-winy );
		SetPixel(hdc, x1-1,winmaxy-winy+1,ctable[MAXCOLOR]);
		SetPixel(hdc, x1-2,winmaxy-winy+2,ctable[MAXCOLOR]);
		SetPixel(hdc, x1-3,winmaxy-winy+3,ctable[MAXCOLOR]);
		SetPixel(hdc, x1+1,winmaxy-winy+1,ctable[MAXCOLOR]);
		SetPixel(hdc, x1+2,winmaxy-winy+2,ctable[MAXCOLOR]);
		SetPixel(hdc, x1+3,winmaxy-winy+3,ctable[MAXCOLOR]);
	}
	if (anzgr > 2)
	{				/* 3D */
		z1 = 0;
		z2 = winy;
		x1 = winx / 2;
		y1 = winy / 2;
		x2 = x1;
		y2 = y1;
		Trans(&x1,&y1,&z1);
		Trans(&x2,&y2,&z2);
		line(x1,winmaxy-y1,x2,winmaxy-y2);
		SetPixel(hdc, x2-1,winmaxy-y2,ctable[MAXCOLOR]);
		SetPixel(hdc, x2-2,winmaxy-y2,ctable[MAXCOLOR]);
		SetPixel(hdc, x2-3,winmaxy-y2,ctable[MAXCOLOR]);
		SetPixel(hdc, x2,winmaxy-y2+1,ctable[MAXCOLOR]);
		SetPixel(hdc, x2,winmaxy-y2+2,ctable[MAXCOLOR]);
		SetPixel(hdc, x2,winmaxy-y2+3,ctable[MAXCOLOR]);
	}
}


void ClearDerzeitAusgabe(void)
{
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 110;
	bar(13,x,y,x+125,y+20); // schwarz
	y = winy / 2 + 130;
	bar(13,x,y,x+125,y+20); // schwarz
}


void DerzeitAusgabe(float best)
{
	char zahl[20] ;
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 110;
	Textausxy(x,y,"derzeit beste:");
	y = winy / 2 + 130;
	sprintf(zahl,"%10.2f ",best);
	Textausxy(x,y,zahl);
}

void ClearSimAnAusgabe(void)
{
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 160;
	bar(13,x,y,x+125,y+20); // schwarz
	y = winy / 2 + 180;
	bar(13,x,y,x+125,y+20); // schwarz
}



void SimAnAusgabe(int zaehler, float zeit)
{
	char zahl[20] ;
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 160;
	Textausxy(x,y,"gefunden in:");
	y = winy / 2 + 180;
	sprintf(zahl,"%4d / %9.3f ",zaehler, zeit);
	Textausxy(x,y,zahl);
}


void ClearZeitAusgabe(void)
{
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 80;
	bar(13,x,y,x+125,y+20); // schwarz
}

void ZeitAusgeben (float zeit)
{
	char zahl[20] ;
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 80;
	sprintf(zahl,"%9.3f Sekunden",zeit);
	Textausxy(x,y,zahl);
}


void Zielfunktionswert_ausgeben(float wert )
{
	char zahl[20] ;
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2;
	sprintf(zahl,"%10.2f",wert);
	bar(13,x,y,x+100,y+20); // schwarz
	Textausxy(x,y,zahl);
}

void Zaehlerstand_ausgeben(int wert )
{
	char zahl[20] ;
	int x,y;

	x = winx+TOLERANZ+5;
	y = winy / 2 + 50;
	sprintf(zahl,"%7u",wert);
	bar(13,x,y,x+100,y+20); // schwarz
	Textausxy(x,y,zahl);
}

static void Paintit1d(int i, int c)
{
	int x,y;

	y = winy / 2;
	x = modwert[i][whichgr[0]] * (winx-10) ;
	bar(c,x,winmaxy-y-5,x+1,winmaxy-y+5);
}


static void Paintit2d(int i, int c)
{
	int x,y;

	x = modwert[i][whichgr[0]] * winx ;
	y = winy - modwert[i][whichgr[1]] * winy ;
	bar(c,x,y,x+TOLERANZ,y+TOLERANZ);
}


static void Paintit3d(int i, int c)
{
	int x,y,z;

	x = modwert[i][whichgr[0]] * winx ;
	y = winy - modwert[i][whichgr[1]] * winy ;
	z = modwert[i][whichgr[2]] * winz ;
	Trans(&x,&y,&z);
	bar(c,x,winmaxy-y,x+TOLERANZ,winmaxy-y-TOLERANZ);
}

void Paintit_all(void)
{
	int i,j, zahl = clusteranz;
	winy = winmaxy-TOLERANZ;
	winx = winy;
	winz = winy;
	barbreite = winmaxy / 480;
	Legende();
	if (mit_KKreuz) KKreuz();

	if (gerechnet)				// male beste aktuelle Loesung
	{
		/* Problem, wenn die Clusteranzahl interaktiv geaendert wird:*/
		if (clusteranz != berechnetecluster)
			zahl = berechnetecluster;
		for (i=0; i<zahl; i++)
			for (j=0; j<best->anz_in_cluster[i]; j++)
				switch (anzgr)
				{
				case 1:
					if (consider[best->cluster[i][j]])
						Paintit1d(best->cluster[i][j],i);
					else
						Paintit1d(best->cluster[i][j],MAXCOLOR-1);
					break;
				case 2:
					if (consider[best->cluster[i][j]])
						Paintit2d(best->cluster[i][j],i);
					else
						Paintit2d(best->cluster[i][j],MAXCOLOR-1);
					break;
				case 3:
					if (consider[best->cluster[i][j]])
						Paintit3d(best->cluster[i][j],i);
					else
						Paintit3d(best->cluster[i][j],MAXCOLOR-1);
					break;
				}
	}
	else
	{
	 for (i=0; i<anzahl; i++)
		switch (anzgr)
		{
		case 1:
			if (consider[i])
				Paintit1d(i,MAXCOLOR);
			else
				Paintit1d(i,MAXCOLOR-1);
			break;
		case 2:
			if (consider[i])
				Paintit2d(i,MAXCOLOR);
			else
				Paintit2d(i,MAXCOLOR-1);
			break;
		case 3:
			if (consider[i])
				Paintit3d(i,MAXCOLOR);
			else
				Paintit3d(i,MAXCOLOR-1);
			break;
		}
	}
}


void Paintit (int i, int c)
{
	switch (anzgr)
	{
	case 1: Paintit1d(i,c); break;
	case 2: Paintit2d(i,c); break;
	case 3: Paintit3d(i,c); break;
	}
}


