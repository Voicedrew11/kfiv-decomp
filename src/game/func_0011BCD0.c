#include "common.h"
#include "kf_player.h"

extern u16 D_00413DA8[];
extern PlayerState D_00413F80;
extern f32 D_00431AF0[16];      /* the unit matrix */

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0MulMatrix(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
void KF_Vector_Subtract(void *, void *, void *);
void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010A7E0(void *);
void func_0010B000(void);
void func_0011C160(void);
float func_0019B838(float);
void func_001B5460(void);
void func_0020B780(int);
void func_00216C48(void *, void *, void *);
void func_00216D20(void *, void *);

/* The free-camera viewer of func_0011C160's picture: runs its own frame loop, in which the pad turns the
   camera about the player (with 0x4000 held) or moves it, and both 0x400 and 0x800 put it back. */
void func_0011BCD0(void) {
    VuVector4 position;
    VuVector4 rotation;
    int reset_held;

    reset_held = FALSE;
    memset(&position, 0, sizeof(position));
    position.z = 10000.0f;
    rotation = (VuVector4){ -1.5707964f, D_00413F80.rotation.y, 0.0f, 0.0f };
    {
        f32 world_screen[16];
        f32 view_screen[16];
        f32 world_view[16];
        f32 unused[16];
        int held;
        int pressed;

        sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        do {
            VuVector4 move;

            func_0020B780(0);
            func_001096E8();
            func_001095E0();
            func_0010B000();
            func_001B5460();
            held = D_00413DA8[0];
            pressed = D_00413DA8[1];
            move.x = move.y = move.z = move.w = 0.0f;
            if (held & 0x4000) {
                if (held & 0x40) {
                    rotation.x = func_0019B838(rotation.x + 0.08726646f);
                }
                if (held & 0x10) {
                    rotation.x = func_0019B838(rotation.x - 0.08726646f);
                }
                if (held & 0x80) {
                    rotation.y = func_0019B838(rotation.y + 0.08726646f);
                }
                if (held & 0x20) {
                    rotation.y = func_0019B838(rotation.y - 0.08726646f);
                }
            } else {
                if (held & 0x80) {
                    move.x = -1.0f;
                }
                if (held & 0x20) {
                    move.x += 1.0f;
                }
                if (held & 0x10) {
                    move.y -= 1.0f;
                }
                if (held & 0x40) {
                    move.y += 1.0f;
                }
            }
            if (!reset_held) {
                if ((held & 0xC00) == 0xC00) {
                    reset_held = TRUE;
                    position.z = 10000.0f;
                    rotation.x = -1.5707964f;
                    rotation.y = D_00413F80.rotation.y;
                    position.x = 0.0f;
                    position.y = 0.0f;
                    position.w = 0.0f;
                    rotation.z = 0.0f;
                    rotation.w = 0.0f;
                } else if (held & 0x400) {
                    move.z += 1.0f;
                } else if (held & 0x800) {
                    move.z -= 1.0f;
                }
            } else if (!(held & 0xC00)) {
                reset_held = FALSE;
            }
            if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
                func_00216D20(&move, &move);
                KF_Vector_Scale(&move.x, &move.x, 100.0f);
                func_00216C48(&position, &position, &move);
                if (position.x < D_00413F80.position.x + -48000.0f) {
                    position.x = D_00413F80.position.x + -48000.0f;
                }
                if (position.x > D_00413F80.position.x + 48000.0f) {
                    position.x = D_00413F80.position.x + 48000.0f;
                }
                if (position.y < D_00413F80.position.y + -48000.0f) {
                    position.y = D_00413F80.position.y + -48000.0f;
                }
                if (position.y > D_00413F80.position.y + 48000.0f) {
                    position.y = D_00413F80.position.y + 48000.0f;
                }
                if (position.z < 2500.0f) {
                    position.z = 2500.0f;
                }
                if (position.z > 50000.0f) {
                    position.z = 50000.0f;
                }
            }
            KF_Vector_Scale(&move.x, &D_00413F80.position.x, -1.0f);
            sceVu0TransMatrix(world_view, D_00431AF0, &move.x);
            sceVu0RotMatrixY(world_view, world_view, -rotation.y);
            sceVu0RotMatrixX(world_view, world_view, -rotation.x);
            sceVu0TransMatrix(world_view, world_view, &D_00413F80.position.x);
            KF_Vector_Subtract(&move, &D_00413F80, &position);
            KF_Vector_Scale(&move.x, &move.x, -1.0f);
            sceVu0TransMatrix(world_view, world_view, &move.x);
            sceVu0MulMatrix(world_screen, view_screen, world_view);
            func_0010A7E0(world_screen);
            func_001095D0();
            func_0011C160();
            func_001095D8();
            // match: it returns a value here, which keeps $2 busy
            ((int (*)(void))func_00109800)();
        } while (!(pressed & 0x2000));
    }
}
