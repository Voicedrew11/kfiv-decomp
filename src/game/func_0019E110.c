#include "kf_player.h"
#include "kf_sound.h"

extern PlayerState D_00413F80;

double sqrt(double);

extern SoundChannel D_003EF710[];

/* Writes where the channel's sound comes from nearest to the player: its point, or the nearest point of its segment. */
int func_0019E110(s32 index, Vector4 *nearest) {
    Vector4Arg start;
    Vector4Arg end;
    char reserved[0x40]; /* unused, but retail reserves the 0x40 bytes */
    f32 seg_x;
    f32 seg_y;
    f32 seg_z;
    f32 to_start_x;
    f32 to_start_y;
    f32 to_start_z;
    f32 to_end_x;
    f32 to_end_y;
    f32 to_end_z;
    f32 back_x;
    f32 back_y;
    f32 back_z;
    f32 dot_start;
    f32 cos_start;
    f32 cos_end;
    f32 along;

    switch (D_003EF710[index].unk3C) {
    case 1:
        nearest->x = D_003EF710[index].origin.x;
        nearest->y = D_003EF710[index].origin.y;
        nearest->z = D_003EF710[index].origin.z;
        break;
    case 2:
        start = D_003EF710[index].origin;
        // match: retail adds the 0x10 to the table base before the index
        end = (&D_003EF710[0].target)[index * 4];
        to_start_x = D_00413F80.position.x - start.x;
        seg_x = end.x - start.x;
        to_start_y = D_00413F80.position.y - start.y;
        seg_y = end.y - start.y;
        back_x = start.x - end.x;
        back_y = start.y - end.y;
        back_z = start.z - end.z;
        to_end_x = D_00413F80.position.x - end.x;
        to_start_z = D_00413F80.position.z - start.z;
        seg_z = end.z - start.z;
        to_end_y = D_00413F80.position.y - end.y;
        to_end_z = D_00413F80.position.z - end.z;
        dot_start = to_start_x * seg_x + to_start_y * seg_y + to_start_z * seg_z;
        cos_start = dot_start / (sqrt(to_start_x * to_start_x + to_start_y * to_start_y + to_start_z * to_start_z) * sqrt(seg_x * seg_x + seg_y * seg_y + seg_z * seg_z));
        cos_end = (to_end_x * back_x + to_end_y * back_y + to_end_z * back_z) / (sqrt(to_end_x * to_end_x + to_end_y * to_end_y + to_end_z * to_end_z) * sqrt(back_x * back_x + back_y * back_y + back_z * back_z));
        if (cos_start > 0.0f && cos_start < 1.0f && cos_end > 0.0f && cos_end < 1.0f) {
            along = dot_start / (seg_x * seg_x + seg_y * seg_y + seg_z * seg_z);
            nearest->x = start.x + seg_x * along;
            nearest->y = start.y + seg_y * along;
            nearest->z = start.z + seg_z * along;
        } else if (cos_start <= 0.0f) {
            nearest->x = start.x;
            nearest->y = start.y;
            nearest->z = start.z;
        } else if (cos_end <= 0.0f) {
            nearest->x = end.x;
            nearest->y = end.y;
            nearest->z = end.z;
        }
        break;
    case 3:
        break;
    }
}
