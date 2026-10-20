#include "core/game.h"
#include "model/model.h"
#include "ui/splash.h"
#include "ui/message.h"
#include "map_object/map_object.h"

/* MapObject.step of the things only examined (kind 0xB) */
#define EXAMINABLE_STEP_IDLE        1       /* waiting for the player to use it */
#define EXAMINABLE_STEP_SHOW_SIGN   2       /* shows its sign picture */
#define EXAMINABLE_STEP_SHOW_MESSAGE 3      /* shows its message */
#define EXAMINABLE_STEP_WAIT_MESSAGE 4      /* the message stays up for 45 frames */
/* MapObject.step of a symbol of passage (kind 0xC) */
#define SYMBOL_OF_PASSAGE_STEP_IDLE 1       /* waiting for the wands to look for it */

/* Step 0 of the things that are only examined (kind 0xB): takes the message (the definition's name when the
   source gives none) and, from the source, whether a sign picture goes with it; step 1. */
void MapObject_StartExaminable(MapObject *object) {
    s32 *work = object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);

    work[1] = source->kind_param_0;
    if (work[1] == 0) {
        work[1] = (s16)object->definition->definition_flags;
    }
    object->work[0] = source->kind_param_1;
    MAP_OBJECT_BYTES(object)->step = EXAMINABLE_STEP_IDLE;
}

/* Step 1 of kind 0xB: used by the player, it shows its sign picture (step 2) when it has one, else its
   message (step 3). */
void MapObject_UpdateExaminable(MapObject *object) {
    MapObject *used;

    if (GAME_STATUS_FLAG_BITS(game_status_flags)->using_faced_object && (used = faced_object[0]) == object) {
        if (used->work[0] != 0) {
            MAP_OBJECT_BYTES(used)->step = EXAMINABLE_STEP_SHOW_SIGN;
        } else {
            MAP_OBJECT_BYTES(used)->step = EXAMINABLE_STEP_SHOW_MESSAGE;
        }
    }
}

/* Step 2 of kind 0xB: back to step 1 once Splash_Start has taken the sign picture (msg/*.tm2) of the
   message's number. */
void MapObject_ShowExaminedSign(MapObject *object) {
    if (Splash_Start(object->work[1])) {
        MAP_OBJECT_BYTES(object)->step = EXAMINABLE_STEP_IDLE;
    }
}

/* Step 3 of kind 0xB: shows the message for 45 frames; step 4. */
void MapObject_ShowExaminedMessage(MapObject *object) {
    s32 *work = object->work;

    Message_ShowNotice(0, 0, 0, 0, 0x2D, work[1]);
    work[2] = 0x2D;
    MAP_OBJECT_BYTES(object)->step = EXAMINABLE_STEP_WAIT_MESSAGE;
}

/* Step 4 of kind 0xB: back to step 1 when the 45 frames are over. */
void MapObject_WaitExaminedMessage(MapObject *object) {
    s32 *work = object->work;

    if (--work[2] <= 0) {
        MAP_OBJECT_BYTES(object)->step = EXAMINABLE_STEP_IDLE;
    }
}

/* Step 0 of the symbols of passage (kind 0xC): step 1, where nothing happens; the wands look for them
   (Player_UseItem). */
void MapObject_StartSymbolOfPassage(MapObject *object) {
    MAP_OBJECT_BYTES(object)->step = SYMBOL_OF_PASSAGE_STEP_IDLE;
}

/* Does nothing: the step a symbol of passage stays in once started (the wands look for it). */
void MapObject_IdleSymbolOfPassage(MapObject *object) {
}
