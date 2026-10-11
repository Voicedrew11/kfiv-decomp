#include "kf_ent.h"

Ent *func_00124C08(Ent *);
Ent *func_00124CA0(Ent *);
Ent *func_00124D48(Ent *);
void func_00123AB8(Ent *);
void func_001243E8(char);
void func_001A32A8(Ent *);

/* Free every entity in group arg0 from all three lists. */
void func_001231C0(int arg0) {
    Ent *s;

    s = D_004470A8;
    while (s != 0) {
        if (s->unkD5 == arg0) {
            s = func_00124C08(s);
        } else {
            s = s->next;
        }
    }

    s = D_004470B0;
    while (s != 0) {
        if (s->unkD5 == arg0) {
            s = func_00124CA0(s);
        } else {
            s = s->next;
        }
    }

    s = D_004467E0;
    while (s != 0) {
        if (s->unkD5 == arg0) {
            func_001243E8(s->unkD3);
            func_00123AB8(s);
            s = func_00124D48(s);
            func_001A32A8(s);
        } else {
            s = s->next;
        }
    }
}
