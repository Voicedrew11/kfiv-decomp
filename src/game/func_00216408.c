#include "kf_script.h"
#include "kf_entity.h"

Entity *func_001267B0(int);
int func_00211CF8(ScriptArg *, int);

/* Script command: gather sixteen of the entity's flags into one word for the script (the bits
   func_00216088 sets). */
void func_00216408(ScriptLookupCommand *command) {
    Entity *entity;
    int result;

    entity = func_001267B0(command->input.value);
    if (entity != NULL) {
        result = ENTITY_FLAG_BITS(entity)->bit1;
        if (ENTITY_FLAG_BITS(entity)->bits2 >= 2) {
            result |= 2;
        }
        if (ENTITY_FLAG_BITS(entity)->bit5) {
            result |= 0x4;
        }
        if (ENTITY_FLAG_BITS(entity)->bit6) {
            result |= 0x8;
        }
        if (ENTITY_FLAG_BITS(entity)->bit7) {
            result |= 0x10;
        }
        if (ENTITY_FLAG_BITS(entity)->bit18) {
            result |= 0x20;
        }
        if (ENTITY_FLAG_BITS(entity)->bit22) {
            result |= 0x40;
        }
        if (ENTITY_FLAG_BITS(entity)->bit23) {
            result |= 0x80;
        }
        if (ENTITY_FLAG_BITS(entity)->bit24) {
            result |= 0x100;
        }
        if (ENTITY_FLAG_BITS(entity)->bit25) {
            result |= 0x200;
        }
        if (ENTITY_FLAG_BITS(entity)->bit26) {
            result |= 0x400;
        }
        if (ENTITY_FLAG_BITS(entity)->bit27) {
            result |= 0x800;
        }
        if (ENTITY_FLAG_BITS(entity)->bit28) {
            result |= 0x1000;
        }
        if (ENTITY_FLAG_BITS(entity)->bit14) {
            result |= 0x2000;
        }
        if (ENTITY_FLAG_BITS(entity)->bit9) {
            result |= 0x4000;
        }
        if (ENTITY_FLAG_BITS(entity)->bit29) {
            result |= 0x8000;
        }
        func_00211CF8(&command->result, result);
    }
    command->active = 0;
}
