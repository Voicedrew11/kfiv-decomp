#include "common.h"
#include "kf_load.h"
#include "kf_file.h"
#include "kf_map.h"

extern s32 D_004468A8;
extern s32 D_004470DC;
extern s32 D_004470F4;
extern char D_004FF440[];
extern MapHeader *D_0038CD28[];
extern s32 func_001CBD20(void);
extern s32 sprintf(char *, char *, ...);

/* Step a two-phase load: start reading the named file into the buffer, then wait for it and close. */
s32 func_0019F3A0(LoadRequest *request) {
    u32 buffer;

    if (func_001CBD20() != 0) {
        switch (D_004468A8) {
        case 0:
            buffer = D_004470F4;
            request->target->buffer = (void *)buffer;
            D_004470F4 = (((buffer + 0x2003F) >> 6) << 6);
            sprintf(D_004FF440, request->name, D_0038CD28[0]->unk70);
            D_004470DC = func_00102238(D_004FF440, TRUE, TRUE);
            func_001025C0(D_004470DC, request->target->buffer, -1);
            D_004468A8 += 1;
            break;
        case 1:
            if (func_00102790(D_004470DC, 1) == 0) {
                func_001024B0(D_004470DC);
                D_004468A8 = 0;
                return 1;
            }
            break;
        }
    }
    return -1;
}
