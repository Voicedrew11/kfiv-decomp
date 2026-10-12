#include "kf_mc.h"

extern s32 D_00440A68[4][4];
extern f32 D_00440AA8[3][4];
extern f32 D_00440AD8[3][4];
extern f32 D_00440B08[4];
extern char D_00446D30[] __attribute__((aligned(4))); // match: the alignment lets the 5-byte copy use word and byte stores
extern char D_00440B18[];
extern char D_00440940[];

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);

/* Fills the memory card icon header with the save title, icon file names and lighting. */
void func_001FB2A0(s32 slot) {
    s32 bg_color[4][4];
    f32 light_dir[3][4];
    f32 light_color[3][4];
    f32 ambient[4];

    memcpy(bg_color, D_00440A68, sizeof(bg_color));
    memcpy(light_dir, D_00440AA8, sizeof(light_dir));
    memcpy(light_color, D_00440AD8, sizeof(light_color));
    memcpy(ambient, D_00440B08, sizeof(ambient));
    memset(D_00447338, 0, 0x3C4);
    memcpy(D_00447338->head, D_00446D30, 5);
    memcpy(D_00447338->title, D_00440B18, 59);
    memcpy(D_00447338->view_name, D_00440940, 12);
    memcpy(D_00447338->copy_name, D_00440940, 12);
    memcpy(D_00447338->delete_name, D_00440940, 12);
    D_00447338->transparency = 0x40;
    memcpy(D_00447338->bg_color, bg_color, sizeof(bg_color));
    memcpy(D_00447338->light_dir, light_dir, sizeof(light_dir));
    memcpy(D_00447338->light_color, light_color, sizeof(light_color));
    memcpy(D_00447338->ambient, ambient, sizeof(ambient));
    D_00447338->line_break = 0x20;
}
