#include "kf_entity.h"

extern PlayerData D_00413F80;

int rand(void);
double atan2(double, double);
int func_0019A448(void *, void *, int, int, int, int, int);
int func_00199EB8(void *, void *, float);
char *func_0010CAB8(char *, u32);
float func_00194F80(void *, void *);
float func_00194FB8(float);
void func_00216778(void *, void *);
int func_00173D38(Entity *, u8 *, int, int, void *);
s32 func_00173AB0(Entity *, EntityParamsUnk3D8 *, s32, s32, f32 *, f32, f32);
void func_001958B8(char *, void (*)(void), int);

void func_001735C8(Entity *entity) {
    EntityUnkE0Kind1719F8 *unkE0 = (EntityUnkE0Kind1719F8 *)&entity->unkE0;
    s32 kind = (s32)(ENTITY_FLAG_WORD(entity->flags) >> 58) & 3;
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)ENTITY_PARAMS_UNK3D8(entity->params, kind);
    f32 target[4];
    f32 angles[4];
    f32 *point;
    s32 frame;
    s32 elapsed;
    s32 launch_frame;
    s32 i;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, kind + 0x17, 0, entity->params->unkC4 >> 1, 1);
        func_0019A448(entity, &unkE0->unk88, 1, ((s32)(entity->flags >> 58) & 3) + 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (unkE0->unk80.unk01 == 0) {
            entity->action_step++;
        }
        if (record->unk01 != 0) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        }
        break;
    case 2:
        if (unkE0->unk80.unk02 <= record->unk01) {
            func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        } else {
            unkE0->unk1C = 0.0f;
        }
        frame = unkE0->unk80.unk02;
        elapsed = (unkE0->unk80.unk04 - entity->params->unk80[entity->unkD1]) >> 8;
        if (((s32)(entity->flags >> 58) & 3) == 1) {
            for (i = 0; i < 3; i++) {
                switch (i) {
                case 0:
                    launch_frame = record->unk10[0].unk02;
                    break;
                case 1:
                    launch_frame = entity->params->unk43A;
                    break;
                case 2:
                    launch_frame = entity->params->unk43C;
                    break;
                }
                if (launch_frame != 0 && elapsed < launch_frame && frame >= launch_frame) {
                    point = (f32 *)func_0010CAB8((char *)entity->model, 0);
                    angles[0] = func_00194FB8(-atan2(D_00413F80.position[1] - point[1], func_00194F80(point, &D_00413F80)));
                    if (!(rand() & 1)) {
                        angles[1] = func_00194FB8(entity->model->unkD0[1] + 3.1415927f + (f32)(rand() % 15) * 0.017453292f);
                    } else {
                        angles[1] = func_00194FB8(entity->model->unkD0[1] + 3.1415927f - (f32)(rand() % 15) * 0.017453292f);
                    }
                    angles[2] = 0.0f;
                    func_00173D38(entity, (u8 *)record, elapsed, unkE0->unk80.unk02, angles);
                }
            }
        } else {
            func_00216778(target, &D_00413F80);
            target[1] -= 200.0f;
            if (record->unk10[0].unk02 != 0 && elapsed < record->unk10[0].unk02 && frame >= record->unk10[0].unk02) {
                func_00173AB0(entity, (EntityParamsUnk3D8 *)record, elapsed, unkE0->unk80.unk02, target, 0.7853982f, 0.0f);
            }
        }
        if (unkE0->unk80.unk02 > 0 && record->unk03 >= unkE0->unk80.unk02) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->unkA2 = record->unk02;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
