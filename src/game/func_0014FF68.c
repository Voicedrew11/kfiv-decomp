#include "kf_entity.h"

extern EntityFrameTable D_0043E900;

float atan2f(float, float);
int func_00192C98(void *, int, int, u32, int);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
int func_00195E48(void *, int, void *, void *);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_0014FF68(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, 1);
    f32 direction[4];
    EntityFrameTable launch_frames = D_0043E900;
    f32 *source;
    f32 delta_x;
    f32 delta_z;
    s32 elapsed;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x18, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        elapsed = (entity->model->unkFC->unk84 - entity->params->unk80[entity->unkD1]) >> 8;
        for (i = 0; i < 4; i++) {
            if (entity->unk2A == 0 || unk3D8->unk0C != 0) {
                if (elapsed < launch_frames.frames[i] && entity->model->unkFC->unk82 >= launch_frames.frames[i]) {
                    source = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk43E);
                    if (source != NULL) {
                        delta_x = source[0] - entity->model->position[0];
                        delta_z = source[2] - entity->model->position[2];
                        direction[0] = -0.17453292f;
                        direction[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
                        direction[2] = direction[3] = 0.0f;
                        func_00195E48(entity, unk3D8->unk10[0].unk00, source, direction);
                    }
                }
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
