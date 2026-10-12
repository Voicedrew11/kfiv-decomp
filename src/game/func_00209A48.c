#include "kf_splash.h"

extern BootTexture D_005FAB60[13];
extern BootSpritePacket D_005F9B60;
extern u64 D_0042C1C0[4];
extern u32 *D_004473BC;

int func_00209868(int);
int func_00209CD8(u32, u32);

/* Draws a sprite of the boot screens: sends its texture, then a sprite packet (BootSpritePacket) written through
   the cursor D_004473BC. Its brightness is the track's value at the sprite's frame, times scale. */
int func_00209A48(float scale, void *sprite_arg, int blend) {
    BootSprite *sprite = sprite_arg;
    int alpha;
    int next;
    int i;

    func_00209868(sprite->texture);
    for (i = 3; i >= 0; i--) {
        if (sprite->frame >= sprite->track->key_frames[i]) {
            // match: at the last key, key_frames[next] and key_values[next] are past their arrays (the ratio is 0 there)
            next = i + 1;
            alpha = sprite->track->key_values[next - 1] +
                    (sprite->track->key_values[next] - sprite->track->key_values[next - 1]) *
                        ((f32)(sprite->frame - sprite->track->key_frames[i]) / (f32)(sprite->track->key_frames[next] - sprite->track->key_frames[i]));
            break;
        }
    }
    alpha *= scale;

    D_004473BC = (u32 *)&D_005F9B60;
    *D_004473BC++ = 0x8001;
    *D_004473BC++ = 0x20000000;
    *D_004473BC++ = 0xEE;
    *D_004473BC++ = 0;
    *((u64 *)D_004473BC)++ = D_0042C1C0[blend];
    *((u64 *)D_004473BC)++ = 0x42;
    *((u64 *)D_004473BC)++ = 0x3000D;
    *((u64 *)D_004473BC)++ = 0x47;
    *D_004473BC++ = 0x8001;
    *D_004473BC++ = 0x84000000;
    *D_004473BC++ = 0xF5252160;
    *D_004473BC++ = 0;
    *D_004473BC++ = 0x56;
    *D_004473BC++ = 0;
    *((BootTex0 *)D_004473BC)++ = D_005FAB60[sprite->texture].tex0;
    *D_004473BC++ = (alpha << 24) + 0x808080;
    *((f32 *)D_004473BC)++ = 1.0f;
    *((f32 *)D_004473BC)++ = 0.0f;
    *((f32 *)D_004473BC)++ = 0.0f;
    *D_004473BC++ = ((sprite->pos_x + 0x800) << 4) + ((sprite->pos_y / 2 + 0x800) << 20);
    *D_004473BC++ = 0;
    *((f32 *)D_004473BC)++ = 1.0f;
    *((f32 *)D_004473BC)++ = 1.0f;
    *D_004473BC++ = ((sprite->pos_x + sprite->width + 0x800) << 4) + (((sprite->pos_y + sprite->height) / 2 + 0x800) << 20);
    *D_004473BC++ = 0xFFFF;
    *D_004473BC++ = 0;
    *D_004473BC++ = 0;
    func_00209CD8((u32)&D_005F9B60, (u32)(D_004473BC - (u32 *)&D_005F9B60) / 4);
    sprite->frame++;
    if (sprite->track->length < sprite->frame) {
        sprite->frame = 0;
    }
    return 0;
}
