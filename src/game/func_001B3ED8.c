#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern Entity D_003BEDF0[][0x80];
extern Entity D_003D6C70;           /* the last entity of D_003BEDF0 */

/* EntityParams.unk238 as six signed bytes: tenths the six magic strengths of an entity's hit are scaled by */
#define ENTITY_HIT_SCALES(entity) ((s8 *)&(entity)->params->unk238)

/* store a strength as a whole number, rounded up when more than a thousandth is left over */
#define STORE_ROUNDED_UP(dest, value) \
    strength = (value); \
    whole = strength; \
    (dest) = whole; \
    if (strength - whole > 0.001f) { \
        (dest) = whole + 1; \
    }

/* The nine strengths of a hit as they count against the target (func_001B2F70). The player's are
   raised by the weapon bonus (extra->unk20) or the magic bonus (unk22, plus unk0E[] per kind) in
   fiftieths. A hit of kind 0 from anything else counts as it is; otherwise the three physical
   strengths are dropped and the six magic ones scaled, by EntityParams.unk238 for an entity.
   The rows are spelled out as retail has them. */
void func_001B3ED8(EntityParamsUnk240PartUnk00 *out, Unk1B1470Request *request, Unk1B1470Extra *extra) {
    f32 strength;
    s32 whole;

    if (request->owner == (Entity *)&D_00413F80) {
        if (extra->unk00 == 0) {
            STORE_ROUNDED_UP(out->unk00[0], (f32)request->unk50.unk00[0] + (f32)extra->unk20 * (f32)request->unk50.unk00[0] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[1], (f32)request->unk50.unk00[1] + (f32)extra->unk20 * (f32)request->unk50.unk00[1] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[2], (f32)request->unk50.unk00[2] + (f32)extra->unk20 * (f32)request->unk50.unk00[2] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[3], (f32)request->unk50.unk00[3] + (f32)extra->unk22 * (f32)request->unk50.unk00[3] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[4], (f32)request->unk50.unk00[4] + (f32)extra->unk22 * (f32)request->unk50.unk00[4] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[5], (f32)request->unk50.unk00[5] + (f32)extra->unk22 * (f32)request->unk50.unk00[5] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[6], (f32)request->unk50.unk00[6] + (f32)extra->unk22 * (f32)request->unk50.unk00[6] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[7], (f32)request->unk50.unk00[7] + (f32)extra->unk22 * (f32)request->unk50.unk00[7] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[8], (f32)request->unk50.unk00[8] + (f32)extra->unk22 * (f32)request->unk50.unk00[8] / 50.0f);
        } else {
            STORE_ROUNDED_UP(out->unk00[0], (f32)request->unk50.unk00[0] + (f32)extra->unk22 * (f32)request->unk50.unk00[0] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[1], (f32)request->unk50.unk00[1] + (f32)extra->unk22 * (f32)request->unk50.unk00[1] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[2], (f32)request->unk50.unk00[2] + (f32)extra->unk22 * (f32)request->unk50.unk00[2] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[3], request->unk50.unk00[3] + (f32)extra->unk22 * request->unk50.unk00[3] / 50.0f + extra->unk0E[0] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[4], request->unk50.unk00[4] + (f32)extra->unk22 * request->unk50.unk00[4] / 50.0f + extra->unk0E[1] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[5], request->unk50.unk00[5] + (f32)extra->unk22 * request->unk50.unk00[5] / 50.0f + extra->unk0E[2] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[6], request->unk50.unk00[6] + (f32)extra->unk22 * request->unk50.unk00[6] / 50.0f + extra->unk0E[3] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[7], request->unk50.unk00[7] + (f32)extra->unk22 * request->unk50.unk00[7] / 50.0f + extra->unk0E[4] / 50.0f);
            STORE_ROUNDED_UP(out->unk00[8], request->unk50.unk00[8] + (f32)extra->unk22 * request->unk50.unk00[8] / 50.0f + extra->unk0E[5] / 50.0f);
        }
    } else if (request->owner >= D_003BEDF0[0] && request->owner <= &D_003D6C70) {
        if (extra->unk00 == 0) {
            *out = request->unk50;
        } else {
            Entity *owner = request->owner;

            out->unk00[0] = 0;
            out->unk00[2] = 0;
            out->unk00[1] = 0;
            STORE_ROUNDED_UP(out->unk00[3], request->unk50.unk00[3] * (f32)ENTITY_HIT_SCALES(owner)[0] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[4], request->unk50.unk00[4] * (f32)ENTITY_HIT_SCALES(owner)[1] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[5], request->unk50.unk00[5] * (f32)ENTITY_HIT_SCALES(owner)[2] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[6], request->unk50.unk00[6] * (f32)ENTITY_HIT_SCALES(owner)[3] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[7], request->unk50.unk00[7] * (f32)ENTITY_HIT_SCALES(owner)[4] / 10.0f);
            STORE_ROUNDED_UP(out->unk00[8], request->unk50.unk00[8] * (f32)ENTITY_HIT_SCALES(owner)[5] / 10.0f);
        }
    } else {
        if (extra->unk00 == 0) {
            *out = request->unk50;
        } else {
            out->unk00[0] = 0;
            out->unk00[2] = 0;
            out->unk00[1] = 0;
            STORE_ROUNDED_UP(out->unk00[3], request->unk50.unk00[3] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[4], request->unk50.unk00[4] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[5], request->unk50.unk00[5] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[6], request->unk50.unk00[6] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[7], request->unk50.unk00[7] * 100.0f / 10.0f);
            STORE_ROUNDED_UP(out->unk00[8], request->unk50.unk00[8] * 100.0f / 10.0f);
        }
    }
}
