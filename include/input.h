#ifndef LONGCALC_H_INPUT
#define LONGCALC_H_INPUT

/* В заголовочном файле input.h описана функция, возвращающая вводимое с клавиатуры выражение,
   а также объявлена функция удаления newline из файла anewline.c . */
#define BUFMULT 1024

/* aNewLine: удаление символа newline из строки */
void aNewLine(char *);

/* inputString: чтение строки из stdin */
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
	if (bpos == bcursiz)
	{
	    bcursiz *= 2;
	    ret = realloc(ret, bcursiz);
	}
	ret[bpos] = 0;
    }

    return ret;
}

#endif
