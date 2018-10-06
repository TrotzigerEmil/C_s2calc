#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "solver.h"
#include "numbers.h"

// RPN-Parse

char isNumber(char *str)
{
    while (*str && *str >= '0' && *str <= '9') str++;
    return *str;
}

char *parseString(char *rpnstr)
{
    Stack operands = initStack(); // инициализация стека начальными значениями

    /* проход по всем токенам выражения, разделенным пробелами */
    for (char *token = strtok(rpnstr, DELIM); token; token = strtok(NULL, DELIM))
    {
	/* если токен - число, то отправить его в стек */
	if (!isNumber(token))
	{
	    push(token, &operands);
	}
	/* в ином случае извлечь данные из стека, совершить операцию,
	   результат отправить в стек */
	else
	{
	    char *b = pop(&operands);
	    char *a = pop(&operands);
	    //printf("%s %s\n", a, b);
	    char *c;
	    if (!strcmp(token, ADD))
	    {
		c = add(a, b);
	    }
	    if (!strcmp(token, SUB))
	    {
		c = sub(a, b);
	    }
	    if (!strcmp(token, MUL))
	    {
		c = mul(a, b);
	    }
	    if (!strcmp(token, DIV))
	    {
		c = dvr(a, b, NULL);
	    }
	    //printf("%s\n", c);
	    push(c, &operands);
	    free(a);
	    free(b);
	    free(c);
	}
	
    }
    
    char *result = malloc(strlen(peek(&operands)) + 1); // выделить память под строку на вершине стека
    strcpy(result, peek(&operands)); // скопировать в возвращаемое значение
    killStack(&operands); // освободить память, занятую стеком
    
    return result;
}

// Infix-To-RPN

/* Структура, содержащая все данные, необходимые для преобразования выражения */
struct state
{
    char *res; // Выражение-результат
    unsigned reslen; // Длина res
    
    States cstate; // Текущее состояние автомата
    States pstate; // Состояние автомата до встречи пробела
    
    unsigned infpos; // Позиция во входной строке

    char *currentdgt; // Массив, содержащий текущее считываемое число
    unsigned sizeofdigit; // Длина currentdgt
    int sign; // Знак числа
    
    Stack operators; // Стек с операторами
};

/* Множество символов */
struct set
{
    char setb[SETSIZE];
    unsigned size;
};

Set s_digit = {"0123456789", 10}; // множество ЦИФРА
Set s_sign = {"+-", 2}; // множество ЗНАКЧИСЛА
Set s_binop = {"+-*/", 4}; // множество ОПЕРАТОР
Set s_term = {"\0", 1}; // множество ТЕРМИНАТОР
Set s_spc = {"\x20\x00", 1}; // множество ПРОБЕЛ

/* inSet: возвращает позицию символа в заданном множестве, или -1, если символа нет во множестве */
int inSet(char c, Set set)
{
    for (int i = 0; i < set.size; i++)
    {
	if (set.setb[i] == c) return i;
    }
    return -1;
}

/* freeState: перевод автомата в конечное состояние */
States freeState(State *S)
{
    free(S->res);
    S->res = NULL;
    S->reslen = 0;
    free(S->currentdgt);
    S->currentdgt = NULL;
    S->sizeofdigit = 0;
    killStack(&(S->operators));
    return TERM;
}

/* changeSign: меняет знак считываемого числа на противоположный */
States changeSign(char c, State *S)
{
    if (c == '-')
        S->sign *= -1;
    return SIGN;
}

/* addDigit: добавляет цифру в конец текущего числа */
States addDigit(char c, State *S)
{
    (S->sizeofdigit)++;
    //printf("sd: %u\n", S->sizeofdigit);
    S->currentdgt = realloc(S->currentdgt, S->sizeofdigit);
    S->currentdgt[S->sizeofdigit - 1] = c;
    return DIGIT;
}

/* encode: кодирует число в дополнительном коде для последующей записи */
number encode(State *S)
{
    number wrt;
    if (S->sign == 1)
    {
        wrt = malloc(S->sizeofdigit + 2);
	*wrt = '0';
	for (int i = 0; i < S->sizeofdigit; i++)
	{
	    wrt[i + 1] = S->currentdgt[i];
	}
	wrt[S->sizeofdigit + 1] = 0;
	shift(&wrt, stripsign(wrt), strlen(wrt));
    }
    else
    {
	number tmp = malloc(S->sizeofdigit + 2);
	*tmp = '0';
	for (int i = 0; i < S->sizeofdigit; i++)
	{
	    tmp[i + 1] = S->currentdgt[i];
	}
	tmp[S->sizeofdigit + 1] = 0;	
	wrt = inv(tmp);
	free(tmp);
    }
    return wrt;
}

/* writeDigit: дописывает полученное число в конец результата */
States writeDigit(State *S, States ret)
{
    number wrt = encode(S);
    unsigned wrts = strlen(wrt);
    S->res = realloc(S->res, S->reslen + wrts + 1);
    for (int i = 0; i < wrts; i++)
    {
        S->res[S->reslen++] = wrt[i];
    }
    S->res[S->reslen++] = '\x20'; // insert space
    S->sizeofdigit = 0;
    S->sign = 1;
    free(S->currentdgt);
    free(wrt);
    S->currentdgt = NULL;

    return ret;
}

/* condIfEmpty: используется для выброса всех элементов из стека */
int condIfEmpty(Stack S, char c)
{
    return isEmpty(S);
}

/* writeOperators: записывает в результат операторы из стека в соответствии с функцией f */
States writeOperators(char c, State *S, cond f, States ret)
{
    while (!f(S->operators, c))
    {
	S->res = realloc(S->res, S->reslen + 2);
	char *op = pop(&(S->operators));
        S->res[S->reslen] = *op;
        S->res[S->reslen + 1] = '\x20';
        S->reslen += 2;
	free(op);
    }

    return ret;
}

/* condIfPriority: используется для выброса тех элементов стека, приоритет которых больше или равен данному */
int condIfPriority(Stack S, char c)
{
    int cond1, cond2;

    if (!(S.head)) return 1;

    cond1 = inSet(*(S.head->data), s_binop) / 2;
    cond2 = inSet(c, s_binop) / 2;
    
    return cond1 < cond2;
}

/* printState: выводит текущее состояние автомата */
void printState(State S)
{    
    printf("resptr: %p\nrespos: %u\ncurstate: %d\ninfixpos: %u\ncdptr: %p\ncdsize: %u\nsign: %d\nstack contents: ", S.res, S.reslen, S.cstate, S.infpos, S.currentdgt, S.sizeofdigit, S.sign);
    for (nptr i = S.operators.head; i; i = i->next)
    {
	printf("%c ", *(i->data));
    }
    printf("\nres contents: ");
    for (unsigned i = 0; i < S.reslen; i++)
    {
	printf("%c", S.res[i]);
    }
    printf("\ncd contents: ");
    for (unsigned i = 0; i < S.sizeofdigit; i++)	
    {
	printf("%c", S.currentdgt[i]);
    }
    printf("\n\n");
}

/* DtoB: переход от ЦИФРА к ОПЕРАТОР */
States DtoB(State *S, char c)
{
    writeDigit(S, BINOP);
    S->cstate = writeOperators(c, S, condIfPriority, BINOP);
    char *ins = malloc(2);
    *ins = c;
    ins[1] = 0;
    push(ins, &(S->operators));
    free(ins);
    return BINOP;
}

/* Алгоритм преобразования выражения в ИФЗ в выражение в ПФЗ (алгоритм Дейкстры) для данных условий:
   0. Считать приоритет операторов + и - равным 0, а приоритет оператора * - равным 1.
   1. Пока есть токены, прочитать токен.
   2. В зависимости от типа токена:
   2.1. Если токен - число, записать его на выход.
   2.2. Если токен - оператор, извлечь из стека все операторы, приоритет которых больше или равен данному, записать
   их на вывод, а затем поместить токен в стек.
   3. Извлечь все оставшиеся операторы в стеке и записать их на вывод. */

/* InfixToRPN: возвращает строку с выражением в ПФЗ или NULL, если преобразование не удалось */
char *InfixToRPN(char *infix)
{
    State C = {NULL, 0, SIGN, SIGN, 0, NULL, 0, 1, initStack()}; // инициализация состояния
    char *ret = NULL; // возвращаемое значение
    while(infix[C.infpos] == '\x20') (C.infpos)++; // пропуск ведущих пробелов

    /* Все переходы, не выраженные в конструкции ниже, являются неверными и приводят к выходу из подпрограммы с возвратом 0. */
    while (C.cstate != TERM) // состояние ТЕРМИНАТОР приводит к выходу и возвращению выражения
    {        
	switch(C.cstate)
	{
	case SIGN:
	{
	    if (inSet(infix[C.infpos], s_sign) >= 0) // ЗНАК -> ЗНАК
	    {
		C.cstate = changeSign(infix[C.infpos], &C);
	    }
	    else if (inSet(infix[C.infpos], s_digit) >= 0) // ЗНАК -> ЦИФРА
	    {
		C.cstate = addDigit(infix[C.infpos], &C);
	    }
	    else C.cstate = freeState(&C); // остальные переходы
	    break;
	}
	case DIGIT:
	{
	    if (inSet(infix[C.infpos], s_digit) >= 0) // ЦИФРА -> ЦИФРА
	    {
	        C.cstate = addDigit(infix[C.infpos], &C);
	    }
	    else if (inSet(infix[C.infpos], s_binop) >= 0) // ЦИФРА -> ОПЕРАТОР
	    {
		C.cstate = DtoB(&C, infix[C.infpos]);
	    }
	    else if (inSet(infix[C.infpos], s_term) >= 0) // ЦИФРА -> ТЕРМИНАТОР
	    {
		writeDigit(&C, TERM);
		C.cstate = writeOperators(infix[C.infpos], &C, condIfEmpty, TERM);
		C.res[C.reslen - 1] = 0;
		ret = malloc(C.reslen);
		strcpy(ret, C.res);
		//printState(C);
		C.cstate = freeState(&C);
	    }
	    else if (inSet(infix[C.infpos], s_spc) >= 0) // ЦИФРА -> ПРОБЕЛ
	    {
		C.pstate = C.cstate;
		C.cstate = SPC;
	    }
	    else C.cstate = freeState(&C); // ЦИФРА -> ЗНАК
	    break;
	}
	case BINOP:
	{
	    if (inSet(infix[C.infpos], s_sign) >= 0) // ОПЕРАТОР -> ЗНАК
	    {
		C.cstate = changeSign(infix[C.infpos], &C);
	    }
	    else if (inSet(infix[C.infpos], s_digit) >= 0) // ОПЕРАТОР -> ЦИФРА
	    {
		C.cstate = addDigit(infix[C.infpos], &C);
	    }
	    else if (inSet(infix[C.infpos], s_spc) >= 0) // ОПЕРАТОР -> ПРОБЕЛ
	    {
		C.pstate = C.cstate;
		C.cstate = SPC;
	    }
	    else C.cstate = freeState(&C); // ОСТАЛЬНЫЕ ПЕРЕХОДЫ
	    break;
	}

	case SPC:
	{
	    if (inSet(infix[C.infpos], s_spc) >= 0) // ПРОБЕЛ -> ПРОБЕЛ
	    {
		C.cstate = SPC;
	    }
	    else if (C.pstate == DIGIT && inSet(infix[C.infpos], s_binop) >= 0) // ЦИФРА -> ПРОБЕЛ -> ОПЕРАТОР
	    {
		C.pstate = SIGN;
		C.cstate = DtoB(&C, infix[C.infpos]);
	    }
	    else if (C.pstate == DIGIT && inSet(infix[C.infpos], s_term) >= 0) // ЦИФРА -> ПРОБЕЛ -> ТЕРМИНАТОР
	    {
		C.pstate = SIGN;
		writeDigit(&C, TERM);
		C.cstate = writeOperators(infix[C.infpos], &C, condIfEmpty, TERM); 
		C.res[C.reslen - 1] = 0;
		ret = malloc(C.reslen);
		strcpy(ret, C.res);
		//printState(C);
		C.cstate = freeState(&C);
	    }
	    else if (C.pstate == BINOP && inSet(infix[C.infpos], s_sign) >= 0) // ОПЕРАТОР -> ПРОБЕЛ -> ЗНАКЧИСЛА
	    {
		C.pstate = SIGN;
		C.cstate = changeSign(infix[C.infpos], &C);		
	    }
	    else if (C.pstate == BINOP && inSet(infix[C.infpos], s_digit) >= 0) // ОПЕРАТОР -> ПРОБЕЛ -> ЦИФРА
	    {
		C.pstate = SIGN;
		C.cstate = addDigit(infix[C.infpos], &C);
	    }
	    else C.cstate = freeState(&C); // ОСТАЛЬНЫЕ ПЕРЕХОДЫ
	}
	}	
	(C.infpos)++; // следующий символ	
    }
    
    return ret;
}
