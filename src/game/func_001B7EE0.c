#include "kf_player.h"
#include "kf_game.h"
#include "kf_map.h"
#include "kf_entity.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;      /* the player as of the frame before */
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern int D_00446770;
extern f32 D_00431AF0[16];
extern f32 D_00447124;

/* the movement tables, one row per way of moving (see movement_row) */
extern f32 D_00413DE0[6];           /* top speed */
extern f32 D_00413DF8[6];           /* speed gained per frame */
extern f32 D_00413E10[6];           /* speed lost per frame */
extern f32 D_00413E28[6];           /* top turning speed */
extern f32 D_00413E40[6];           /* turning speed gained per frame */
extern f32 D_00413E70[6];           /* top speed of looking up and down */
extern f32 D_00413E88[6];           /* looking speed gained per frame */
extern f32 D_00413EA0[6];           /* turning and looking speed lost per frame */
extern f32 D_00413EB8[6];           /* how fast the view levels out */
extern f32 D_00413ED0[6];           /* falling speed gained per frame */

/* the pad bits of each control */
extern u16 D_0044692C;              /* forward */
extern u16 D_0044692E;              /* back */
extern u16 D_00446930;              /* turn right */
extern u16 D_00446932;              /* turn left */
extern u16 D_00446934;              /* step right */
extern u16 D_00446936;              /* step left */
extern u16 D_00446938;              /* look down */
extern u16 D_0044693A;              /* look up */

void *memset(void *, int, u32);
float sqrtf(float);
float sinf(float);
float cosf(float);
float atan2f(float, float);
double sin(double);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
float func_0019B838(float);
int func_0019B908(float *, float, float);
int func_0019D0F0(int);
int func_0019D158(int);
void func_001B92C0(float);
void func_001B9610(void);
int func_001BDDC0(void);
void func_001BE760(void);
void func_001BEA50(void);
void func_001BECA8(int);
void func_001BF350(int);
void func_00216778(void *, void *);
void func_00216BC0(void *, void *, void *);
void func_00216C48(void *, void *, void *);
void func_00216D20(void *, void *);

#define PLAYER_FLAG_32 0x100000000L /* the view is levelling out */
#define PLAYER_FLAG_37 0x2000000000L /* the view dips after a landing */

#define STEP_LENGTH 25.0f

/* Moves the player for one frame: the pad bits in `buttons` turn, look and walk; then gravity,
   collision in steps of 25 units, landing, and the push of the floor. */
void func_001B7EE0(u16 buttons) {
    Vector4 delta;
    Vector4 turn;
    Vector4 step;
    f32 matrix[16];
    f32 unused[16];
    MapObject *platform;
    MapRecord *record;
    Vector4 *velocity;
    s32 row;
    s32 steps;
    f32 top_speed;
    f32 speed_gain;
    f32 speed_loss;
    f32 top_turn;
    f32 turn_gain;
    f32 top_look;
    f32 look_gain;
    f32 turn_loss;
    f32 level_step;
    f32 gravity;
    f32 ratio;
    f32 angle;
    f32 distance;
    f32 speed;
    f32 backward;

    memset(&delta, 0, sizeof(delta));
    memset(&turn, 0, sizeof(turn));
    memset(&step, 0, sizeof(step));

    platform = (MapObject *)D_00414280.unk208;
    if (platform != NULL && (platform->unk10 & 0x10000) && (MAP_OBJECT_DEF_FLAGS(platform->unk04) & MAP_OBJECT_DEF_CARRIES)) {
        KF_Vector_Subtract(&turn, &platform->model->rotation, &platform->unk30);
        turn.x = func_0019B838(turn.x);
        turn.y = func_0019B838(turn.y);
        turn.z = func_0019B838(turn.z);
        sceVu0RotMatrixZ(matrix, D_00431AF0, turn.z);
        sceVu0RotMatrixX(matrix, matrix, turn.x);
        sceVu0RotMatrixY(matrix, matrix, turn.y);
        KF_Vector_Subtract(&delta, &D_00413F80, &platform->unk20);
        func_00216BC0(&delta, matrix, &delta);
        func_00216C48(&D_00413F80, &platform->model->position, &delta);
        D_00413F80.rotation.y += platform->model->rotation.y - platform->unk34;
    }
    func_00216C48(&D_00413F80, &D_00413F80, &D_00413F80.unk240);

    if (D_00414280.unk1E0 != 0 && D_00414280.unk1E4 == 4) {
        row = 4;
    } else if ((s32)(D_00414280.unk1C8 >> 39) & 1) {
        row = 3;
    } else if ((s32)(D_00414280.unk1C8 >> 38) & 1) {
        row = 2;
    } else {
        row = D_00414280.unk210 == 3;
    }
    top_speed = D_00413DE0[row];
    speed_gain = D_00413DF8[row];
    speed_loss = D_00413E10[row];
    top_turn = D_00413E28[row];
    turn_gain = D_00413E40[row];
    top_look = D_00413E70[row];
    look_gain = D_00413E88[row];
    turn_loss = D_00413EA0[row];
    level_step = D_00413EB8[row];
    gravity = D_00413ED0[row];

    if (D_00413F80.unkA4 > D_00413F80.unkA8) {
        ratio = (f32)(D_00413F80.unkA4 - D_00413F80.unkA8) / (f32)D_00413F80.unkA8;
        if (ratio < 1.0f) {
            top_speed -= top_speed * ratio;
            top_turn -= top_turn * ratio;
            top_look -= top_look * ratio;
        } else {
            top_look = top_turn = top_speed = 0.0f;
        }
    }
    if ((u16)D_00413F80.timed_flags[4] != 0 || D_00413F80.unk108 != 0 || D_00413F80.unk200 > 0) {
        top_speed *= 0.5f;
        top_turn *= 0.5f;
        top_look *= 0.5f;
        level_step *= 0.5f;
    }
    if ((s32)(D_00413F80.unk1C8 >> 35) & 1) {
        top_speed *= 1.6f;
    }
    if ((s32)(D_00413F80.unk1C8 >> 36) & 1) {
        buttons &= D_00446938 | D_0044693A;
    }

    if (D_00413F80.aim_speed > 0.0f) {
        KF_Vector_Scale((f32 *)&delta, (f32 *)&D_00413F80.aim, D_00413F80.aim_speed);
        func_00216C48(&D_00413F80, &D_00413F80, &delta);
        D_00413F80.aim_speed -= ((f32)D_00413F80.unkA4 + 600.0f) / 600.0f * 20.0f;
        if (D_00413F80.aim_speed < 0.0) {
            D_00413F80.aim_speed = 0.0f;
        }
    }

    if (D_00413F80.unk234 != 0) {
        D_00413F80.unk234--;
        D_00413F80.unk238 += 0.034906585f;
        D_00413F80.unk23C += 0.0017453292f;
        if (D_00413F80.unk238 > 1.012291f) {
            D_00413F80.unk234 = 0;
        }
    } else {
        if (D_00413F80.unk238 > 0.0f) {
            D_00413F80.unk238 -= 0.008726646f;
            if (D_00413F80.unk238 < 0.0f) {
                D_00413F80.unk238 = 0.0f;
            }
        }
        if (D_00413F80.unk23C > 0.0f) {
            D_00413F80.unk23C -= 0.0008726646f;
            if (D_00413F80.unk23C < 0.0f) {
                D_00413F80.unk23C = 0.0f;
            }
        }
    }

    /* looking up and down */
    if (!((s32)(D_00413F80.unk1C8 >> 32) & 1)) {
        if (D_00446938 & buttons) {
            if (D_0044693A & buttons) {
                D_00413F80.unk1B8 = 0.0f;
                D_00413F80.unk1C8 |= PLAYER_FLAG_32;
            } else {
                D_00413F80.unk1B8 += look_gain;
                if (D_00413F80.unk1B8 > top_look) {
                    D_00413F80.unk1B8 = top_look;
                }
            }
        } else if (D_0044693A & buttons) {
            D_00413F80.unk1B8 -= look_gain;
            if (D_00413F80.unk1B8 < -top_look) {
                D_00413F80.unk1B8 = -top_look;
            }
        } else {
            if (D_00413F80.unk1B8 > 0.0f) {
                D_00413F80.unk1B8 -= turn_loss;
                if (D_00413F80.unk1B8 < 0.0f) {
                    D_00413F80.unk1B8 = 0.0f;
                }
            }
            if (D_00413F80.unk1B8 < 0.0f) {
                D_00413F80.unk1B8 += turn_loss;
                if (D_00413F80.unk1B8 > 0.0f) {
                    D_00413F80.unk1B8 = 0.0f;
                }
            }
        }
    } else if ((D_00446938 & buttons) && (D_0044693A & buttons)) {
        func_0019B908(&D_00413F80.rotation.x, 0.0f, level_step);
    } else {
        D_00413F80.unk1C8 &= ~PLAYER_FLAG_32;
    }

    /* turning */
    if ((D_00446932 & buttons) && !(D_00446930 & buttons)) {
        D_00413F80.unk1BC -= turn_gain;
        if (D_00413F80.unk1BC < -top_turn) {
            D_00413F80.unk1BC = -top_turn;
        }
    } else if (!(D_00446932 & buttons) && (D_00446930 & buttons)) {
        D_00413F80.unk1BC += turn_gain;
        if (D_00413F80.unk1BC > top_turn) {
            D_00413F80.unk1BC = top_turn;
        }
    } else {
        if (D_00413F80.unk1BC > 0.0f) {
            D_00413F80.unk1BC -= turn_loss;
            if (D_00413F80.unk1B8 < 0.0f) { // match: retail tests the looking speed here, not the turning speed
                D_00413F80.unk1BC = 0.0f;
            }
        }
        if (D_00413F80.unk1BC < 0.0f) {
            D_00413F80.unk1BC += turn_loss;
            if (D_00413F80.unk1BC > 0.0f) {
                D_00413F80.unk1BC = 0.0f;
            }
        }
    }

    /* walking */
    D_00447124 = top_speed;
    if ((D_0044692C & buttons) && !(D_0044692E & buttons)) {
        if (D_00413F80.unk1B4 < top_speed) {
            D_00413F80.unk1B4 += speed_gain;
            if (D_00413F80.unk1B4 > top_speed) {
                D_00413F80.unk1B4 = top_speed;
            }
        } else if (D_00413F80.unk1B4 > top_speed) {
            D_00413F80.unk1B4 -= speed_loss;
            if (D_00413F80.unk1B4 < top_speed) {
                D_00413F80.unk1B4 = top_speed;
            }
        }
    } else if (!(D_0044692C & buttons) && (D_0044692E & buttons)) {
        backward = -top_speed;
        if (D_00413F80.unk1B4 > backward) {
            D_00413F80.unk1B4 -= speed_gain;
            if (D_00413F80.unk1B4 < backward) {
                D_00413F80.unk1B4 = backward;
            }
        } else if (D_00413F80.unk1B4 < backward) {
            D_00413F80.unk1B4 += speed_loss;
            if (D_00413F80.unk1B4 > backward) {
                D_00413F80.unk1B4 = backward;
            }
        }
    } else {
        if (D_00413F80.unk1B4 > 0.0f) {
            D_00413F80.unk1B4 -= speed_loss;
            if (D_00413F80.unk1B4 < 0.0f) {
                D_00413F80.unk1B4 = 0.0f;
            }
        }
        if (D_00413F80.unk1B4 < 0.0f) {
            D_00413F80.unk1B4 += speed_loss;
            if (D_00413F80.unk1B4 > 0.0f) {
                D_00413F80.unk1B4 = 0.0f;
            }
        }
    }

    /* stepping sideways */
    if (!(D_00446936 & buttons) && (D_00446934 & buttons)) {
        if (D_00413F80.unk1B0 < top_speed) {
            D_00413F80.unk1B0 += speed_gain;
            if (D_00413F80.unk1B0 > top_speed) {
                D_00413F80.unk1B0 = top_speed;
            }
        } else if (D_00413F80.unk1B0 > top_speed) {
            D_00413F80.unk1B0 -= speed_loss;
            if (D_00413F80.unk1B0 < top_speed) {
                D_00413F80.unk1B0 = top_speed;
            }
        }
    } else if ((D_00446936 & buttons) && !(D_00446934 & buttons)) {
        backward = -top_speed;
        if (D_00413F80.unk1B0 > backward) {
            D_00413F80.unk1B0 -= speed_gain;
            if (D_00413F80.unk1B0 < backward) {
                D_00413F80.unk1B0 = backward;
            }
        } else if (D_00413F80.unk1B0 < backward) {
            D_00413F80.unk1B0 += speed_loss;
            if (D_00413F80.unk1B0 > backward) {
                D_00413F80.unk1B0 = backward;
            }
        }
    } else {
        if (D_00413F80.unk1B0 > 0.0f) {
            D_00413F80.unk1B0 -= speed_loss;
            if (D_00413F80.unk1B0 < 0.0f) {
                D_00413F80.unk1B0 = 0.0f;
            }
        }
        if (D_00413F80.unk1B0 < 0.0f) {
            D_00413F80.unk1B0 += speed_loss;
            if (D_00413F80.unk1B0 > 0.0f) {
                D_00413F80.unk1B0 = 0.0f;
            }
        }
    }

    /* falling */
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1) && !((s32)(D_00413F80.unk1C8 >> 36) & 1)) {
        if ((s32)(D_00413F80.unk1C8 >> 33) & 1) {
            D_00413F80.unk1AC = 0.0f;
        } else if (D_00413F80.unk1A8 == 0) {
            D_00413F80.unk1AC += gravity;
        }
    } else {
        D_00413F80.unk1AC = 0.0f;
    }

    delta.y = D_00413F80.unk1BC;
    delta.x = D_00413F80.unk1B8;
    if ((u16)D_00413F80.timed_flags[4] != 0) {
        delta.x *= 0.5f;
        delta.y *= 0.5f;
    }
    D_00413F80.rotation.y = func_0019B838(D_00413F80.rotation.y + delta.y);
    D_00413F80.rotation.x += delta.x;
    if (D_00413F80.rotation.x > 1.012291f) {
        D_00413F80.rotation.x = 1.012291f;
    }
    if (D_00413F80.rotation.x < -1.012291f) {
        D_00413F80.rotation.x = -1.012291f;
    }

    delta.z = D_00413F80.unk1B4;
    delta.x = D_00413F80.unk1B0;
    delta.y = delta.w = 0.0f;
    angle = atan2f(ABS(D_00413F80.unk1B4), ABS(D_00413F80.unk1B0));
    delta.x *= cosf(angle);
    delta.z *= sinf(angle);
    sceVu0RotMatrixY(matrix, D_00431AF0, D_00413F80.rotation.y);
    func_00216BC0(&delta, matrix, &delta);
    D_00413F80.position.x += delta.x;
    D_00413F80.position.z += delta.z;

    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (D_00414280.unk208 == 0) {
            KF_Vector_Subtract(&delta, &D_00413F80, &D_00414280);
            distance = sqrtf(delta.x * delta.x + delta.z * delta.z);
            if (distance > STEP_LENGTH) {
                steps = (s32)(distance / STEP_LENGTH);
                delta.y = 0.0f;
                func_00216D20(&delta, &delta);
                KF_Vector_Scale((f32 *)&step, (f32 *)&delta, STEP_LENGTH);
                KF_Vector_Scale((f32 *)&delta, (f32 *)&delta, distance - (f32)steps * STEP_LENGTH);
                func_00216778(&D_00413F80, &D_00414280);
                do {
                    func_00216C48(&D_00413F80, &D_00413F80, &step);
                    func_001BECA8(0);
                    func_001BF350(0);
                    func_001BE760();
                } while (--steps != 0);
                func_00216C48(&D_00413F80, &D_00413F80, &delta);
            }
        }
        func_001BECA8(1);
        if (!((s32)(D_00413F80.unk1C8 >> 36) & 1)) {
            func_001BDDC0();
            func_001BF350(1);
            if (D_00413F80.unk210 == 6 && D_00413F80.unk208 != 0 && ((MapObject *)D_00413F80.unk208)->unk04->kind == 12) {
                func_001BEA50();
            }
            if (!((s32)(D_00413F80.unk1C8 >> 34) & 1)) {
                if (((s32)(D_00414280.unk1C8 >> 34) & 1) && D_00413F80.unk1A8 == 0) {
                    if (D_00413F80.position.y - D_00413F80.unk1D8 > 110.0f) {
                        if (!((s32)(D_00413F80.unk1C8 >> 38) & 1)) {
                            func_0019D0F0(0x22);
                        } else if (!((s32)(D_00414280.unk1C8 >> 38) & 1)) {
                            func_0019D158(0x38);
                        }
                    }
                    if (D_00414280.unk1AC >= 66.666664f) {
                        func_001B92C0(D_00414280.unk1AC);
                        if (!((s32)(D_00413F80.unk1C8 >> 38) & 1) && D_00413F80.unk1E4 != 4) {
                            D_00413F80.unk1C8 |= PLAYER_FLAG_37;
                            D_00413F80.unk1F4 = D_00414280.unk1AC * 0.5f;
                            if (D_00413F80.unk1F4 > 150.0f) {
                                D_00413F80.unk1F4 = 150.0f;
                            }
                            D_00413F80.unk1F0 = 0.0f;
                            D_00413F80.unk1FC = 0;
                            D_00413F80.unk1F8 = (s32)(D_00413F80.unk1F4 * 40.0f / 150.0f);
                        }
                    }
                }
            }
            if (((s32)(D_00413F80.unk1C8 >> 34) & 1) && !((s32)(D_00414280.unk1C8 >> 34) & 1)) {
                D_00413F80.unk1D8 = D_00414280.position.y;
            }
        }
        if (D_00413F80.map_index == 0xFFFF) {
            D_00413F80.map_index = 0;
            D_00413F80.unk250 = 0;
        }
    }

    if ((s32)(D_00413F80.unk1C8 >> 37) & 1) {
        D_00413F80.unk1F0 = (f32)sin((f32)D_00413F80.unk1FC * 3.1415927f / (f32)D_00413F80.unk1F8) * D_00413F80.unk1F4;
        if (D_00413F80.unk1FC++ >= D_00413F80.unk1F8) {
            D_00413F80.unk1C8 &= ~PLAYER_FLAG_37;
            D_00413F80.unk1F0 = 0.0f;
            D_00413F80.unk1F4 = 0.0f;
            D_00413F80.unk1FC = 0;
            D_00413F80.unk1F8 = 0;
        }
    }

    /* the push of the floor */
    if (D_0038CC50.unk00 != 0 && !(D_0038CC50.unk30 & 1)) {
        if (D_00413F80.map_index != 0xFFFF && D_00413F80.map_index < D_0038CD20[D_00446770].header->record_count) {
            record = &((MapRecord *)D_0038CD20[D_00446770].records)[D_00413F80.map_index];
        } else {
            record = NULL;
        }
        speed = sqrtf(D_00413F80.unk240.x * D_00413F80.unk240.x + D_00413F80.unk240.y * D_00413F80.unk240.y
                      + D_00413F80.unk240.z * D_00413F80.unk240.z);
        if (record != NULL && record->unk2E != 0 && record->unk2C != 0) {
            velocity = &D_00413F80.unk240;
            if (speed < (f32)record->unk2C) {
                delta.x = sinf(record->unk1C) * (f32)record->unk2E;
                delta.y = 0.0f;
                delta.z = cosf(record->unk1C) * (f32)record->unk2E;
                delta.w = 0.0f;
                func_00216C48(velocity, velocity, &delta);
                speed = sqrtf(D_00413F80.unk240.x * D_00413F80.unk240.x + D_00413F80.unk240.y * D_00413F80.unk240.y
                              + D_00413F80.unk240.z * D_00413F80.unk240.z);
                if (speed > (f32)record->unk2C) {
                    func_00216D20(velocity, velocity);
                    KF_Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->unk2C);
                }
            } else {
                func_00216D20(&delta, velocity);
                KF_Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
                func_00216C48(velocity, velocity, &delta);
                speed = sqrtf(D_00413F80.unk240.x * D_00413F80.unk240.x + D_00413F80.unk240.y * D_00413F80.unk240.y
                              + D_00413F80.unk240.z * D_00413F80.unk240.z);
                if (speed > (f32)record->unk2C) {
                    func_00216D20(velocity, velocity);
                    KF_Vector_Scale((f32 *)velocity, (f32 *)velocity, (f32)record->unk2C);
                }
            }
        } else if (speed > speed_loss) {
            func_00216D20(&delta, &D_00413F80.unk240);
            KF_Vector_Scale((f32 *)&delta, (f32 *)&delta, -speed_loss);
            func_00216C48(&D_00413F80.unk240, &D_00413F80.unk240, &delta);
        } else {
            D_00413F80.unk240.x = D_00413F80.unk240.y = D_00413F80.unk240.z = 0.0f;
        }
    }

    if (D_00413F80.unk1A8 == 0 && D_00413F80.unk1AC > 1000.0f) {
        D_00413F80.unkB4 = 0;
        D_00413F80.unk1A4 = 0;
        func_001B9610();
    }
}
