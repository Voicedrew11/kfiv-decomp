#include "common.h"

/* EE EABI varargs: the register save area a1..a7 is spilled to, as in libsdr.c */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (char *)__builtin_next_arg(last) - 0x38)

extern int D_004471DC;
extern int D_004471EC;
extern u8 D_005C5A18[];
int vsprintf(char *, const char *, char *);

/* Formats a message into the next 0x108-byte debug log slot. */
void func_001CFBB0(char *fmt, ...) {
    va_list ap;
    int n;

    va_start(ap, fmt);
    n = vsprintf(D_005C5A18 + D_004471EC * 0x108, fmt, ap);
    D_004471DC += n;
    D_004471EC++;
}
