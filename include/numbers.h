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
int cmp(number a, number b);

/* shift: сдвинуть строку с числом на n разрядов влево */
void shift(number *a, unsigned int n, unsigned int l);

/* stripsign: вернуть позицию мантиссы числа в дополнительном коде */
unsigned int stripsign(number a);

/* add: возвращает результат сложения a и b, представленных в доп. коде */
number add(number a, number b);

/* inv: возвращает -a в доп. коде, вариант со сложением */
number inv(number a);

/* sub: возвращает результат вычитания b из a, представленных в ДК
   реализовано при помощи функций сложения и инверса
   a - b = a + (-b) */

number sub(number a, number b);

/* mul: возвращает результат умножения a на b, представленных в ДК
   происходит вычисление модулей a и b, умножение "в столбик"
   и инвертирование в зависимости от совокупности знаков */

number mul(number a, number b);

/* dvr: целочисленное деление с остатком
   возвращает частное из функции или NULL при делении на 0
   остаток возвращается через параметр функции r */
number dvr(number a, number b, number *r);

#endif
