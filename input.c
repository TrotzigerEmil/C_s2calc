#include "input.h"
#include <stdlib.h>
#include <stdio.h>

char *inputString(void)
{
    char *ret = malloc(BUFMULT);
    *ret = 0;
    unsigned bpos = 0;
    unsigned bcursiz = BUFMULT;

    char c;

    while ((c = getchar()) != '\n') // enter
    {
	ret[bpos] = c;
	bpos++;
	if (bpos == bcursiz) // заполнение текущего буфера
	{
	    bcursiz *= 2;
	    ret = realloc(ret, bcursiz);
	}
	ret[bpos] = 0;
    }

    return ret;
}
