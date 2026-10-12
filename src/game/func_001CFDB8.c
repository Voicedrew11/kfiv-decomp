#include "common.h"

/* EE EABI varargs, as in func_001CF988 */
typedef char *va_list;
#define va_arg(ap, type) ((ap) += 8, *(type *)((ap) - 8))

int func_001D0758(int, u8 *);

/* Expands a message's glyph stream: copies it to `out` up to the 0xFFFF end mark, replacing each
   number code (0x1000: digits in the message font, padded with blanks; 0x2000: raw digits, padded
   with 0x4AF) by the next argument printed in the width the code gives. Returns the number of
   glyphs written, not counting control codes. */
int func_001CFDB8(u16 *format, u16 *out, va_list args) {
    u8 digits[0x20];
    int count;
    int width;
    int digit_count;
    int i;

    count = 0;
    while (*format != 0xFFFF) {
        if (*format & 0x3000) {
            width = (*format & 0xF00) >> 8;
            digit_count = func_001D0758(va_arg(args, int), digits);
            if (*format & 0x1000) {
                if (digit_count < width) {
                    for (i = 0; i < width - digit_count; i++) {
                        *out++ = 0x800;
                        count++;
                    }
                }
                for (i = digit_count - 1; i >= 0; i--) {
                    *out++ = (s8)digits[i] + 0x810;
                    count++;
                }
            } else if (*format & 0x2000) {
                if (digit_count < width) {
                    for (i = 0; i < width - digit_count; i++) {
                        *out++ = 0x4AF;
                        count++;
                    }
                }
                for (i = digit_count - 1; i >= 0; i--) {
                    *out++ = (s8)digits[i];
                    count++;
                }
            }
        } else {
            *out = *format;
            if (!(*out++ & 0xF000)) {
                count++;
            }
        }
        format++;
    }
    *out = *format;
    return count;
}
