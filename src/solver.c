#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "solver.h"
#include "numbers.h"

// RPN-Parse -------------------------------------------------------------------

char isNumber(char *str) // возвращает 0 если str - число
{
    while (*str && *str >= '0' && *str <= '9') str++;
    return *str;
}

char *parseString(char *rpnstr)
{
    Stack *operands = mallocStack(sizeof(char *), 0, EXPAND, NULL);
    // инициализация стека начальными значениями

    /* проход по всем токенам выражения, разделенным пробелами */
    for (char *token = strtok(rpnstr, DELIM); token; token = strtok(NULL, DELIM))
    {
	/* если токен - число, то отправить его в стек */
	if (!isNumber(token))
	{
	    char *toPush = malloc(strlen(token) + 1);
	    strcpy(toPush, token);
	    push(operands, &toPush);
	}
	/* в ином случае извлечь данные из стека, совершить операцию,
	   результат отправить в стек */
	else
	{
	    char *a, *b, *c;

	    pop(operands, &b);
	    pop(operands, &a);
	    
	    switch(*token)
	    {
	    case ADD:
		c = add(a, b);
		break;
	    case SUB:
		c = sub(a, b);
		break;
	    case MUL:
		c = mul(a, b);
		break;
	    case DIV:
		c = dvr(a, b, NULL);
		break;
	    default:
		break; // этого не должно быть
	    }

	    push(operands, &c);
	    free(a);
	    free(b);
	}
	
    }
    
    char *result;
    pop(operands, &result);

    freeStack(operands); // освободить память, занятую стеком
    
    return result;
}
//------------------------------------------------------------------------------

// Infix-To-RPN

/* Структура, содержащая все данные, необходимые для преобразования выражения */
struct state
{
    unsigned char running;
    
    char *res; // Выражение-результат
    unsigned reslen; // Длина res
    
    States cstate; // Текущее состояние автомата
    States pstate; // Состояние автомата до встречи пробела
    
    unsigned infpos; // Позиция во входной строке

    char *currentdgt; // Массив, содержащий текущее считываемое число
    unsigned sizeofdigit; // Длина currentdgt
    int sign; // Знак числа
    
    Stack *operators; // Стек с операторами
};

/* Множество символов */
struct set
{
    char setb[SETSIZE];
    unsigned size;
};

const Set s_digit = {"0123456789", 10}; // множество ЦИФРА
const Set s_sign = {"+-", 2}; // множество ЗНАКЧИСЛА
const Set s_binop = {"+-*/", 4}; // множество ОПЕРАТОР
const Set s_term = {"\0", 1}; // множество ТЕРМИНАТОР
const Set s_spc = {"\x20\x00", 1}; // множество ПРОБЕЛ

State *initState(void)
{
    State *ret = malloc(sizeof(State));
    ret->running = 1;
    ret->res = NULL;
    ret->reslen = 0;
    ret->cstate = SIGN;
    ret->pstate = SIGN;
    ret->infpos = 0;
    ret->currentdgt = NULL;
    ret->sizeofdigit = 0;
    ret->sign = 1;
    ret->operators = mallocStack(sizeof(char), 0, EXPAND, printops);
    return ret;
}

/* inSet: возвращает позицию символа в заданном множестве, или -1, 
   если символа нет во множестве */
int inSet(char c, Set set)
{
    for (int i = 0; i < set.size; i++)
    {
	if (set.setb[i] == c) return i;
    }
    return -1;
}

int whichSet(char c, State *S)
{
    if (inSet(c, s_digit) >= 0)
	return DIGIT;
    if (inSet(c, s_term) >= 0)
	return TERM;
    if (inSet(c, s_spc) >= 0)
	return SPC;

    if (inSet(c, s_binop) >= 0 && (S->cstate == DIGIT || S->pstate == DIGIT))
	return BINOP;
    if (inSet(c, s_sign) >= 0 && (S->cstate == SIGN || S->pstate == SIGN
				  || S->cstate == BINOP || S->pstate == BINOP))
	return SIGN;
    
    return -1;
}

/* exitState: перевод автомата в конечное состояние */
States exitState(char c, State *S)
{
    S->running = 0;

    free(S->currentdgt);
    S->currentdgt = NULL;
    S->sizeofdigit = 0;
    clearStack(S->operators);
    return TERM;
}

void freeState(State *S)
{
    free(S->res);    
    freeStack(S->operators);
    free(S);    
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
void writeNumber(State *S)
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
}

/* writeOperators: записывает в результат операторы из стека в соответствии с функцией f */
void writeOperators(char c, State *S, cond f)
{
    while (!f(S->operators, c))
    {
	S->res = realloc(S->res, S->reslen + 2);
	char op;
	pop(S->operators, &op);
        S->res[S->reslen] = op;
        S->res[S->reslen + 1] = '\x20';
        S->reslen += 2;
    }
}

/* condIfEmpty: используется для выброса всех элементов из стека */
int condIfEmpty(Stack *S, char c)
{
    return emptyp(S);
}

/* condIfPriority: используется для выброса тех элементов стека, приоритет которых больше или равен данному */
int condIfPriority(Stack *S, char c)
{
    int cond1, cond2;

    if (emptyp(S)) return 1;

    cond1 = inSet(*((char *)peek(S)), s_binop) / 2;
    cond2 = inSet(c, s_binop) / 2;
    
    return cond1 < cond2;
}

/* DtoB: переход от ЦИФРА к ОПЕРАТОР */
States DtoB(char c, State *S)
{
    writeNumber(S);
    writeOperators(c, S, condIfPriority);
    
    push(S->operators, &c);

    return BINOP;
}

States exitNoWrite(char c, State *S)
{
    free(S->res);
    S->res = NULL;
    (S->infpos)--;
    return TERM;
}

States exitWrite(char c, State *S)
{
    writeNumber(S);
    writeOperators(c, S, condIfEmpty); 
    S->res[S->reslen - 1] = 0;
    (S->infpos)--;
    return TERM;
}

States writePrevState(char c, State *S)
{
    S->pstate = S->cstate;
    return SPC;
}

States checkIfBinop(char c, State *S)
{
    if (S->pstate != BINOP)
	return exitNoWrite(c, S);
    if (inSet(c, s_sign) >= 0)
	return changeSign(c, S);
    else if (inSet(c, s_digit) >= 0)
	return addDigit(c, S);
    else return exitNoWrite(c, S);    
}

States checkIfDigit(char c, State *S)
{
    if (S->pstate != DIGIT)
	return exitNoWrite(c, S);
    if (inSet(c, s_binop) >= 0)
	return DtoB(c, S);
    else if (inSet(c, s_term) >= 0)
	return exitWrite(c, S);
    else return exitNoWrite(c, S);
}

States nilf(char c, State *S)
{
    return SPC;
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
    State *C = initState();
    
    char *ret = NULL; // возвращаемое значение
    while(infix[C->infpos] == '\x20') (C->infpos)++; // пропуск ведущих пробелов

    matf trmat[SETAMT][SETAMT] =
	{
	    {changeSign,   addDigit,     exitNoWrite,  exitNoWrite,  exitNoWrite   },
	    {exitNoWrite,  addDigit,     DtoB,         exitWrite,    writePrevState},
	    {changeSign,   addDigit,     exitNoWrite,  exitNoWrite,  writePrevState},
	    {exitState,    exitState,    exitState,    exitState,    exitState     },
	    {checkIfBinop, checkIfBinop, checkIfDigit, checkIfDigit, nilf          }
	};

    while(C->running)
    {
	char cur = infix[C->infpos];
	int column = whichSet(cur, C);
	
	C->cstate = column < 0 ? exitNoWrite(cur, C) : (trmat[C->cstate][column])(cur, C);

	printf("%d\n", C->cstate);
	
	(C->infpos)++;
    }

    ret = malloc(C->reslen + 1);
    strcpy(ret, C->res);
    freeState(C);
  
    return ret;
}

void printops(Stack *this)
{
    putchar('{');

    size_t cur = getsize(this);
    const char *const head = peek(this);
    
    for (size_t stkcur = 0; stkcur < cur; stkcur++)
    {
	putchar(*((char *)(head - stkcur)));
	putchar(' ');
    }

    printf("%c}\n", cur ? '\b' : '\0');
}


/* printState: выводит текущее состояние автомата */
void printState(State *S)
{    
    printf("resptr: %p\nrespos: %u\ncurstate: %d\ninfixpos: %u\ncdptr: %p\ncdsize: %u\nsign: %d\nstack contents: ",
	   S->res,
	   S->reslen,
	   S->cstate,
	   S->infpos,
	   S->currentdgt,
	   S->sizeofdigit,
	   S->sign);
    
    printStack(S->operators);
    
    printf("\nres contents: ");
    for (unsigned i = 0; i < S->reslen; i++)
    {
	printf("%c", S->res[i]);
    }
    printf("\ncd contents: ");
    for (unsigned i = 0; i < S->sizeofdigit; i++)	
    {
	printf("%c", S->currentdgt[i]);
    }
    printf("\n\n");
}
