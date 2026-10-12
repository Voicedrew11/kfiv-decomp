#include "kf_entity.h"

s32 func_00195BD8(Entity *, void *, s32, f32);

/* Runs func_00195BD8 for one part of an attack record, unless the animation frames [frame_from, frame_to]
   lie outside the frames the part is active on. Parts 3 to 5 take their frames from the four frame marks
   at params+0x434 instead and run as part 1, 0 and 1. */
s32 func_00157898(Entity *entity, EntityParamsUnk240 *unk240, s32 part, s32 frame_from, s32 frame_to, f32 angle) {
    if (part < 3) {
        if (unk240->unk10[part].unk24 == 0 ||
            (frame_from < unk240->unk10[part].unk24 && frame_to < unk240->unk10[part].unk24) ||
            (unk240->unk10[part].unk26 < frame_from && unk240->unk10[part].unk26 < frame_to)) {
            return 0;
        }
    } else if (part == 3) {
        // match: this kind's unk434 is a signed frame number (lh); the shared header has it u16
        if ((s16)entity->params->unk434 == 0 ||
            (frame_from < (s16)entity->params->unk434 && frame_to < (s16)entity->params->unk434) || // match: as above
            (entity->params->unk436 - 1 < frame_from && entity->params->unk436 - 1 < frame_to)) {
            return 0;
        }
        part = 1;
    } else if (part == 4) {
        if (entity->params->unk436 == 0 ||
            (frame_from < entity->params->unk436 && frame_to < entity->params->unk436) ||
            (entity->params->unk438 - 1 < frame_from && entity->params->unk438 - 1 < frame_to)) {
            return 0;
        }
        part = 0;
    } else if (part == 5) {
        if (entity->params->unk438 == 0 ||
            (frame_from < entity->params->unk438 && frame_to < entity->params->unk438) ||
            (entity->params->unk43A - 1 < frame_from && entity->params->unk43A - 1 < frame_to)) {
            return 0;
        }
        part = 1;
    }
    return func_00195BD8(entity, unk240, part, angle);
}
