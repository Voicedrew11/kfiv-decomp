#include "kf_entity.h"

void func_00216D20(float *, float *);

/* Classifies the sign of the turn and of the entity's push direction's y into a 0x101..0x221 code. */
int func_00191340(Entity *entity, int turn) {
    float push[4];
    int code = 0;

    push[0] = push[1] = push[2] = push[3] = 0.0f;
    if (entity->flags & (0xC000L << 31)) {
        push[0] += entity->unkA4[0];
        push[1] += entity->unkA4[1];
        push[2] += entity->unkA4[2];
    }
    if ((s32)(entity->flags >> 48) & 1) {
        push[0] += entity->unkB0[0];
        push[1] += entity->unkB0[1];
        push[2] += entity->unkB0[2];
    }
    func_00216D20(push, push);
    if (turn > 0) {
        code = 0x201;
        if (push[1] > 0.0f) {
            code = 0x221;
        } else if (push[1] < 0.0f) {
            code = 0x211;
        }
    } else if (turn < 0) {
        code = 0x101;
        if (push[1] > 0.0f) {
            code = 0x121;
        } else if (push[1] < 0.0f) {
            code = 0x111;
        }
    }
    return code;
}
