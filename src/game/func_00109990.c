#include "common.h"

extern int D_00446748;          /* which of the two packet buffers is being filled */
extern int D_0044674C;          /* quadwords handed out */
extern int D_00446750;          /* bytes in both buffers together */
extern u32 *D_00447030[2];
extern u32 *D_00447038;         /* next free quadword */
extern int D_0044703C;          /* the buffer overflowed */

void *memset(void *, int, u32);

/* Takes quadwords from the packet buffer; once it overflows everything gets the start of the buffer. */
u32 *func_00109990(int quadwords) {
    u32 *result = D_00447038;

    D_0044674C += quadwords;
    if (D_0044703C == 0) {
        if (D_00446750 / 16 >= D_0044674C) {
            D_00447038 += quadwords * 4;
            return result;
        }
        D_0044703C = 1;
        D_00447038 = D_00447030[D_00446748];
        memset(D_00447038, 0, D_00446750 / 2);
    }
    return D_00447038;
}
