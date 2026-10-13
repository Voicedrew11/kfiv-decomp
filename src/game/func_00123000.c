#include "common.h"
#include "kf_message.h"

void func_00123000(int sound_id) {
    if (sound_id >= 0x2F && sound_id < 0x3C) {
        KF_Message_ShowNotice(0, 0, 0, 0, 0x2D, sound_id + 0x28);
    }
}
