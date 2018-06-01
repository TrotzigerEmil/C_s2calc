#include "repl.h"
#include <stdlib.h>

#if defined (_WIN32) || defined (_WIN64)
#define CLRCMD "cls"
#else
#define CLRCMD "clear"
#endif

int main(void)
{
    system(CLRCMD);
    return repl();
}
