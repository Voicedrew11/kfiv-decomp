#include "common.h"

/* EE EABI varargs: the register save area a1..a7 is spilled to, as in libsdr.c */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (char *)__builtin_next_arg(last) - 0x38)

void func_00217EE0(char *, char *);

/* printf into the text log. */
void func_00217F60(char *format, ...) {
    va_list args;

    va_start(args, format);
    func_00217EE0(format, args);
}
