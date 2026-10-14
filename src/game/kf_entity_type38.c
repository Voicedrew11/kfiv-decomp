/* The behaviour of entity type 38 (Entity.params->unk02, the index into D_003E6E40): its set-up
   function, which the table names, and the handlers that function installs. */
#include "kf_entity.h"
#include "common.h"

void func_001958A8(char *, s32);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00191478(EntityModel *);
int func_00193378(EntityModel *, s32, s32);
void func_00216778(void *, void *);
extern s32 D_003E6E40[][2];
int func_00192EB0(void *, int);
extern PlayerData D_00413F80;
float func_00195478(float *, char *);
float func_00194FB8(float);
// match: the definition names only the three integer arguments; the floats go on to the handler it calls in their registers
void func_001A0CD0(s32, s32, s32, f32, f32, f32);

static void func_00154250(Entity *entity);
static void func_001542B0(Entity *entity);

/* Set-up of a type 38 entity: clears its work area and installs the type's handlers. */
void EntityType38_Init(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    char *unk138 = entity->model->unk138;
    s32 i;

    for (i = ENTITY_WORK_AREA_SIZE - 1; i != -1; i--) {
        ENTITY_WORK_BYTES(entity)[i] = 0;
    }
    func_001958A8((char *)entity, D_003E6E40[entity->params->unk02][1]);
    unkE0->unk00 = (void (*)(Entity *, s32))func_001542B0;
    unkE0->unk04 = (void (*)(void))func_00154250;
    unkE0->unk08 = NULL;
    unkE0->unk18 = NULL;
    unkE0->unk10 = NULL;
    unkE0->unk14 = NULL;
    for (i = 3; i != -1; i--) {
        entity->unkBC[i] = 0x80;
    }
    for (i = 5; i != -1; i--) {
        entity->unkC0[i] = 0x80;
    }
    ENTITY_FLAG_BITS(entity)->bit28 = 1;
    ENTITY_FLAG_BITS(entity)->bit18 = 1;
    ENTITY_FLAG_BITS(entity)->bit21 = 1;
    ENTITY_FLAG_BITS(entity)->bit16 = 1;
    entity->target = NULL;
    if (ENTITY_SPAWN_FLAGS(entity) & (0x8000L << 43)) {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk10, 1);
    } else {
        func_001958B8(entity, (void (*)(Entity *))unkE0->unk18, 0);
    }
    if (unk138 != NULL) {
        func_00216778(unkE0->unk20, unk138 + 0x50);
    }
    unkE0->unk40 = 0.5f;
    entity->unkD6 = (entity->params->flags & 0x4000) ? 0x40 : 0x80;
    entity->model->unk15C = entity->unkD6;
    func_00191478(entity->model);
    func_00193378(entity->model, 0, 0);
}

static void func_00154250(Entity *entity) {
    ENTITY_FLAG_BITS(entity)->bit34 = func_00192EB0(entity, ENTITY_FLAG_BITS(entity)->bits11);
}

static void func_001542B0(Entity *entity) {
    u64 flags;
    f32 angle;

    flags = entity->flags & ~0x20000000ULL;
    entity->flags = flags;
    if ((s32)(flags >> 43) & 1) {
        if (entity->unkA0 == 0) {
            entity->flags = flags | 0x20000000ULL;
            angle = func_00195478(D_00413F80.position, (char *)entity->model->position);
            angle = func_00194FB8(angle - D_00413F80.unk14);
            func_001A0CD0((u8)entity->params->unk432, (u8)entity->params->unk430, entity->unkD2,
                          (f32)entity->params->unk42E, entity->unk60, angle);
            entity->unkA0 = entity->params->unk434;
        }
    }
    entity->flags &= ~(1ULL << 43);
}
