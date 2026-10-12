#include "kf_entity.h"

// match: reading EntityParams word 0 whole; a bit-field in byte 3 would be an lbu
typedef union {
    u32 word;
} EntityParamsFirstWord;

#define ENTITY_PARAMS_WORD(params) (((EntityParamsFirstWord *)(params))->word)

void func_00185480(Entity *);
void func_00186B00(Entity *);
void func_00187E28(Entity *);
void func_001888C0(Entity *);
void func_00188BE0(Entity *);

/* per-frame flag setup for an entity: clears its push and state bits, sets bits 22-27 from its params, then runs its floor and ledge checks */
void func_00199BB0(Entity *entity) {
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 51);
    entity->unkA4[0] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 52);
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 45);
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 46);
    entity->unkA4[1] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 49);
    entity->unkA4[2] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 48);
    entity->unkB0[0] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 47);
    entity->unkB0[1] = 0;
    ENTITY_FLAG_WORD(entity->flags) &= ~(1ULL << 50);
    entity->unkB0[2] = 0;

    if (ENTITY_PARAMS_BITS(entity)->bit25 && ENTITY_PARAMS_BITS(entity)->bit26) {
        return;
    }
    if (!(entity->params->flags & 0x1)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x400000;
    }
    if (!(entity->params->flags & 0x2)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x800000;
    }
    if (!(entity->params->flags & 0x4)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x1000000;
    }
    if (!(entity->params->flags & 0x8)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x2000000;
    }
    if (!(entity->params->flags & 0x10)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x4000000;
    }
    if (!(entity->params->flags & 0x20)) {
        ENTITY_FLAG_WORD(entity->flags) |= 0x8000000;
    }

    func_00185480(entity);

    if (!((s32)(ENTITY_FLAG_WORD(entity->flags) >> 25) & 1)) {
        if (ENTITY_PARAMS_WORD(entity->params) & 0x1000000) {
            func_00188BE0(entity);
        } else {
            func_001888C0(entity);
        }
    }
    if (!(ENTITY_PARAMS_BITS(entity)->bit25 || ENTITY_PARAMS_BITS(entity)->bit26)) {
        func_00186B00(entity);
    } else {
        func_00187E28(entity);
    }

    if ((u32)(ENTITY_FLAG_WORD(entity->flags) >> 51) & 1) {
        entity->unk48 += 2.0f;
        if (40.0f < entity->unk48) {
            entity->unk48 = 40.0f;
        }
    } else {
        entity->unk48 = 10.0f;
    }
}
