#include "common.h"
#include "kf_effect.h"
#include "kf_file.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern EffectDef D_00502260[];
extern Vector4 D_00574260[8];
extern char D_0043EEF0[];
extern char D_0043EF00[];
extern char D_0043EF10[];
extern int D_004468CC;
extern int D_004468D0;
extern char *D_00446F5C;
extern u32 *D_004470F8;
extern char *D_004470FC;
extern char *D_00447100;
extern char **D_00447104;

void *memset(void *, int, u32);
char *func_0020DAA8(char *base, int chunk_arg);
void func_0020D9A8(void);
void func_001A1868(void);

/* Clears the effect pools, lays out the effect buffers, loads the three effect files and
   turns the angles of the effect definitions from degrees into radians. */
void func_001A1610(void) {
    char unused_buffer[0x2C0];
    char *buffer;
    int handle;
    char **table;
    int i;
    int key;
    EffectDef *def;
    EffectDef *next_def;
    Vector4 *keys;
    f32 scale; /* degrees to radians */

    D_004468D0 = 0;
    D_004468CC = 0;
    memset(D_003F3550, 0, 0xF000);
    memset(D_003F1750, 0, 0x1E00);
    buffer = D_00446F5C;
    D_004470F8 = (u32 *)buffer;
    buffer += 0x70400;
    D_00447100 = buffer;
    buffer += 0xC0000;
    D_004470FC = buffer;
    buffer += 0x9FC00;
    D_00447104 = (char **)D_004470FC;
    memset(D_00574260, 0, 0x80);
    D_00574260[7].z = 1.0f;
    D_00574260[7].y = 1.0f;
    D_00574260[7].x = 1.0f;
    D_00574260[7].w = 1.0f;
    handle = func_00102238(D_0043EEF0, 0, 1);
    func_001025C0(handle, D_00502260, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043EF00, 0, 1);
    func_001025C0(handle, buffer, -1);
    func_001024B0(handle);
    func_0020DAA8(buffer, 1);
    func_0020D9A8();
    handle = func_00102238(D_0043EF10, 0, 1);
    func_001025C0(handle, D_004470FC, -1);
    func_001024B0(handle);
    table = D_00447104;
    for (i = 0; i < 0x200; i++) {
        table[i] += (int)table; /* file offsets become pointers */
    }
    func_001A1868();
    scale = 0.017453292f;
    def = D_00502260;
    for (i = 0; i < 0x400; i++, def = next_def) {
        next_def = def + 1; // match: the next entry is worked out first
        keys = def->keys_a;
        for (key = 0; key < 4; key++, keys++) {
            keys[4].x *= scale; // match: keys_b, reached from keys_a so that one pointer walks it
            keys[4].y *= scale;
            keys[4].z *= scale;
        }
        def->rotation_base[0] *= scale;
        def->rotation_base[1] *= scale;
        def->rotation_base[2] *= scale;
        def->rotation_spread[0] *= scale;
        def->rotation_spread[1] *= scale;
        def->rotation_spread[2] *= scale;
        if (def->unkFA != 0) {
            def->unkFA += 0x1400;
        }
    }
}
