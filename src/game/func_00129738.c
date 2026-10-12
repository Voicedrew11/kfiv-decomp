#include "kf_entity.h"

void func_00190A30(char *);
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00195898(char *, u8);
void func_00129CA0(Entity *);

void func_00129738(Entity *entity) {
    func_00190A30((char *)entity);
    entity->target = NULL;
    func_001958B8((char *)entity, (void (*)(void))func_00129CA0, 0);
    func_00195898((char *)entity, 2);
}
