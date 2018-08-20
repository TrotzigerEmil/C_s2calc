#ifndef LONGCALC_H_STACK
#define LONGCALC_H_STACK

/* В заголовочном файле stack.h определена структура данных типа "стек", хранящая в
   качестве данных строки символов, а также функции, производящие операции над сте-
   ком (инициализация, добавление на вершину, удаление с вершины, удаление стека). */

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
Stack initStack(void);

/* push: добавление новых данных на вершину стека */
void push(char *data, Stack *S);

/* pop: извлечение данных с вершины стека */
char *pop(Stack *S);

/* peek: просмотр вершины стека без ее удаления */
char *peek(Stack *S);

/* isEmpty: возвращает 1, если стек пуст, 0 в противном случае */
int isEmpty(Stack S);

/* killStack: освобождение памяти, занятой под данные стека и его структуру */
void killStack(Stack *S);

#endif
