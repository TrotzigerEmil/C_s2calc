#ifndef LONGCALC_H_SOLVER
#define LONGCALC_H_SOLVER

#include "stack.h"

/* Алгоритм RPN-Parse --------------------------------- */

#define DELIM "\x20" // разделитель
#define ADD "+" // оператор сложения
#define SUB "-" // оператор вычитания
#define MUL "*" // оператор умножения
#define DIV "/" // оператор целочисленного деления

/* isNumber: возвращает 0, если str состоит только из цифр, и ненулевое значение
   в противном случае */
char isNumber(char *str);

/* parseString: возвращает результат вычисления выражения,
   записанного в обратной польской нотации
   (постфиксная форма записи, ОПН) */
char *parseString(char *rpnstr);

/* ---------------------------------------------------- */

/* Алгоритм Infix-To-RPN ------------------------------ */

#define SETSIZE 11 // максимальный размер множества символов

typedef struct state  State;
typedef struct set    Set;
typedef enum states
{
    SIGN, // ЗНАКЧИСЛА
    DIGIT, // ЦИФРА
    BINOP, // ОПЕРАТОР
    TERM, // ТЕРМИНАТОР
    SPC // ПРОБЕЛ
} States;
typedef int    (*cond)(Stack, char);

/* inSet: возвращает позицию символа в заданном множестве, или -1, если символа нет во множестве */
int inSet(char c, Set set);

/* freeState: перевод автомата в конечное состояние */
States freeState(State *S);

/* changeSign: меняет знак считываемого числа на противоположный */
States changeSign(char c, State *S);

/* addDigit: добавляет цифру в конец текущего числа */
States addDigit(char c, State *S);

/* writeDigit: дописывает полученное число в конец результата */
States writeDigit(State *S, States ret);

/* writeOperators: записывает в результат операторы из стека в соответствии с функцией f */
States writeOperators(char c, State *S, cond f, States ret);

/* condIfEmpty: используется для выброса всех элементов из стека */
int condIfEmpty(Stack S, char c);

/* condIfPriority: используется для выброса тех элементов стека, приоритет которых больше или равен данному */
int condIfPriority(Stack S, char c);

/* DtoB: переход от ЦИФРА к ОПЕРАТОР */
States DtoB(State *S, char c);

/* printState: выводит текущее состояние автомата */
void printState(State S);

/* InfixToRPN: возвращает строку с выражением в ПФЗ или NULL, если преобразование не удалось */
char *InfixToRPN(char *infix);

/* ----------------------------------------------------- */

#endif
