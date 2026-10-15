#include "kf_entity.h"

int Motion_Start(void *, int, int, u32, int);
void EntityAi_SetUpdate(char *, s32);
void *memset(void *, int, u32);
extern s32 D_003E6E40[][2];

/* Default init of an entity: clears the work area, sets flags 22-28 and the model of its kind. */
void func_00124E48(Entity *entity) {
    Motion_Start(entity, 0, 0, 0, 0);
    memset(&entity->unkE0, 0, ENTITY_WORK_AREA_SIZE);
    ENTITY_FLAG_BITS(entity)->bit22 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit24 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit25 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = TRUE;
    EntityAi_SetUpdate((char *)entity, D_003E6E40[entity->params->unk02][1]);
}
