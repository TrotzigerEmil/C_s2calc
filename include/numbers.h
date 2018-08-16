#ifndef LONGCALC_H_NUMBERS
#define LONGCALC_H_NUMBERS

/* В заголовочном файле numbers.h определены функции, производящие операции
   над массивами символов, представляющими числа в дополнительном коде. */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define LZ(A) (*(A) == '9')

#define LONGC_CONST_ZERO "00"
#define LONGC_CONST_ONE "01"

typedef char *number;

/* cmp: сравнить два числа; возвращает 0, если числа равны, <0, если a < b, и >0, если a > b */
int cmp(number a, number b)
{
    if (LZ(a) && !LZ(b)) return -1;
    if (!LZ(a) && LZ(b)) return 1;
    
    char *ap = a, *bp = b;
    while (*ap == *bp && *ap && *bp) {ap++; bp++;}
    return *ap - *bp;
}

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

/* inv: возвращает -a в доп. коде, вариант со сложением */
number inv(number a)
{
    int len = strlen(a);
    int i;
    
    number invtmp = malloc((len + 1) * sizeof(char));
    for (i = 0; i < len; i++)
    {
	invtmp[i] = '9' - a[i] + '0';
    }
    invtmp[i] = 0;

    number ret = add(invtmp, LONGC_CONST_ONE); // сдвиг тут
    
    free(invtmp);
    return ret;
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
    number mula = (!LZ(a) ? a : inv(a)); //модуль a
    number mulb = (!LZ(b) ? b : inv(b)); //модуль b

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

/* dvr: целочисленное деление с остатком
   возвращает частное из функции или NULL при делении на 0
   остаток возвращается через параметр функции r
   (пока не реализовано) */
number dvr(number a, number b, number *r)
{
    char sign = (*a ^ *b); // знак результата
    
    if (!cmp(b, LONGC_CONST_ZERO)) {return NULL;} // деление на 0
    
    number diva = (!LZ(a) ? a : inv(a)); // модули
    number divb = (!LZ(b) ? b : inv(b));

    unsigned lena = strlen(diva); // длины модулей
    unsigned lenb = strlen(divb);

    number divc = malloc(2 * sizeof(char)); // результат
    *divc = '0';
    unsigned cpos = 1;

    number rem = malloc((lenb + 1) * sizeof(char));
    int rempos;
    for (rempos = 0; rempos < lenb && diva[rempos]; rempos++)
    {
	*(rem + rempos) = *(diva + rempos);
    }
    *(rem + rempos) = 0;

    do
    {
	int curdgt = 0;	
	number looped = sub(rem, divb);
	
	while (!LZ(looped))
	{
	    curdgt++;
	    free(rem);
	    rem = looped;	    
	    looped = sub(rem, divb);
	}

	divc[cpos++] = '0' + curdgt;
	divc = realloc(divc, (cpos + 1) * sizeof(char));
	
	free(rem);
        rem = add(looped, divb);
	free(looped);
	int remlen = strlen(rem);
        rem = realloc(rem, (remlen + 2) * sizeof(char));
	rem[remlen] = diva[rempos];
        rem[remlen + 1] = 0;
	
    } while (a[rempos++]);

    divc[cpos] = 0;

    if (!r) // остаток
	free(rem);
    else
    {
	if (LZ(a)) // a == ((a / b) * a) + a % b
	{
	    number tmp = inv(rem);
	    number swp = tmp;
	    tmp = rem;
	    rem = swp;
	    free(tmp);	    
	}
	*r = rem;
    }

    if (LZ(a)) free(diva);
    if (LZ(b)) free(divb);

    if (!sign)
    {
	shift(&divc, stripsign(divc), strlen(divc));
	return divc;
    }
    else
    {
	number divcinv = inv(divc);
	free(divc);
	return divcinv;
    }
}
#endif
