#include "kf_game.h"
#include "kf_player.h"

extern GameStatus D_0038CC50;
extern PlayerState D_00413F80;
extern EventRunner D_00420D00;
extern CameraState D_004325A0;

void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void Vector_Add(void *, void *, void *);
void Vector_Copy(void *, void *);
int Angle_Approach(float *, float, float);
float atan2f(float, float);
float sqrtf(float);

/* Moves and turns the camera for the current camera mode, then adds the two shake offsets. */
void func_00100FC8(void) {
    f32 unused_matrix[4][4];
    f32 direction[4];
    f32 target[4];
    f32 unused_vector[4];
    f32 yaw;
    f32 pitch;
    f32 length;

    switch (D_0038CC50.unk03) {
    case 0:
        Vector_Scale(target, D_00420D00.unk10, D_00420D00.unk40);
        Vector_Add(target, target, D_00420D00.unk00);
        D_004325A0.position[0] = D_00413F80.position.x;
        D_004325A0.position[2] = D_00413F80.position.z;
        D_004325A0.position[1] =
            D_00413F80.position.y - 220.0f + D_00413F80.unk1C4 + D_00413F80.unk1F0 + D_004325A0.unk4C;
        Vector_Subtract(direction, target, D_004325A0.position);
        D_004325A0.unk10 =
            atan2f(-direction[1], sqrtf(direction[0] * direction[0] + direction[2] * direction[2]));
        D_004325A0.unk14 = atan2f(direction[0], direction[2]);
        D_004325A0.unk18 = 0.0f;
        break;
    case 1:
        if (D_0038CC50.unk05 != 0) {
            Vector_Subtract(direction, D_004325A0.unk30, D_004325A0.position);
            length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
            if (D_004325A0.unk44 < length) {
                Vector_Scale(direction, direction, D_004325A0.unk44 / length);
                Vector_Add(D_004325A0.position, D_004325A0.position, direction);
            } else {
                Vector_Copy(D_004325A0.position, D_004325A0.unk30);
            }
        }
        break;
    case 2:
        break;
    }
    if (D_0038CC50.unk03 != 0 && (s8)D_0038CC50.unk04 != 0) {
        Vector_Subtract(direction, D_004325A0.unk20, D_004325A0.position);
        yaw = atan2f(direction[0], direction[2]);
        length = sqrtf(direction[0] * direction[0] + direction[2] * direction[2]);
        pitch = atan2f(-direction[1], length);
        switch ((s8)D_0038CC50.unk04) {
        case 1:
            D_004325A0.unk10 = pitch;
            D_004325A0.unk14 = yaw;
            break;
        case 2:
            Angle_Approach(&D_004325A0.unk10, pitch, D_004325A0.unk40);
            Angle_Approach(&D_004325A0.unk14, yaw, D_004325A0.unk40);
            break;
        }
    }
    Vector_Add(D_004325A0.position, D_004325A0.position, D_004325A0.unk50);
    Vector_Add(D_004325A0.position, D_004325A0.position, D_004325A0.unk60);
}
