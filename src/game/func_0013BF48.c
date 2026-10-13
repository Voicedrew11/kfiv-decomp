#include "common.h"
#include "kf_entity_world.h"

typedef struct {
    int a[5];
} Tbl;

typedef struct {
    char pad0[0xD8];
    s64 flags;
    char pade0[0x7A];
    char tag;
} Obj;

extern Tbl D_0043E890;
int func_0013BF48(Obj *arg0, int arg1) {
    Tbl t = D_0043E890;
    int i;
    Obj *o;

    for (i = 0; i < 5; i++) {
        o = Entity_GetBySpawnId(t.a[i]);
        if (o != 0 && (o->flags & 0xD) == 1 && o != arg0 && o->tag == arg1) {
            return 0;
        }
    }
    return 1;
}
