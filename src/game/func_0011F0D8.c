#include "kf_map.h"

extern int D_00446770;
int Sound_StopTagged(char);

void func_0011F0D8(MapTrigger *trigger) {
    TriggerSource *source;

    if (*trigger->unk04 == 0) {
        source = MAP_TRIGGER_SOURCE(trigger);
        if (((u32)(source->unk44 - 0x40) < 0x40) && (trigger->shot.unk00[0] != 0)) {
            if ((trigger->unk10 & 1) != D_00446770) {
                trigger->shot.unk00[0] = 0;
                return;
            }
            Sound_StopTagged((char)source->unk44);
            trigger->shot.unk00[0] = 0;
        }
    }
}
