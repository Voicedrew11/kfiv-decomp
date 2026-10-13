#include "kf_scroll_texture.h"
#include "kf_gs.h"
#include "kf_texture.h"

void *memset(void *, int, u32);
void func_0010A880(void *, void *);
void func_0010A008(void);
void func_00109E88(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);

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
void ScrollTexture_DrawAll(void) {
    ScrollTexture *texture = scroll_textures;
    ScrollOffset *offset = scroll_offsets;

    func_0010A880(texture++, offset++);
    func_0010A880(texture++, offset++);
    func_0010A880(texture++, offset++);
    func_0010A880(texture++, offset++);
    func_00109E88();
    func_0010A008();
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x5000D);
    func_00109AD0();
}
