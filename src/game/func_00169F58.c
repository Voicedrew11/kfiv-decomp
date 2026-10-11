#include "kf_entity.h"
#include "kf_map.h"

extern s32 D_00446770;
extern PlayerData D_00413F80;
extern MapSlot D_0038CD20[];
extern KfVector D_004FF390[];
extern f32 D_00446828[];

int rand(void);
int func_00192C98(void *, int, int, u32, int);
void func_0018B778(float, void *, void *, void *);
f32 func_0018A9F0(void *, void *);
int func_00195ED0(Entity *, EntityParamsUnk3D8 *, s32, s32);
void *func_001267B0(int);
float func_00194F80(void *, void *);
void func_0018DC40(char *);
float func_00194FB8(float);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);

void func_00169F58(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    EntityParams *params = entity->params;
    s32 kind = (s32)(entity->flags >> 58) & 3;
    EntityParamsUnk3D8 *unk3D8 = ENTITY_PARAMS_UNK3D8(params, kind);
    MapSlot *slot;
    KfVector *floor_point;
    KfVector *point;
    Entity *minion;
    EntityModel *minion_model;
    f32 height;
    f32 no_floor;
    f32 spin;
    s32 minion_index;
    s32 point_index;
    s32 count;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, kind + 0x17, 0, params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk6A = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unkE0->unk6A < 5) {
            func_0018B778(500.0f, &D_004FF390[unkE0->unk6A], &D_00413F80, &D_00413F80);
            slot = &D_0038CD20[D_00446770];
            height = func_0018A9F0((MapRecord *)slot->records + D_00413F80.unk252, &D_004FF390[unkE0->unk6A]);
            no_floor = D_00446828[0];
            if (height == no_floor) {
                height = func_0018A9F0(entity->unk14, &D_004FF390[unkE0->unk6A]);
                if (height == no_floor) {
                    break;
                }
            }
            floor_point = &D_004FF390[unkE0->unk6A];
            floor_point->y += height;
            unkE0->unk6A++;
        }
        break;
    case 2:
        unkE0->unk1C = 0.0f;
        func_00195ED0(entity, unk3D8, (entity->model->unkFC->unk84 - params->unk80[entity->unkD1]) >> 8,
                      entity->model->unkFC->unk82);
        if (entity->model->unkFC->unk82 == 0x11) {
            minion_index = 0;
            count = rand() % 5 + 1;
            if (unkE0->unk6A < count) {
                count = unkE0->unk6A;
            }
            for (point_index = 0; point_index < count; point_index++) {
                for (; minion_index < 8; minion_index++) {
                    minion = func_001267B0(minion_index + 10);
                    if (minion != NULL && (minion->flags & 3) == 1 &&
                        (((s32)(minion->flags >> 2) & 3) == 0 || ((s32)(minion->flags >> 2) & 3) == 3)) {
                        point = &D_004FF390[point_index];
                        if (func_00194F80(point, &D_00413F80) > 180.0f) {
                            func_0018DC40((char *)minion);
                            minion->model->position[0] = point->x;
                            minion->model->position[1] = point->y;
                            minion->model->position[2] = point->z;
                            minion->model->unkD0[1] = func_00194FB8((f32)(rand() % 360) * 0.017453292f);
                            minion_model = minion->model;
                            spin = minion_model->unkE0[0];
                            if (rand() % 3 != 0) {
                                spin = -spin;
                            }
                            minion_model->unkE0[0] = spin;
                            break;
                        }
                    }
                }
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
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
