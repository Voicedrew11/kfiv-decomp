#include "common.h"
#include "kf_entity.h"
#include "kf_script.h"

extern Entity D_003BEDF0[][0x80];
extern s32 D_00446770;
extern s32 D_004467D8[2];

/* Copy bit 0 of the request into flag bit 5 of every entity in the current map's list, depending on the request kind. */
void func_00215A18(ScriptFlagCommand *request) {
    Entity *entity = D_003BEDF0[D_00446770];
    s32 i;

    i = D_004467D8[D_00446770];
    while (--i != -1) {
        if (request->kind == 1) {
            if (!ENTITY_FLAG_BITS(entity)->bit16) {
                ENTITY_FLAG_BITS(entity)->bit5 = request->value;
            }
        } else if (request->kind == 2) {
            if (ENTITY_FLAG_BITS(entity)->bit16) {
                ENTITY_FLAG_BITS(entity)->bit5 = request->value;
            }
        } else {
            ENTITY_FLAG_BITS(entity)->bit5 = request->value;
        }
        entity++;
    }
    request->active = 0;
}
