#include "common.h"

void func_00190A30(char *);
void func_0019A310(char *, void (*)(void), int);
void func_00195898(char *, u8);
void func_001814B0(void);

void func_001367B8(char *actor) {
    func_00190A30(actor);
    func_0019A310(actor, func_001814B0, 0);
    func_00195898(actor, 3);
}
