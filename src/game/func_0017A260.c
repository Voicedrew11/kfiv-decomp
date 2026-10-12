#include "kf_entity.h"

extern EntityFrameTable D_0043EAD8;
extern int D_00446770;

int func_00192C98(void *, int, int, u32, int);
int func_002109C8(int, int);
void func_001B57A0(u8, u8, u8);
void func_0017A158(Entity *, s32);

void func_0017A260(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    EntityFrameTable shake_frames = D_0043EAD8;
    u32 i;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if ((s16)entity->params->unk434 < entity->model->unkFC->unk82) { // match: read signed here
            if (entity->group_id == D_00446770) {
                // match: an int result keeps $2 busy after the call
                ((int (*)(s32, s32))func_002109C8)(entity->params->unk436, 0);
                entity->action_step++;
            }
        }
        break;
    case 3:
        for (i = 0; i < 4; i++) {
            if (entity->model->unkFC->unk82 == shake_frames.frames[i]) {
                if (entity->distance_to_player < 3000.0f) {
                    func_001B57A0((3000.0f - entity->distance_to_player) * 64.0f / 3000.0f, 5, 0);
                }
            }
        }
        if (entity->model->unkFC->unk82 == 0x1B8) {
            if (entity->distance_to_player < 3000.0f) {
                func_001B57A0((3000.0f - entity->distance_to_player) * 32.0f / 3000.0f, 0x3C, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk48 = entity->model->position[0];
            unkE0->unk4C = entity->model->position[1];
            unkE0->unk50 = entity->model->position[2];
            unkE0->unk00 = func_0017A158;
            unkE0->unk7A = 1;
            unkE0->unk00(entity, 1);
        }
        break;
    }
}
