#ifndef LONGCALC_H_INPUT
#define LONGCALC_H_INPUT

#include <stdio.h>
#include <wchar.h>
/* В заголовочном файле input.h описана функция, возвращающая вводимое с клавиатуры выражение. */
#define BUFMULT 1024 // начальный размер буфера чтения

#define getln() fgetln(stdin)
#define wgetln() fwgetln(stdin)

/* inputString: чтение строки из stdin */
char *fgetln(FILE *);
wchar_t *fwgetln(FILE *);

#endif
