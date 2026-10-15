#include "kf_model.h"
#include "kf_splash.h"
#include "kf_message.h"

extern u32 D_0038CC74[];
extern void *D_00420D50[];

/* Step 0 of the things that are only examined (kind 0xB): takes the message (the definition's name when the
   source gives none) and, from the source, whether a sign picture goes with it; step 1. */
void MapObject_StartExaminable(MapObject *object) {
    s32 *work = object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);

    work[1] = source->unk44;
    if (work[1] == 0) {
        work[1] = (s16)object->unk04->unk68;
    }
    object->work[0] = source->unk46;
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Step 1 of kind 0xB: used by the player, it shows its sign picture (step 2) when it has one, else its
   message (step 3). */
void MapObject_UpdateExaminable(MapObject *object) {
    MapObject *used;

    if (((D_0038CC74[0] >> 1) & 1) && (used = D_00420D50[0]) == object) {
        if (used->work[0] != 0) {
            MAP_OBJECT_BYTES(used)->step = 2;
        } else {
            MAP_OBJECT_BYTES(used)->step = 3;
        }
    }
}

/* Step 2 of kind 0xB: back to step 1 once Splash_Start has taken the sign picture (msg/*.tm2) of the
   message's number. */
void MapObject_ShowExaminedSign(MapObject *object) {
    if (Splash_Start(object->work[1])) {
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}

/* Step 3 of kind 0xB: shows the message for 45 frames; step 4. */
void MapObject_ShowExaminedMessage(MapObject *object) {
    s32 *work = object->work;

    Message_ShowNotice(0, 0, 0, 0, 0x2D, work[1]);
    work[2] = 0x2D;
    MAP_OBJECT_BYTES(object)->step = 4;
}

/* Step 4 of kind 0xB: back to step 1 when the 45 frames are over. */
void MapObject_WaitExaminedMessage(MapObject *object) {
    s32 *work = object->work;

    if (--work[2] <= 0) {
        MAP_OBJECT_BYTES(object)->step = 1;
    }
}

/* Step 0 of the symbols of passage (kind 0xC): step 1, where nothing happens; the wands look for them
   (Player_UseItem). */
void MapObject_StartSymbolOfPassage(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = 1;
}

/* Does nothing: step 1 of the symbols of passage. */
void func_00120ED8(void) {
}
