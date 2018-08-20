#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"

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

/* peek: просмотр вершины стека без ее удаления */
char *peek(Stack *S)
{
    return (S->head) ? (S->head->data) : NULL;
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
