#include "gfx/scroll_texture.h"
#include "gfx/gs.h"
#include "gfx/texture.h"
#include "core/file.h"
#include "gfx/gs_draw.h"
#include "gfx/packet.h"

static ScrollTexture scroll_textures[4]; /* 0x474AC0 */
static ScrollOffset scroll_offsets[4];   /* 0x474B60 */

/* Sets up the four scrolling textures (the water and the like) and rewinds them. */
void ScrollTexture_Init(void) {
    ScrollTexture *texture = scroll_textures;

    memset(scroll_offsets, 0, sizeof(scroll_offsets));
    texture->width = 0x80;
    texture->height = 0x80;
    texture->psm = 0x13;
    texture->tbp = 0x39C0;
    texture->cpsm = 0;
    texture->cbp = 0x3EA8;
    texture->repeat_x = 1.0f;
    texture->repeat_y = 1.0f;
    texture->target_width = 0x80;
    texture->target_height = 0x80;
    texture->target_psm = 0;
    texture->target_fbp = 0x3A40;
    texture->rgba[0] = 0x80;
    texture->rgba[1] = 0x80;
    texture->rgba[2] = 0x80;
    texture->rgba[3] = 0x80;
    texture->alpha_a = 2;
    texture->alpha_b = 2;
    texture->alpha_c = 0;
    texture->alpha_d = 0;
    texture->alpha_fix = 0;
    texture->period = 1;
    texture->step_x = -2;
    texture->step_y = -2;
    texture++;
    texture->width = 0x80;
    texture->height = 0x80;
    texture->psm = 0x13;
    texture->tbp = 0x3940;
    texture->cpsm = 0;
    texture->cbp = 0x3EA4;
    texture->repeat_x = 2.0f;
    texture->repeat_y = 2.0f;
    texture->target_width = 0x80;
    texture->target_height = 0x80;
    texture->target_psm = 0;
    texture->target_fbp = 0x3A40;
    texture->rgba[0] = 0x80;
    texture->rgba[1] = 0x80;
    texture->rgba[2] = 0x80;
    texture->rgba[3] = 0x80;
    texture->alpha_a = 0;
    texture->alpha_b = 2;
    texture->alpha_c = 0;
    texture->alpha_d = 1;
    texture->alpha_fix = 0;
    texture->period = 1;
    texture->step_x = -2;
    texture->step_y = -2;
    texture++;
    texture->width = 0x80;
    texture->height = 0x80;
    texture->psm = 0x13;
    texture->tbp = 0x3980;
    texture->cpsm = 0;
    texture->cbp = 0x3EA0;
    texture->repeat_x = 1.0f;
    texture->repeat_y = 1.0f;
    texture->target_width = 0x80;
    texture->target_height = 0x80;
    texture->target_psm = 0;
    texture->target_fbp = 0x3A40;
    texture->rgba[0] = 0x80;
    texture->rgba[1] = 0x80;
    texture->rgba[2] = 0x80;
    texture->rgba[3] = 0x80;
    texture->alpha_a = 2;
    texture->alpha_b = 0;
    texture->alpha_c = 0;
    texture->alpha_d = 1;
    texture->alpha_fix = 0;
    texture->period = 1;
    texture->step_x = 0;
    texture->step_y = -4;
    texture++;
    texture->width = 0x80;
    texture->height = 0x80;
    texture->psm = 0x13;
    texture->tbp = 0x3980;
    texture->cpsm = 0;
    texture->cbp = 0x3EA0;
    texture->repeat_x = 1.0f;
    texture->repeat_y = 1.0f;
    texture->target_width = 0x80;
    texture->target_height = 0x80;
    texture->target_psm = 0;
    texture->target_fbp = 0x3A40;
    texture->rgba[0] = 0x80;
    texture->rgba[1] = 0x80;
    texture->rgba[2] = 0x80;
    texture->rgba[3] = 0x80;
    texture->alpha_a = 0;
    texture->alpha_b = 2;
    texture->alpha_c = 0;
    texture->alpha_d = 1;
    texture->alpha_fix = 0;
    texture->period = 1;
    texture->step_x = 2;
    texture->step_y = 2;
}

/* Scrolls and redraws the four textures, then restores the GS state (a map object's draw hook). */
void ScrollTexture_DrawAll(ModelInstance *model) {
    ScrollTexture *texture = scroll_textures;
    ScrollOffset *offset = scroll_offsets;

    GsDraw_DrawScrollingTexture(texture++, offset++);
    GsDraw_DrawScrollingTexture(texture++, offset++);
    GsDraw_DrawScrollingTexture(texture++, offset++);
    GsDraw_DrawScrollingTexture(texture++, offset++);
    GsDraw_QueueTexFlush();
    GsDraw_ResetDrawTarget();
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ALPHA_1, 0x44);
    Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
    Packet_EndGsRegisters();
}
