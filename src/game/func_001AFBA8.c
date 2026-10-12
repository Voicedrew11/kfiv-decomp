#include "common.h"
#include "kf_effect.h"

/* Index of the keyframe segment the effect's frame falls in (or the first empty one), or -1 after three segments. */
int func_001AFBA8(void *effect, int *frames) {
    EffectActor *actor = effect;
    s32 segment;
    s32 *pair = frames;

    for (segment = 0; segment < 3; segment++, pair++) {
        if ((actor->frame >= pair[0] && actor->frame < pair[1]) || pair[1] - pair[0] <= 0) {
            return segment;
        }
    }
    return -1;
}
