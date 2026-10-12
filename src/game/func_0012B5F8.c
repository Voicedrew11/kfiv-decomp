#include "kf_entity.h"

extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
int func_00197078(void);
int func_0018F030(char *);
void func_00196D48(Entity *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
int func_001958B8(Entity *, void (*)(Entity *), int);
int func_001958F8(Entity *, void (*)(Entity *entity), int, int, int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_0012B5F8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
        return;
    }
    if (entity->unkA2 == 0 && ((int (*)(Entity *))func_00197078)(entity)) { // match: the entity is passed
        entity->target = NULL;
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
        }
        return;
    }
    entity->flags |= 0x200;
    if ((s32)(entity->flags >> 32) & 1) {
        if (unkE0->unk7A == 1) {
            unkE0->unk7A = 0;
            if (rand() % 100 < 40) {
                func_001958F8(entity, (void (*)(void))func_00182240, unknown, 1, 0);
                return;
            }
        }
    } else {
        unkE0->unk7A = 1;
    }
    func_00196D48(entity);
    if (ENTITY_FLAG_BITS(entity)->bit31) {
        angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
        if (unkE0->unk79 == 0) {
            unkE0->unk79 = ABS(angle) > 0.5235988f;
            if (unkE0->unk79 == 0) {
                goto done;
            }
        }
        entity->unkC8 = func_00190BE0(entity);
        if (entity->unkC8 == 0) {
            unkE0->unk79 = 0;
        }
    } else if (unkE0->unk78 == 1) {
        entity->target = &D_00413F80;
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x241;
        } else if (turn < 0) {
            entity->unkC8 = 0x141;
        } else {
            entity->unkC8 = 0x841;
        }
    } else {
        unkE0->unk78 = 1;
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
