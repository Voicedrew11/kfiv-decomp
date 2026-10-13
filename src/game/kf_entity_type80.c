/* The behaviour of entity type 80 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
extern float D_00413F80[];
extern u8 D_0034B217[];
int rand(void);
float sqrtf(float);
float atan2f(float, float);
double atan2(double, double);
char *func_0010CAB8(char *, u32);
float func_00194FB8(float);
float func_00194F80(void *, void *);
float func_00195478(float *, char *);
f32 func_00195018(f32, f32, f32);
void func_00216778(void *, void *);
int func_00195E48(void *, int, void *, void *);
void func_0018DA38(Entity *, s32, s32, s32);
int func_00192C98(void *, int, int, u32, int);

static void func_00142EF8(Entity *entity, s32 unknown);

/* Set-up of a type 80 entity: clears its work area and installs the type's handlers. */
void EntityType80_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_00142EF8;
    unkE0->unk04 = func_00182A08;
    unkE0->unk08 = (void (*)(void))func_00182BD0;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    unkE0->unk18 = NULL;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
    } else {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00142EF8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angles[4];
    f32 target[4];
    f32 *from;
    f32 *tip;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    f32 pitch;
    f32 yaw;

    if (D_0034B217[0] != 0) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk14, 0);
        return;
    }
    if (entity->unkA0 == 0) {
        from = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432);
        tip = (f32 *)func_0010CAB8((char *)entity->model, entity->params->unk432 + 1);
        if (from == NULL || tip == NULL) {
            return;
        }
        if (entity->params->unk432 != 0xFF) {
            delta_x = tip[0] - from[0];
            delta_y = tip[1] - from[1];
            delta_z = tip[2] - from[2];
            angles[0] = -atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
            angles[1] = func_00194FB8(-atan2f(delta_z, delta_x) + 1.5707964f);
            angles[2] = angles[3] = 0.0f;
        } else {
            func_00216778(angles, entity->model->unkD0);
            angles[1] = func_00194FB8(angles[1] + 3.1415927f);
        }
        func_00216778(target, D_00413F80);
        target[1] -= 190.0f;
        pitch = func_00194FB8(-atan2(target[1] - from[1], func_00194F80(from, target)));
        yaw = func_00195478(from, (char *)target);
        angles[0] = func_00195018(angles[0], pitch, entity->params->unk436);
        angles[1] = func_00195018(angles[1], yaw, entity->params->unk438);
        if (unkE0->unk7A % 3 == entity->unkD7 % 3U) {
            func_00195E48(entity, entity->params->unk430, from, angles);
        } else {
            func_00195E48(entity, 0xCD, from, angles);
        }
        unkE0->unk7A++;
        if (unkE0->unk7A < 0) {
            unkE0->unk7A = 0;
        }
        func_0018DA38(entity, entity->params->unk43A, entity->params->unk43C, entity->params->unk43E);
        entity->unkA0 = entity->params->unk434 + rand() % 3;
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, unknown);
}
