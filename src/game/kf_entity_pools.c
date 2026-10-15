/* The entity pools: a model instance per pool slot, the entities of both map slots, the
   pool slots the entity lists are built from and the work area a model is built into. */
#include "kf_entity.h"
#include "kf_model.h"

ModelInstance D_003BE530[ENTITY_POOL_SLOT_COUNT] = { 0 }; /* 0x3BE530 a model per pool slot */

Entity D_003BEDF0[2][0x80] = { 0 }; /* 0x3BEDF0 the entities of both map slots */
/* the last entity of D_003BEDF0, which other files name on its own */
asm(".globl D_003D6C70\nD_003D6C70 = D_003BEDF0 + 0x17E80");  /* 0x3D6C70 the 256th entity */

EntityPoolSlot D_003D6DF0[ENTITY_POOL_SLOT_COUNT] = { 0 }; /* 0x3D6DF0 the pool slots */

s64 D_003D6E40[0x2000] = { 0 }; /* 0x3D6E40 the work area func_0010CE08 builds a model in */
