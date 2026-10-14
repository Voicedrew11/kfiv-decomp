/* The behaviour of entity type 14 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *entity, void (*handler)(Entity *), int run_now);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00183570(Entity *);
void func_00181038(Entity *);
void func_001818B8(Entity *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
// match: the callee is void, but retail keeps what it leaves in $2
int func_00194368(EntityModel *, f32);
void func_00190A30(char *);
int func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_001814B0(Entity *);
extern u32 D_00446ED4;
void func_00196870(void *, int);
int rand(void);
int func_00192C98(void *, int, int, u32, int);
float func_00194FB8(float);
float func_00195388(float *, float, float);
void func_00195588(Vector4 *, float);
void func_0018D600(char **, int, u32, int);
s32 func_0018B150(f32 *, f32 *, f32, f32 *);
void func_00216778(void *, void *);
float func_00194F80(void *, void *);

static void func_001366C8(Entity *entity);
static void func_001367B8(Entity *entity);
static void func_001367F8(Entity *entity, s32 unknown);
static void func_001368B8(Entity *entity);

/* Set-up of a type 14 entity: clears its work area and installs the type's handlers. */
void EntityType14_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = func_001367F8;
    unkE0->unk04 = (void (*)(void))func_001366C8;
    unkE0->unk08 = (void (*)(void))func_001367B8;
    unkE0->unk18 = (void (*)(void))func_00183570;
    unkE0->unk10 = (void (*)(void))func_00181038;
    unkE0->unk14 = (void (*)(void))func_001818B8;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 0;
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

static void func_001366C8(Entity *entity) {
    u32 blend_frames; // match: converted to float as unsigned
    s64 flags;
    EntityUnkE0 *unkE0;

    flags = entity->flags;
    unkE0 = &entity->unkE0;
    if (ENTITY_FLAG_BITS(entity)->bit14) {
        blend_frames = entity->model->unkFC->unk88;
        if (blend_frames != 0) {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00194368(entity->model, (entity->unkC6 & 0x12) ? unkE0->unk40 : 1.0f / (f32)blend_frames);
        } else {
            ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, (s32)(flags >> 11) & 7);
        }
    }
}

static void func_001367B8(Entity *entity) {
    func_00190A30((char *)entity);
    func_0019A310((char *)entity, (void (*)(void))func_001814B0, 0);
    func_00195898((char *)entity, 3);
}

static void func_001367F8(Entity *entity, s32 unknown) {
    if (D_00446ED4 % 10 == entity->unkD7 % 10) {
        func_00196870(entity, 2);
    }
    if (entity->unkC6 == 0) {
        if (entity->state == 3) {
            func_001958B8(entity, func_001368B8, unknown);
        } else {
            func_001958B8(entity, func_001368B8, unknown);
        }
    }
}

static void func_001368B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 bounds[4];
    f32 direction[4];
    f32 heading;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 4, 0, entity->params->unkC4, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = 1;
        }
        unkE0->unk48 = entity->unk68 + 1.5707964f + (f32)(rand() % 180) * 0.017453292f;
        unkE0->unk48 = func_00194FB8(unkE0->unk48);
        ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
        ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
        entity->action_step++;
        break;
    case 1:
        heading = entity->model->unkD0[1];
        func_00195388(&heading, unkE0->unk48, 10.0f);
        entity->unk94 += heading - entity->model->unkD0[1];
        bounds[0] = entity->model->position[0] - entity->radius;
        bounds[1] = entity->model->position[0] + entity->radius;
        bounds[2] = entity->model->position[2] - entity->radius;
        bounds[3] = entity->model->position[2] + entity->radius;
        func_00195588((Vector4 *)direction, entity->model->unkD0[1]);
        if ((s32)(entity->flags >> 52) & 1) {
            ENTITY_FLAG_BITS(entity)->bit14 = FALSE;
            ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
            entity->unkA0 = 20;
            func_0018D600((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            entity->action_step = 3;
        } else if (func_0018B150(entity->model->position, direction, entity->radius, bounds)) {
            func_00216778(unkE0->unk30, entity->model->position);
            ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
            func_0018D600((char **)entity, entity->params->unk430, 0xFF, (int)entity->model->unkD0);
            entity->action_step++;
        }
        break;
    case 2:
        if (func_00194F80(entity->model->position, unkE0->unk30) > entity->radius * 4.0f) {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    case 3:
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
        }
        break;
    }
}
