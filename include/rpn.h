#ifndef LONGCALC_H_RPN
#define LONGCALC_H_RPN

/* В заголовочном файле rpn.h определены функции, реализующие вычисление выражения в 
   обратной польской нотации (ОПН). */

#define DELIM "\x20" // разделитель
#define ADD "+" // оператор сложения
#define SUB "-" // оператор вычитания
#define MUL "*" // оператор умножения
#define DIV "/" // оператор целочисленного деления

#include "numbers.h"
#include "stack.h"

/* isNumber: возвращает 0, если str состоит только из цифр, и ненулевое значение
   в противном случае */
char isNumber(char *str)
{
    while (*str && *str >= '0' && *str <= '9') str++;
    return *str;
}

/* parseString: возвращает результат вычисления выражения,
   записанного в обратной польской нотации
   (постфиксная форма записи, ОПН) */
number parseString(char *rpnstr)
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
    
    char *result = malloc(strlen(operands.head->data) + 1); // выделить память под строку на вершине стека
    strcpy(result, operands.head->data); // скопировать в возвращаемое значение
    killStack(&operands); // освободить память, занятую стеком
    
    return result;
}

#endif
