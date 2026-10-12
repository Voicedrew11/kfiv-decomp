#include "common.h"

extern void *D_003EF700[];

char func_0019C8D0(int index, int first, int second, int third, int fourth) {
    int (*handler)(int, int, int, char);

    handler = D_003EF700[index];
    return handler(first, second, third, (char)fourth);
}
