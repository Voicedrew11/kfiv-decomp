/* The entity pools, their slots and the model work area. */
#include "kf_entity.h"
#include "kf_model.h"

ModelInstance entity_models[ENTITY_POOL_SLOT_COUNT] = { 0 }; /* 0x3BE530 a model per pool slot */

Entity entity_pool[2][0x80] = { 0 }; /* 0x3BEDF0 the entities of both map slots */
/* the last entity of entity_pool, which other files name on its own */
asm(".globl D_003D6C70\nD_003D6C70 = entity_pool + 0x17E80");  /* 0x3D6C70 the 256th entity */

EntityPoolSlot entity_pool_slots[ENTITY_POOL_SLOT_COUNT] = { 0 }; /* 0x3D6DF0 the pool slots */

s64 entity_work_area[0x2000] = { 0 }; /* 0x3D6E40 the work area func_0010CE08 builds a model in */
