#include "kf_entity.h"

extern ObjectReference *D_0044712C;

int func_0019D020(int);

/* Plays the sound for the kind of object D_0044712C refers to. */
int func_001BDD38(void) {
    u16 kind;

    if (D_0044712C != NULL) {
        kind = D_0044712C->unk08->unk30;
        switch (kind) {
        case 0x56:
            return func_0019D020(0x22);
        case 0x167:
        case 0x1B1:
            return func_0019D020(0x20);
        default:
            func_0019D020(0x20);
            break;
        }
    }
}
