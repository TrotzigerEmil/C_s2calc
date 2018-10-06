#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "stack.h"

typedef struct stack
{
    void *data;
    size_t esize;
    size_t ssize;
    size_t cur;
    size_t bsize;
    int flags;
} Stack;

struct stack *mallocStack(size_t elsize, size_t maxsize, int flag)
{
    if (flag != ERROR && flag != EXPAND) return NULL;
    if (!elsize) return NULL;
    
    if (maxsize < BSIZE) maxsize = BSIZE;
    
    struct stack *ret = malloc(sizeof(struct stack));
    ret->esize = elsize;
    ret->ssize = ret->bsize = maxsize;
    ret->flags = flag;
    ret->data = malloc(ret->esize * ret->ssize);
    ret->cur = 0;

    return ret;
}

int push(struct stack *stk, void *data)
{
    char changed = 0;
    
    if (stk->cur == stk->ssize)
    {
	if (stk->flag == EXPAND)
	{
	    void *tmp = realloc(stk->data, stk->ssize + stk->bsize);
	    if (!tmp)		
		return ERROR;
	    stk->data = tmp;
	    stk->ssize += stk->bsize;
	    changed = 1;
	}
	else return ERROR;
    }

    memcpy(stk->data + stk->esize * stk->cur, data, stk->esize);
    (stk->cur)++;
    return changed ? EXPAND : SUCCESS;
}

int pop(struct stack *stk, void *dst)
{
    if (!stk) return ERROR;

    memcpy(dst, stk->data + (stk->cur - 1) * (stk->esize), stk->esize);
    (stk->cur)--;

    if (stk->cur <= stk->ssize - stk->bsize && stk->flags == EXPAND && stk->cur)
    {
	stk->data = realloc(stk->data, stk->ssize - stk->bsize);
	stk->ssize -= stk->bsize;
    }

    return stk->ssize == stk->cur ? EXPAND : SUCCESS;
}

const void *const peek(struct stack *stk)
{
    return stk->cur ? stk->data + (stk->cur - 1) * stk->esize : NULL;
}

int emptyp(struct stack *stk)
{
    return !(stk->cur);
}

void freeStack(struct stack *stk)
{
    free(stk->data);
    free(stk);
}
