#include "input.h"
#include <stdlib.h>

char *fgetln(FILE *in)
{
    if (!in) return NULL;
    
    char *ret = malloc(BUFMULT);
    *ret = 0;
    unsigned bpos = 0;
    unsigned bcursiz = BUFMULT;

    char c;

    while ((c = fgetc(in)) != '\n' && c != EOF) // enter/eof
    {
	ret[bpos] = c;
	bpos++;
	if (bpos == bcursiz) // заполнение текущего буфера
	{
	    bcursiz *= 2;
	    char *tmp = realloc(ret, bcursiz);
	    if (!tmp)
	    {
		free(ret);
		return NULL;
	    }	  
	    ret = tmp;
	}
	ret[bpos] = 0;
    }

    return ret;
}

wchar_t *fwgetln(FILE *in)
{
    if (!in) return NULL;
    
    wchar_t *ret = malloc(BUFMULT);
    *ret = 0;
    unsigned bpos = 0;
    unsigned bcursiz = BUFMULT;

    wchar_t c;

    while ((c = fgetwc(in)) != '\n' && c != EOF) // enter/eof
    {
	ret[bpos] = c;
	bpos++;
	if (bpos == bcursiz) // заполнение текущего буфера
	{
	    bcursiz *= 2;
	    wchar_t *tmp = realloc(ret, bcursiz);
	    if (!tmp)
	    {
		free(ret);
		return NULL;
	    }	  
	    ret = tmp;
	}
	ret[bpos] = 0;
    }

    return ret;
}
