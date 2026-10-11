#include "kf_entity.h"

extern float D_00413F80[];

int func_0019A448(void *, void *, int, int, int, int, int);
int func_00199EB8(void *, void *, float);
s32 func_0017F0F0(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_0017EEB0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind151D30 *slots = (EntityUnkE0Kind151D30 *)unkE0;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(entity->params, ENTITY_FLAG_BITS(entity)->bits58);
    s32 frame;
    s32 launch_frame;
    s32 current_frame;
    f32 unused_vector[4]; // match: the frame has 16 unused bytes, and they keep the tail calls from becoming jumps

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &((EntityUnkE0Kind151D30 *)&entity->unkE0)->unk80, 0, ENTITY_FLAG_BITS(entity)->bits58 + 0x17, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->action_step++;
    case 1:
        if (slots->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (unk3D8->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (slots->unk80.unk02 <= unk3D8->unk01) {
            func_00199EB8(entity, &entity->unkE0.unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = (slots->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        current_frame = slots->unk80.unk02;
        launch_frame = ((EntityParamsUnk3D8Full *)unk3D8)->unk10[0].unk02;
        if (launch_frame != 0 && frame < launch_frame && launch_frame <= current_frame) {
            func_0017F0F0(entity, unk3D8, frame, current_frame, D_00413F80, 0.7853982f, 0.7853982f);
        }
        if (slots->unk80.unk02 > 0 && unk3D8->unk03 >= slots->unk80.unk02) {
            ENTITY_FLAG_BITS(entity)->bit19 = 1;
        } else {
            ENTITY_FLAG_BITS(entity)->bit19 = 0;
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
