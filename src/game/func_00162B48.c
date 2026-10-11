#include "kf_entity.h"

void *func_001267B0(int);

/* Counts the first six objects whose flags low nibble is 3, then stores (count + 2) / 3. */
void func_00162B48(Entity *entity) {
    EntityUnkE0 *sub;
    Entity *object;
    int count;
    int next;
    int i;

    sub = &entity->unkE0;
    count = 0;
    i = 0;
    do {
        object = func_001267B0(i + 0x65);
        if (object != NULL) {
            next = count + 1;
            count = ((object->flags & 0xF) ^ 3) ? count : next;
        }
        i += 1;
    } while (i < 6);
    sub->unk7C = (char)((count + 2) / 3);
}
