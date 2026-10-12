#include "kf_model.h"

extern f32 D_0042C6F0[];

void func_0019B1E8(s16, Vector4 *, f32);

/* Update handler of the light kind: the flickering sorts (0 faintly, 1 strongly)
   step their two periods and scale the colour by the product of the two waves
   read from the table D_0042C6F0 (0xA00 entries a period). */
void func_00120BB8(MapObject *object) {
    s16 *work = (s16 *)object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    Vector4 colour;
    f32 level;

    switch (source->unk50) {
        case 0: {
            // match: the depth and base are locals of each case
            f32 depth = 0.1f;
            f32 base = 0.9f;

            work[1] = (work[1] + 1) % work[2];
            work[3] = (work[3] + 1) % work[4];
            level = D_0042C6F0[work[1] * 0xA00 / work[2]] * D_0042C6F0[work[3] * 0xA00 / work[4]];
            depth *= level;
            level = depth + base;
            break;
        }
        case 1: {
            f32 depth = 0.25f;
            f32 base = 0.75f;

            work[1] = (work[1] + 1) % work[2];
            work[3] = (work[3] + 1) % work[4];
            level = D_0042C6F0[work[1] * 0xA00 / work[2]] * D_0042C6F0[work[3] * 0xA00 / work[4]];
            depth *= level;
            level = depth + base;
            break;
        }
        case 2: // match: the steady sort is named, which decides the order of the tests
        default:
            level = 1.0f;
            break;
    }
    colour.x = level * source->unk44;
    colour.y = level * source->unk46;
    colour.z = level * source->unk48;
    colour.w = 1.0f;
    func_0019B1E8(work[0], &colour, source->unk4A);
}
