/*
 * error.c
 *
 * Sat Jul 15 20:25:17 GMT 1995
 * Wolfgang Lauffher
 */

#include<stdio.h>
#include<stdarg.h>

/* 
 * error: gib eine Fehlermeldung aus */
void error (char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}


/* 
 * Error: gib eine Fehlermeldung aus und verende! */
void Error (char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
}
