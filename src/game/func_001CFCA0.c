#include "common.h"

extern int D_004471EC;
extern u8 D_005C5A18[];
void func_001D0298(int, int, int, int, int, int, int);

void func_001CFCA0(void)
{
    int i;
    int color = 0;
    int x = 0;
    int y = 0;
    u8 *s;
    u8 c;

    for (i = 0; i < D_004471EC; i++) {
        s = D_005C5A18 + i * 0x108;
        c = *s;
        if (c < 0x20) {
            color = (char)c;
        } else {
            for (; *s != 0; s++) {
                char ch = *s;
                if (ch == 10) {
                    func_001D0298(x, y, 0, color, 0x80, 2, 1);
                    x = 0;
                    y += 16;
                } else {
                    func_001D0298(x, y, ch - 0x20, color, 0x80, 2, 1);
                    x += 10;
                }
            }
        }
    }
}
