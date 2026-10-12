#include "kf_map.h"

/* Switches the trigger on (type 1 to 2) or off (type 3 to 4) when it is at rest in the other position. */
void func_00122E18(MapTrigger *trigger, int turn_on) {
    if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 1 && !MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = TRUE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 2;
    } else if (MAP_TRIGGER_FLAGS(trigger)->bits.type == 3 && MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 && !turn_on) {
        MAP_TRIGGER_FLAGS(trigger)->bits.flag_20000 = FALSE;
        MAP_TRIGGER_FLAGS(trigger)->bits.type = 4;
    }
}
