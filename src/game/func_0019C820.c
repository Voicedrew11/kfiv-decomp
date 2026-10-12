#include "common.h"

int func_001CDBC0(u32, s16);

extern int D_003EF5F8[];
extern int D_003EF628[];
extern int D_003EF658[];
extern int D_003EF688[];
extern int D_003EF6B8[];

/* Apply one level to nine slots: the five globals, then slots 6 to 9. */
void func_0019C820(int level) {
    s16 short_level = level;

    func_001CDBC0(D_003EF5F8[0], short_level);
    func_001CDBC0(D_003EF6B8[0], short_level);
    func_001CDBC0(D_003EF688[0], short_level);
    func_001CDBC0(D_003EF658[0], short_level);
    func_001CDBC0(D_003EF628[0], short_level);
    func_001CDBC0(6, short_level);
    func_001CDBC0(7, short_level);
    func_001CDBC0(8, short_level);
    func_001CDBC0(9, short_level);
}
