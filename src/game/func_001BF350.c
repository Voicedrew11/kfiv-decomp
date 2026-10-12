#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;
extern u32 D_00446ED4;

void func_00216778(void *, void *);
void func_001C2758(f32 *, s32, f32, f32);
s32 func_001C2A38(f32 *, s32, f32, f32);
void func_001BB360(void);

/* Collides the player with the map objects. With `climb` set the player is first tried a little
   higher, so a low object is stepped onto; a step higher than 16.67 is taken over several frames
   (status bit 33). An object touched for the first time starts the timed flag its definition
   names. */
void func_001BF350(s32 climb) {
    f32 old_position[4];
    s32 hit = FALSE;
    f32 reach;
    f32 rise;
    f32 step;

    D_00413F80.unk208 = 0;
    D_00413F80.unk20C = 0;
    if (climb) {
        func_00216778(old_position, &D_00413F80);
        D_00413F80.position.y += 1.0f;
        reach = D_00413F80.unk1AC + 75.0f;
        if (D_00414280.unk208 != 0) {
            D_00413F80.position.y += 30.0f;
            reach += 30.0f;
        }
        if (!((s32)(D_00413F80.unk1C8 >> 33) & 1)) {
            hit = func_001C2A38((f32 *)&D_00413F80, 1, reach, 100.0f);
        }
        if (hit) {
            step = 16.666666f;
            rise = old_position[1] - D_00413F80.position.y;
            if (step < rise && D_00414280.unk208 != D_00413F80.unk208) {
                D_00413F80.position.y = old_position[1] - step;
                D_00413F80.unk1D0 = rise - step + 5.0f;
                D_00413F80.unk1D4 = step;
                D_00413F80.unk1C8 |= 1L << 33;
            }
            D_00413F80.unk1AC = 0.0f;
            D_00413F80.unk1C8 &= ~(1L << 34);
        } else {
            func_00216778(&D_00413F80, old_position);
        }
    }
    if (!((s32)(D_00413F80.unk1C8 >> 33) & 1)) {
        func_001C2758((f32 *)&D_00413F80, 0, 230.0f, 100.0f);
    }
    if (D_00413F80.unk20C != 0 && D_00413F80.unk20C != D_00414280.unk20C) {
        switch ((s16)((MapObject *)D_00413F80.unk20C)->unk08->unk52) {
        case 1:
            D_00413F80.timed_flags[0] = 1800;
            D_00413F80.unk2C4 = D_00446ED4;
            break;
        case 2:
            D_00413F80.timed_flags[1] = 150;
            D_00413F80.unk2C8 = D_00446ED4;
            break;
        case 3:
            D_00413F80.timed_flags[2] = 3600;
            D_00413F80.unk2CC = D_00446ED4;
            break;
        case 4:
            D_00413F80.timed_flags[3] = 5400;
            D_00413F80.unk2D0 = D_00446ED4;
            break;
        case 5:
            D_00413F80.timed_flags[4] = 1800;
            D_00413F80.unk2D4 = D_00446ED4;
            break;
        case 6:
            D_00413F80.timed_flags[5] = 18000;
            D_00413F80.unk2D8 = D_00446ED4;
            break;
        }
        func_001BB360();
    }
    if (D_00413F80.unk208 != 0 && ((MapObject *)D_00413F80.unk208)->unk08->unk30 == 0x165) {
        D_00413F80.unk210 = 2;
    }
}
