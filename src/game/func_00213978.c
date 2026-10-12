#include "kf_script.h"
#include "kf_map.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern int D_00446770;

void *func_001B1390(void);
int func_001B4F30(u16, void *, void *, void *, void *);

/* Launch shot `object_id` from a point in a direction. */
void func_00213978(ScriptPointCommand *command) {
    VuVector4 from = (VuVector4){ command->from_x.value, command->from_y.value, command->from_z.value, 1.0f };
    VuVector4 angles = (VuVector4){ command->to_x.value, command->to_y.value, command->to_z.value, 1.0f };
    PlayerShot *shot;

    shot = func_001B1390();
    shot->unk00 = 1;
    shot->unk20 = 0;
    shot->unk22 = 0;
    func_001B4F30(command->object_id.value, shot, &from, &angles, &D_0038CD20[D_00446770]);
    command->active = 0;
}
