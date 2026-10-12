#include "common.h"
#include "kf_model.h"
#include "kf_sound.h"

extern char D_004470E1;
extern int D_003EF648[];
extern SoundChannel D_00500170[];
extern SoundChannel D_003EF710[];

s32 func_0019D980(s8, u8, u8, f32, Vector4Arg *);
s32 func_0019DAA8(s8, u8, u8, Vector4Arg *, Vector4Arg *, f32);
s8 func_0019C700(void);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Starts the sounds queued in D_00500170 now that the driver is free, freeing each request. */
void func_0019E758(void) {
    int i;
    SoundChannel *request;

    if (D_004470E1 != 0) {
        for (i = 0; i < 0x80; i++) {
            request = &D_00500170[i];
            if (request->unk38 > 0) {
                switch (request->unk39) {
                case 1:
                    request->unk38 = -1;
                    func_0019D980(request->unk3A, request->unk3B, request->unk3C, request->volume, &request->origin);
                    break;
                case 2:
                    request->unk38 = -1;
                    // match: target is indexed off the array; through request it is not strength-reduced
                    func_0019DAA8(request->unk3A, request->unk3B, request->unk3C, &request->origin, &D_00500170[i].target, request->volume);
                    break;
                case 0x63:
                    D_003EF710[i].unk3A = request->unk3A;
                    D_003EF710[i].unk3C = 0x63;
                    D_003EF710[i].unk30 = request->unk30;
                    request->unk38 = -1;
                    D_003EF710[i].unk3B = func_0019C700();
                    func_001CE690(D_003EF710[i].unk3B, D_003EF648[4], 0, request->unk3B, 0x3C, request->unk30, 0, 0x3F);
                    break;
                }
            }
        }
    }
}
