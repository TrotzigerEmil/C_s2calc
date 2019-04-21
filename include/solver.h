#ifndef LONGCALC_H_SOLVER
#define LONGCALC_H_SOLVER

#include "stack.h"

/* Алгоритм RPN-Parse --------------------------------- */

#define DELIM "\x20" // разделитель
#define ADD '+' // оператор сложения
#define SUB '-' // оператор вычитания
#define MUL '*' // оператор умножения
#define DIV '/' // оператор целочисленного деления

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
#define SETAMT 5 // количество множеств

typedef struct state  State;
typedef struct set    Set;
typedef enum states
{
    SIGN = 0, // ЗНАКЧИСЛА
    DIGIT = 1, // ЦИФРА
    BINOP = 2, // ОПЕРАТОР
    TERM = 3, // ТЕРМИНАТОР
    SPC = 4 // ПРОБЕЛ
} States;

typedef int    (*cond)(struct stack *, char);
typedef States (*matf)(char, State *);

State *initState(void);
void freeState(State *S);
void resetState(State *S);

void printState(State *S);

int inSet(char c, Set set); // позиция c в set или -1
int whichSet(char c, State *S);

States exitState(char c, State *S);

States changeSign(char c, State *S);
States addDigit(char c, State *S);
States DtoB(char c, State *S);

States exitNoWrite(char c, State *S);
States exitWrite(char c, State *S);

States checkIfBinop(char c, State *S);
States checkIfDigit(char c, State *S);

States writePrevState(char c, State *S);
States nilf(char c, State *S);

/* writeNumber: дописывает полученное число в конец результата */
void writeNumber(State *S);

/* writeOperators: записывает в результат операторы из стека в соответствии с функцией f */
void writeOperators(char c, State *S, cond f);

/* condIfEmpty: используется для выброса всех элементов из стека */
int condIfEmpty(Stack *S, char c);

/* condIfPriority: используется для выброса тех элементов стека, приоритет которых больше или равен данному */
int condIfPriority(Stack *S, char c);

/* InfixToRPN: возвращает строку с выражением в ПФЗ или NULL, если преобразование не удалось */
char *InfixToRPN(char *infix);

void printops(struct stack *);

/* ----------------------------------------------------- */

#endif
