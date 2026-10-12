#include "kf_entity.h"

extern EntityFrameTable5 D_0043E8E8;
extern float D_00413F80[];

int rand(void);
int func_00192C98(void *, int, int, u32, int);
void func_00216778(void *, void *);
int func_00195E48(void *, int, void *, void *);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_0014FCC0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Full *unk3D8 = (EntityParamsUnk3D8Full *)&entity->params->unk3D8[0];
    f32 target[4];
    EntityFrameTable5 launch_frames = D_0043E8E8;
    s32 elapsed;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
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
        for (i = 0; i < 5; i++) {
            if (entity->unk2A == 0 || unk3D8->unk0C != 0) {
                if (elapsed < launch_frames.frames[i] && entity->model->unkFC->unk82 >= launch_frames.frames[i]) {
                    func_00216778(target, D_00413F80);
                    target[0] += rand() % 401 - 200;
                    target[2] += rand() % 401 - 200;
                    func_00195E48(entity, unk3D8->unk10[0].unk00, target, entity->model->unkD0);
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
