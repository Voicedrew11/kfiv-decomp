#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern f32 D_00414740[4];
extern PlayerArmPoints D_00414780;
extern PlayerArmPoints D_004147A0;
extern MapSlotUnk10Half D_004145E0[2];
extern MapSlotUnk10Half D_0038A270;
extern CameraState D_004325A0;
extern f32 D_00431AF0[16];
extern ModelInstance D_0057B6A0;
extern ModelInstance D_0057B860;

void *memset(void *, int, u32);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);

/* Each frame: places the held item's model in front of the camera (turned with the player's view),
   poses its parts, copies the pose to the arm's model and notes two of its points. */
void func_001BFCC8(void) {
    f32 matrix[16];
    f32 position[4];
    f32 unused[4];
    f32 pitch;
    ModelPart *part;
    ModelPart *arm_part;
    s32 count;
    s32 i;

    D_004147A0 = D_00414780;
    memset(&D_00414780, 0, 0x20);
    if (D_00413F80.unk258 != 0) {
        sceVu0RotMatrixY(matrix, D_00431AF0, 3.1415927f);
        sceVu0TransMatrix(matrix, matrix, D_00414740);
        pitch = D_00413F80.rotation.x - D_00413F80.unk238;
        if (pitch > 1.0122910f) {
            pitch = 1.0122910f;
        } else if (pitch < -1.0122910f) {
            pitch = -1.0122910f;
        }
        sceVu0RotMatrixX(matrix, matrix, pitch);
        sceVu0RotMatrixY(matrix, matrix, D_00413F80.rotation.y);
        func_00216778(position, &D_00413F80);
        position[1] = position[1] - 220.0f + D_00413F80.unk1C4 + D_00413F80.unk1F0;
        func_00216C48(position, position, D_004325A0.unk50);
        func_00216C48(position, position, D_004325A0.unk60);
        position[1] += D_004325A0.unk4C;
        sceVu0TransMatrix((f32 *)D_0057B6A0.unk40, matrix, position);
        count = D_0057B6A0.part_count;
        part = D_0057B6A0.parts;
        for (i = 0; i < count; i++, part++) {
            func_0020B8B0(&D_0057B6A0, part, part);
        }
        if (D_0057B6A0.unk100 & 1) {
            part = D_0057B6A0.parts;
            for (i = 0; i < count; i++, part++) {
                func_0010B5D0(part);
            }
        }
        func_00216778(&D_0057B6A0.rotation, &D_00413F80.rotation);
        func_00216778(&D_0057B860.rotation, &D_00413F80.rotation);
        *(ModelMatrix *)D_0057B860.unk40 = *(ModelMatrix *)D_0057B6A0.unk40;
        part = D_0057B6A0.parts;
        arm_part = D_0057B860.parts;
        // match: retail takes the larger part count of the two models, not the smaller
        count = D_0057B6A0.part_count > D_0057B860.part_count ? D_0057B6A0.part_count : D_0057B860.part_count;
        for (i = 0; i < count; i++, part++, arm_part++) {
            func_00216778(arm_part->unk40, part->unk40);
            func_00216778(&arm_part->unk50, &part->unk50);
            func_00216778(arm_part->unk60, part->unk60);
            *(ModelMatrix *)arm_part->unk00 = *(ModelMatrix *)part->unk00;
        }
        D_0057B6A0.unk1B4(&D_0057B6A0);
        func_00216778(&D_00414780.points[0], (void *)D_0057B6A0.get_point(&D_0057B6A0, 30));
        func_00216778(&D_00414780.points[1], (void *)D_0057B6A0.get_point(&D_0057B6A0, 31));
    }
    D_004145E0[0] = D_0038A270;
}
