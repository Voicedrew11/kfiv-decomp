/* libdev.a: sceDevVu0GetCnd (KFIV: not in ICO; the name is inferred from what the
   code does, not taken from SDK headers) */
#include <libdev_internal.h>

int sceDevVu0GetCnd(void)
{
    unsigned int s;
    int ret = 0;

    LIBDEV_CFC2(s, vi29);
    if (s & 0x200) {
        ret = 1;
    } else if (s & 0x400) {
        ret = 2;
    } else if (s & 0x800) {
        ret = 3;
    } else if (s & 0x100) {
        ret = 4;
    }
    return ret;
}
