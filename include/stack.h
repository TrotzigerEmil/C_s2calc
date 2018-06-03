#ifndef LONGCALC_H_STACK
#define LONGCALC_H_STACK

/* В заголовочном файле stack.h определена структура данных типа "стек", хранящая в
   качестве данных строки символов, а также функции, производящие операции над сте-
   ком (инициализация, добавление на вершину, удаление с вершины, удаление стека). */

#include <stdlib.h>
#include <string.h>

struct node; // элемент стека
typedef struct node *nptr;

typedef struct node
{
    char *data;
    nptr next;
} StackNode;

typedef struct
{
    nptr head;
} Stack;

/* initStack: инициализация стека начальными значениями */
Stack initStack(void)
{
    Stack ret = {NULL};
    return ret;
}

/* push: добавление новых данных на вершину стека */
void push(char *data, Stack *S)
{
    StackNode *new = malloc(sizeof(StackNode));
    new->data = malloc(strlen(data) + 1);
    strcpy(new->data, data);
    new->next = S->head;
    S->head = new;
}

/* pop: извлечение данных с вершины стека */
char *pop(Stack *S)
{
    if (!(S->head)) return NULL;

    char *ret = malloc(strlen(S->head->data) + 1);
    strcpy(ret, S->head->data);

    nptr nexth = S->head->next;
    free(S->head->data);
    free(S->head);
    S->head = nexth;

    return ret;
}

/* isEmpty: возвращает 1, если стек пуст, 0 в противном случае */
int isEmpty(Stack S)
{
    return !(S.head);
}

/* killStack: освобождение памяти, занятой под данные стека и его структуру */
void killStack(Stack *S)
{
    while (!isEmpty(*S))
    {
	char *tmp = pop(S); // структура освобождена
	free(tmp); // данные освобождены
    }
}
#endif
