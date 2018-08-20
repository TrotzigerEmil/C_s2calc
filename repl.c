#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "repl.h"
#include "input.h"
#include "solver.h"
#include "numbers.h"

#define REPL_MSG_START "LongCalc v1\nType \'%s\' to get help\n" // Сообщение-приветствие
//Сообщение о помощи
#define REPL_MSG_HELP "\t<infix notation expression> - evaluate expression\n\t%s - exit the program\n\t%s - print this message\
\nSupported operations: addition ('+'), subtraction ('-'), multiplication ('*'), integer division ('/')\
\nParentheses are not supported\n"
#define REPL_MSG_EXIT "Have a nice day.\n" //Сообщение-прощание
#define REPL_MSG_ILLEGAL_EXPR "error: cannot evaluate expression\n" // Сообщение об ошибке преобразования
#define REPL_CMD_HELP "help" // Команда "помощь"
#define REPL_CMD_EXIT "exit" // Команда "выход"

/* decode: декодирует число, записанное в дополнительном коде */
char *decode(number a)
{
    char *ret;
    if (*a == '9')
    {
        ret = inv(a);
	*ret = '-';
    }
    else
    {
	ret = malloc(strlen(a));
	strcpy(ret, a + 1); // копируем со второго символа
    }
    return ret;
}

/* repl: запуск цикла чтения-вычисления-вывода */
int repl(void)
{
    char *in = NULL; // строка на вход
    char *rpn = NULL; // результат перевода в ОПН
    number dres = NULL; // результат вычисления ОПН в доп. коде
    char *res = NULL; // ответ на выход (если таковой есть)
    unsigned running = 1;
    printf(REPL_MSG_START, REPL_CMD_HELP);
    do
    {
        printf("> ");
        in = inputString(); // ввод строки (R)

	if (!strcmp(in, REPL_CMD_HELP)) // получена команда "помощь"
	{
	    printf(REPL_MSG_HELP, REPL_CMD_EXIT, REPL_CMD_HELP);
	}
	else if (!strcmp(in, REPL_CMD_EXIT)) // получена команда "выход"
	{
	    printf(REPL_MSG_EXIT);
	    running = 0;
	}
	else
	{
	    rpn = InfixToRPN(in); // перевод в ОПН
	    if (!rpn) // если не получилось
	    {
		printf(REPL_MSG_ILLEGAL_EXPR);
	    }
	    else
	    {
	        dres = parseString(rpn); // вычисление ОПН (E)
	        res = decode(dres); // декодирование полученного ответа
		printf("= %s\n", res); // вывод (P)
	    }
	}
	/* освобождение памяти; free(NULL) не делает ничего */
	free(dres);
	free(res);
	free(in);
	free(rpn);
	in = res = dres = rpn = NULL;
    } while(running); // цикл (L)
    return 0;
}
