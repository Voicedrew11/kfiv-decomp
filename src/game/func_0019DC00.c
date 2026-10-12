#include "kf_player.h"
#include "kf_sound.h"

extern PlayerState D_00413F80;
extern SoundChannel D_003EF710[];
extern int D_003EF648[];
extern u8 D_00349680[];
extern char D_00500158[];
extern int D_00446888;
extern int D_0044688C;
extern int D_00446890;
extern char D_00446894;
extern s8 D_004470E6;
double sqrt(double);
float atan2f(float, float);
int rand(void);
int func_001CE8B0(u32);
int func_001CE9A0(int kind, u32 slot, int value);
int func_0019E110(s32, Vector4 *);
float func_0019C640(float *, float *, float *);
void func_0019C598(float, float *, float *);
s8 func_0019C700(void);
int func_001CE690(u32, int, char, char, char, char, s16, char);

/* Each frame, updates the voice of every live sound channel: stops it out of range, else sets its volume and pan. */
void func_0019DC00(void) {
    float diff[4];
    float origin[4];
    float offset[4];
    Vector4 nearest;
    float distance;
    float level;
    float angle;
    int volume;
    int pan;
    int i;
    s8 voice;

    for (i = 0; i < 0x80; i++, D_00446888 = 0) {
        if (D_003EF710[i].unk3A >= 0 && D_003EF710[i].unk3C != 0x63) {
            func_0019E110(i, &nearest);
            distance = func_0019C640(diff, &nearest.x, &D_00413F80.position.x);
            if (D_003EF710[i].volume < distance) {
                if (D_003EF710[i].unk3B > 0) {
                    func_001CE8B0(D_003EF710[i].unk3B);
                    D_00500158[D_003EF710[i].unk3B - 0x54] = D_003EF710[i].unk3B = -1;
                }
            } else {
                if (D_004470E6 == 0) {
                    switch ((u8) D_003EF710[i].unk39) {
                    case 0:
                    case 6:
                        volume = (D_003EF710[i].volume - distance) * 64.0f * (1.0f / D_003EF710[i].volume);
                        break;
                    case 1:
                    case 7:
                        level = D_003EF710[i].volume - distance;
                        level = D_003EF710[i].volume * D_003EF710[i].volume - level * level;
                        volume = (int)((1.0 - sqrt(level) / D_003EF710[i].volume) * 64.0) * 1.5f;
                        break;
                    case 2:
                    case 8:
                        level = D_003EF710[i].volume * D_003EF710[i].volume - distance * distance;
                        volume = sqrt(level) / D_003EF710[i].volume * 64.0;
                        break;
                    }
                    if (volume <= 0) {
                        if (D_003EF710[i].unk3B > 0) {
                            func_001CE8B0(D_003EF710[i].unk3B);
                            D_00500158[D_003EF710[i].unk3B - 0x54] = D_003EF710[i].unk3B = -1;
                        }
                        continue;
                    } else {
                        volume = (float) volume * (float) D_00349680[2] * 0.015625f;
                        if (volume >= 0x80) {
                            volume = 0x7F;
                        }
                    }
                } else {
                    volume = 0;
                }
                angle = atan2f(nearest.x - D_00413F80.position.x, nearest.z - D_00413F80.position.z) -
                        D_00413F80.rotation.y;
                angle -= (int)(angle / 6.2831855f) * 6.2831855f;
                if (3.1415927f < angle) {
                    angle -= 6.2831855f;
                } else if (angle < -3.1415927f) {
                    angle += 6.2831855f;
                }
                func_0019C598(angle, origin, offset);
                pan = (offset[0] * 0.6f + 1.0f) * 63.0f;
                if ((u8) D_003EF710[i].unk39 < 6 || (u8) D_003EF710[i].unk39 > 8) {
                    if (D_00446894 != 0) {
                        D_00446888 -= rand() % 60;
                    } else {
                        D_00446888 += rand() % 60;
                    }
                    if (D_0044688C < D_00446888) {
                        D_00446890 = -(rand() % 0x555 + 0x555);
                        D_00446894 = 1;
                    } else if (D_00446888 < D_00446890) {
                        D_0044688C = rand() % 0x555 + 0x555;
                        D_00446894 = 0;
                    }
                    func_001CE9A0(2, D_003EF710[i].unk3B, D_00446888);
                }
                if (D_003EF710[i].unk3B < 0) {
                    voice = D_003EF710[i].unk3B = func_0019C700();
                    func_001CE690(voice, D_003EF648[4], 0, D_003EF710[i].unk38, 0x3C, volume, D_00446888, (char) pan);
                    continue;
                }
                func_001CE9A0(0, D_003EF710[i].unk3B, volume);
                func_001CE9A0(1, D_003EF710[i].unk3B, pan);
            }
        }
    }
}
