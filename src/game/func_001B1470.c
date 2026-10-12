#include "kf_entity.h"

#define SCRATCH_HIT_KIND (*(s32 *)0x70000018)

s32 func_001B1510(Unk1B1470Request *, Unk1B1470Extra *);
int func_001B1B80(void);
int func_001B1B88(void);
s32 func_001B1B90(Unk1B1470Request *, Unk1B1470Extra *);

/* Runs an attack request against the world, by the request's shape (0 to 3). Only shapes
   0 and 3 do anything. */
s32 func_001B1470(Unk1B1470Request *request, Unk1B1470Extra *extra) {
    SCRATCH_HIT_KIND = 6;
    switch ((u8)((Unk1B1470RequestWord *)request)->flags & 3) {
    case 0:
        return func_001B1510(request, extra);
    case 1:
        return func_001B1B80();
    case 2:
        return func_001B1B88();
    case 3:
        return func_001B1B90(request, extra);
    }
    return 0;
}
