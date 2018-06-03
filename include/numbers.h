#ifndef LONGCALC_H_NUMBERS
#define LONGCALC_H_NUMBERS

/* В заголовочном файле numbers.h определены функции, производящие операции
   над массивами символов, представляющими числа в дополнительном коде. */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))

typedef char *number;

/* shift: сдвинуть строку с числом на n разрядов влево */
void shift(number *a, unsigned int n, unsigned int l)
{
    if (!n) return;
    for (int i = n; i <= l; i++) // сдвиг
    {
	(*a)[i - n] = (*a)[i];
    }

    *a = realloc(*a, l - n + 1); // перераспределить ненужную память
}

/* stripsign: вернуть позицию мантиссы числа в дополнительном коде */
unsigned int stripsign(number a)
{
    unsigned int ret = 0;

    while ((a[ret] == a[ret + 1]) && a[ret + 2]) ret++;
    return ret;
}

/* inv: возвращает -a в дополнительном коде */
number inv(number a)
{
    number ia = malloc( (strlen(a) + 1) * sizeof(char) );
    signed i = strlen(a);
    ia[i] = 0;
    i--;

    char cf = 0;
    while(i >= 0)
    {
	ia[i] = ('9' - a[i] + cf + !a[i + 1]) % 10 + '0'; // !a[i+1] - проверка на крайний правый символ
	cf = ('9' - a[i] + cf + !a[i + 1]) / 10;
	i--;
    }

    shift(&ia, stripsign(ia), strlen(ia));
    return ia;
}

/* add: возвращает результат сложения a и b, представленных в доп. коде */

number add(number a, number b)
{
    unsigned int lena = strlen(a);
    unsigned int lenb = strlen(b);
    number mtmp = (MAX(lena, lenb) == lena ? a : b);
    unsigned int lmtmp = strlen(mtmp) + 2;
    number c = malloc( lmtmp * sizeof(char) );

    char cf = 0; // carry flag
    unsigned int rpos = lmtmp - 1;
    c[rpos] = 0;
    rpos--;

    while(lena || lenb)
    {
	/* Если одно из чисел короче другого,
	 происходит чтение его знака из первого элемента строки. */
	int calctmp = (a[lena ? lena - 1 : 0] + b[lenb ? lenb - 1 : 0] + cf - 2 * '0'); // two zeros to substract
	c[rpos] = (calctmp % 10) + '0'; // one zero to add
	cf = calctmp / 10; // carry flag setup
	lena = lena ? lena - 1 : lena;
	lenb = lenb ? lenb - 1 : lenb;
	rpos--;
    }

    /* Дополнительное сложение знаков ( первых символов ) */
    int signex = *a + *b - 2 * '0' + cf;
    c[rpos] = (signex % 10) + '0';

    /* Знакорасширение */
    for (int i = rpos - 1; i >= 0; i--)
    {
	c[i] = c[i + 1];
    }

    /* Сдвиг */
    shift(&c, stripsign(c), strlen(c));
    return c;
}

/* sub: возвращает результат вычитания b из a, представленных в ДК
   реализовано при помощи функций сложения и инверса
   a - b = a + (-b) */

number sub(number a, number b)
{
    number i = inv(b);
    number r = add(a, i);
    free(i);
    return r;
}

/* mul: возвращает результат умножения a на b, представленных в ДК
   происходит вычисление модулей a и b, умножение "в столбик"
   и инвертирование в зависимости от совокупности знаков */

number mul(number a, number b)
{
    // Операция МОД2 вернет 0, если знаки совпадут, и 9, если нет
    char sign = (*a ^ *b);

    char cf = 0;
    number mula = (*a == '0' ? a : inv(a)); //модуль a
    number mulb = (*b == '0' ? b : inv(b)); //модуль b

    /* длины модулей */
    unsigned int lena = strlen(mula);
    unsigned int lenb = strlen(mulb);
    unsigned int lenc = lena + lenb;
    number ct = calloc(lenc, sizeof(char));

    /* массив для хранения временных результатов умножения */
    unsigned long long *tmp = calloc(lenc, sizeof(unsigned long long));

    for (int i = lenb - 1; i >= 0; i--)
    {
	for (int j = lena - 1; j >= 0; j--)
	{
	    tmp[i + j] += ((mulb[i] - '0') * (mula[j] - '0') + cf) % 10; // вычисление элемента
	    cf = ((mulb[i] - '0') * (mula[j] - '0') + cf) / 10;
	}
    }

    /* сдвиг полученных временных значений для получения числа */
    cf = 0;
    for (int i = lenc - 2; i > 0; i--)
    {
        tmp[i - 1] += tmp[i] / 10; //carryflag kinda
	tmp[i] %= 10;
	ct[i] = tmp[i] + '0';
    }

    free(tmp);

    /* освобождение модулей, если на них была выделена память */
    if (*a == '9') free(mula);
    if (*b == '9') free(mulb);

    *ct = '0'; // образование модуля произведения
    if (!sign) // одинаковые знаки
    {
	shift(&ct, stripsign(ct), strlen(ct));
	return ct;
    }
    else // разные знаки
    {
	number ctr = inv(ct);
	free(ct);
	shift(&ctr, stripsign(ctr), strlen(ctr));
	return ctr;
    }
}

#endif
