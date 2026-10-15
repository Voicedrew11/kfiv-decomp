/* Map object definitions, both slots' objects and per-kind handlers. */
#include "kf_game.h"
#include "kf_map.h"
#include "kf_model.h"

/* the handlers the kind tables name (kf_map_object_*.c) */
void MapObject_UpdateClosedChest(void *arg, int unused);
void MapObject_UpdateClosedDoor(MapObject *object);
void MapObject_UpdateOpenChest(MapObject *object);
void MapObject_UpdateSavePoint(MapObject *object);
void func_0011EDC8(MapObject *object, void *offset);
void func_0011EE18(MapObject *object);
void func_0011EE78(MapObject *object);
void func_0011EFC8(MapObject *object);
void func_0011F0D8(MapTrigger *trigger);
void func_0011F148(char *arg);
int func_0011F220();
void func_0011F228();
void func_0011F230();
void func_0011F238(MapObject *object);
void func_0011F498(MapObject *object);
void func_0011F708(MapObject *object);
void func_0011F7E0(MapObject *object);
void func_0011F910(MapObject *object, int open);
void func_0011F980(MapObject *object);
void func_0011FAA0(MapObject *object);
void func_0011FAB0(MapObject *object);
void func_0011FBE8(MapObject *object);
void func_0011FC10(MapObject *object);
void func_0011FCF8(MapObject *object);
void func_0011FFF8(MapObject *object);
void func_00120128();
void func_00120130(GameEvent *event, int mode);
void func_001201D8(MapObject *object);
void func_001201E0(MapObject *object);
void func_00120278(MapObject *object);
void func_001203F0(MapObject *object);
void func_00120668(MapObject *object);
void func_00120698(MapObject *object);
void func_001206F0(MapObject *object);
void func_00120778(MapObject *object);
void func_001207D0(void *arg);
void func_00120810(MapObject *object);
void func_00120868(void *arg);
void func_001208B0(MapObject *object);
void func_00120908(MapObject *object, int turn_on);
void func_00120A10(MapObject *object);
void func_00120BB8(MapObject *object);
void func_00120D38(MapObject *object);
void func_00120D78(MapObject *object);
void func_00120D88();
void func_00120D90(MapObject *object);
void func_00120DC8(MapObject *object);
void func_00120E10(MapObject *object);
void func_00120E48(MapObject *object);
void func_00120EA0(MapObject *object);
void func_00120EC8(MapObject *object);
void func_00120ED8();
void func_00120EE0(MapObject *object);
void func_00120F48(MapObject *object);
void func_001211B0(MapObject *object);
void func_00121228(MapObject *object);
void func_001212B0(MapObject *object);
void func_00121398(MapObject *object);
void func_00121578(MapObject *object);
void func_00121618(char *object);
int func_00121620(MapObject *object, int item);
void func_00121660(MapTrigger *trigger);
void func_00121680(MapObject *object);
void func_001217B0(MapObject *object);
void func_001217C0(MapObject *object);
void func_00121958(MapObject *object);
void func_00121968(MapObject *object);
void func_00121B10(MapObject *object, int enabled);
void func_00121B38(MapObject *object);
void func_00121BA8(MapObject *object);
void func_00121BE8(MapObject *object);
void func_00121C68(MapObject *object);
void func_00121CA0(MapObject *object);
void func_00121D20(MapObject *object, int turn_on);
void func_00121DA8(MapObject *object);
void func_00121E78(MapObject *object);
void func_001220B8(MapObject *object);
void func_00122138(MapObject *object);
void func_001221C0(MapObject *object);
void func_001222C0(MapObject *object);
void func_00122480(MapObject *object);
void func_00122530(char *object);
int func_00122538(MapTrigger *trigger, int type_index);
void MapObject_StartSavePoint(MapObject *object);
void MapObject_StartContainer(MapObject *object);
void MapObject_UpdateContainer(MapObject *object);
void MapObject_FadeOutContainer(MapObject *object);
void MapObject_SetUpContainer(MapObject *object);
void MapObject_StartSwitch(MapObject *object);
void MapObject_UpdateSwitchOff(MapObject *object);
void MapObject_TurnSwitchOn(MapObject *object);
void MapObject_UpdateSwitchOn(MapObject *object);
void MapObject_TurnSwitchOff(MapObject *object);
void MapObject_SignalSwitch(MapTrigger *trigger, int turn_on);
void MapObject_PoseSwitch(MapTrigger *trigger);
void MapObject_StartFadeObject(MapTrigger *trigger);
void func_00122CA8();
void MapObject_FadeInObject(MapTrigger *trigger);
void func_00122D60();
void MapObject_FadeOutObject(MapTrigger *trigger);
void MapObject_SignalFadeObject(MapTrigger *trigger, int turn_on);
void MapObject_StartPulse(MapTrigger *trigger);
int MapObject_UpdatePulse(MapTrigger *trigger);

char map_object_defs[0x14000] = { 0 }; /* 0x38DA30 the param/obj.prm row of every object */

MapObject map_slot_objects[2][0x200] = { 0 }; /* 0x3A1A30 the objects of both map slots, a row each */
/* its second row, which other files name on its own */
asm(".globl D_003AFA30\nD_003AFA30 = map_slot_objects + 0xE000");  /* 0x3AFA30 the objects of the other loaded map */

void (*map_object_step_handlers[0x20][16])(MapObject *) = { /* 0x3BDA30 the step handler of each object kind */
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011F238, MapObject_UpdateClosedDoor, func_0011F498, func_0011F708, func_0011F7E0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011F238, MapObject_UpdateClosedDoor, func_0011F498, func_0011F708, func_0011F7E0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011FAA0, func_0011FAB0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011FBE8,
    func_0011FCF8, (void (*)(MapObject *))MapObject_UpdateClosedChest, func_0011FFF8, MapObject_UpdateOpenChest, (void (*)(MapObject *))func_00120128, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_001201E0, func_00120278, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011F238, MapObject_UpdateClosedDoor, func_0011F498, func_0011F708, func_0011F7E0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120668, func_00120698, func_001206F0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120778, (void (*)(MapObject *))func_001207D0, func_00120810, (void (*)(MapObject *))func_00120868, func_001208B0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120A10, func_00120BB8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120D78, (void (*)(MapObject *))func_00120D88, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120D90, func_00120DC8, func_00120E10, func_00120E48, func_00120EA0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120EC8, (void (*)(MapObject *))func_00120ED8, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00120EE0, func_00120F48, func_001211B0, func_00121228, func_001212B0, func_00121398, func_00121578, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    (void (*)(MapObject *))func_00121660, func_00121680, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_001217B0, func_001217C0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00121958, func_00121968, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011F238, MapObject_UpdateClosedDoor, func_0011F498, func_0011F708, func_0011F7E0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00121B38, func_00121BA8, func_00121BE8, func_00121C68, func_00121CA0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_00121DA8, func_00121E78, func_001220B8, func_00122138, func_001221C0, func_001222C0, func_00122480, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    MapObject_StartSavePoint, MapObject_UpdateSavePoint, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    MapObject_StartContainer, MapObject_UpdateContainer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, MapObject_FadeOutContainer,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    func_0011EE18, func_0011EE78, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    (void (*)(MapObject *))MapObject_StartPulse, (void (*)(MapObject *))MapObject_UpdatePulse, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    MapObject_StartSwitch, MapObject_UpdateSwitchOff, MapObject_TurnSwitchOn, MapObject_UpdateSwitchOn, MapObject_TurnSwitchOff, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
    (void (*)(MapObject *))MapObject_StartFadeObject, (void (*)(MapObject *))func_00122CA8, (void (*)(MapObject *))MapObject_FadeInObject, (void (*)(MapObject *))func_00122D60, (void (*)(MapObject *))MapObject_FadeOutObject, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, func_0011EFC8,
};

void (*map_object_draw_hooks[0x20])(MapObject *) = { /* 0x3BE230 the kind's draw hook */
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, func_00120D38, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
    (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8, (void (*)(MapObject *))func_0011F0D8,
};

int (*map_object_item_hooks[0x20])(MapObject *, int) = { /* 0x3BE2B0 the kind's item hook */
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
    (int (*)(MapObject *, int))func_0011F220, func_00121620, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_00122538,
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
    (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220, (int (*)(MapObject *, int))func_0011F220,
};

void (*map_object_signal_hooks[0x20])(MapObject *, int) = { /* 0x3BE330 the kind's signal hook */
    (void (*)(MapObject *, int))func_0011F228, func_0011F910, func_0011F910, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_00120130, (void (*)(MapObject *, int))func_0011F228, func_0011F910, (void (*)(MapObject *, int))func_0011F228,
    func_00120908, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    func_00121B10, func_0011F910, func_00121D20, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228,
    (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))func_0011F228, (void (*)(MapObject *, int))MapObject_SignalSwitch, (void (*)(MapObject *, int))MapObject_SignalFadeObject,
};

void (*map_object_update_hooks[0x20])(MapObject *) = { /* 0x3BE3B0 the kind's every-frame update */
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148,
    (void (*)(MapObject *))func_0011F148, func_001203F0, func_0011F980, (void (*)(MapObject *))func_0011F148,
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148,
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_00121618, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148,
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_00122530,
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148,
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148,
    (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))func_0011F148, (void (*)(MapObject *))MapObject_PoseSwitch, (void (*)(MapObject *))func_0011F148,
};

void (*map_object_offset_hooks[0x20])(MapObject *, f32 *) = { /* 0x3BE430 the kind's move by an offset */
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
    (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8, (void (*)(MapObject *, f32 *))func_0011EDC8,
};

void (*map_object_setup_hooks[0x20])(MapObject *) = { /* 0x3BE4B0 the kind's set-up */
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, func_0011FC10,
    func_001201D8, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, MapObject_SetUpContainer, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
    (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230, (void (*)(MapObject *))func_0011F230,
};
