#ifndef LONGCALC_H_STACK
#define LONGCALC_H_STACK

/* В заголовочном файле stack.h определена структура данных типа "стек", хранящая в
   качестве данных строки символов, а также функции, производящие операции над сте-
   ком (инициализация, добавление на вершину, удаление с вершины, удаление стека). */

#define SUCCESS 0x00
#define ERROR 0x01  /* выдать ошибку при заполнении  */
#define EXPAND 0x02 /* расширить стек при заполнении */

#define BSIZE 4096

struct stack;

struct stack *mallocStack(size_t, size_t, int);
int push(struct stack *, void *);
int pop(struct stack *, void *);
const void *const peek(struct stack *);
int emptyp(struct stack *);
void freeStack(struct stack *);

#endif
