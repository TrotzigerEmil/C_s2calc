#ifndef LONGCALC_H_REPL
#define LONGCALC_H_REPL

/* В заголовочном файле repl.h описана функция, реализующая цикл чтения-вычисления-вывода (read-eval-print loop),
   а также функция декодирования числа в ДК. */

/* decode: декодирует число, записанное в дополнительном коде */
char *decode(char *a);

/* repl: запуск цикла чтения-вычисления-вывода */
int repl(void);

#endif
