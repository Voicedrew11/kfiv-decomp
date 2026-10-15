/* The map script interpreter (kf_script.c) and its commands (kf_script.c, kf_script_world.c,
   kf_script_entity.c). A running command is a record that starts with its number (`active`) followed by
   8-byte arguments (an opcode word and its value); it stays active, stepping once a frame, until its
   handler clears `active`. */
#ifndef KF_SCRIPT_H
#define KF_SCRIPT_H

#include "common.h"

/* the control words of a script (the high half of a word; anything else starts a command) */
#define SCRIPT_WORD_LABEL           0x190   /* the start of the script with the id in the low half */
#define SCRIPT_WORD_END             0x191   /* the thread ends */
#define SCRIPT_WORD_IF              0x192   /* comparisons, then the words to skip when they fail */
#define SCRIPT_WORD_BEGIN_NO_WAIT   0x194   /* the commands up to the next 0x195 start together */
#define SCRIPT_WORD_END_NO_WAIT     0x195
#define SCRIPT_WORD_SET             0x196   /* a variable, then operands and operators */
#define SCRIPT_WORD_JUMP            0x197   /* the low half is a signed word count */
#define SCRIPT_WORD_HALT            0x19D   /* halts the thread: Script_HaltCursor returns a NULL cursor */
#define SCRIPT_LABEL_END            0x7FFE  /* the label after the last script */

/* the kinds of argument (the low byte of an argument's opcode word) */
#define SCRIPT_ARG_CONSTANT         0       /* the type is in the high half */
#define SCRIPT_ARG_GAME_VAR         1       /* a variable in one of the blocks of D_00433340 */
#define SCRIPT_ARG_LOCAL_VAR        2       /* one of the thread's own variables */
#define SCRIPT_ARG_SHARED_VAR       3       /* one of the variables all threads share */
#define SCRIPT_ARG_OUT_GAME_VAR     0x11    /* the same three, where a command writes its answer */
#define SCRIPT_ARG_OUT_LOCAL_VAR    0x12
#define SCRIPT_ARG_OUT_SHARED_VAR   0x13

/* ScriptArg.opcode of a value read from a variable: 0 s8, 1 u8, 2 s16, 3 u16, 4 and 5 32 bits, 6 float */
#define SCRIPT_TYPE_FLOAT           6

/* a command number with this bit set is not stepped by Script_StepCommands (only by the unused Script_StepHeldCommands) */
#define SCRIPT_COMMAND_HELD         0x8000

/* error flags, bytes of D_00432920 the interpreter sets to 1 */
#define SCRIPT_ERROR_NO_FREE_THREAD     0
#define SCRIPT_ERROR_NO_FREE_COMMAND    1
#define SCRIPT_ERROR_BAD_ARGUMENT       2
#define SCRIPT_ERROR_BAD_INT_TARGET     3
#define SCRIPT_ERROR_BAD_FLOAT_TARGET   4
#define SCRIPT_ERROR_BAD_SHARED_VAR     10
#define SCRIPT_ERROR_BAD_LOCAL_VAR      11

/* one argument of a command: its opcode word and its value. An argument a command answers through keeps
   the opcode word of the variable to write (kinds 0x11 to 0x13, see Script_StoreResult). */
typedef struct {
    int opcode;
    int value;
} ScriptArg;

/* an argument whose value is a float */
typedef struct {
    int opcode;
    f32 value;
} ScriptFloatArg;

/* an argument whose value is a u16 */
typedef struct {
    int opcode;
    u16 value;
} ScriptShortArg;

/* reads a float's bits as an int */
typedef union {
    f32 as_float;
    int as_int;
} FloatBits;

/* one running command of a script thread: 0xA4 bytes, the Script*Command structs are views of it. The
   first 16 arguments come from the script, the four after them are the handler's own (cleared when the
   command starts). */
typedef struct {
    int active;                     /* 0x00 the command's number, 0: free or finished */
    ScriptArg args[20];             /* 0x04 */
} ScriptThreadCommand;

/* what a command's handler is called with */
typedef void (*ScriptCommandHandler)(ScriptThreadCommand *command);

/* a script thread (0xC4C bytes, 24 at D_01DFB3B0) */
typedef struct {
    u16 script_id;                  /* 0x00 0: free */
    char pad02[0x02];
    u32 *cursor;                    /* 0x04 next word of the script: opcode << 16 | operand */
    int argument;                   /* 0x08 what the starter passed */
    ScriptThreadCommand commands[16]; /* 0x0C */
    u8 locals[0x200];               /* 0xA4C the script's own variables */
} ScriptThread;

/* an argument as the interpreter's own locals hold it (Script_EvalIf, Script_EvalSet):
   16 bytes apart on the stack, read as an int or as a float by its type (6: float) */
typedef union {
    ScriptArg as_int;
    ScriptFloatArg as_float;
} ScriptValue __attribute__((aligned(16)));

/* a command with one integer argument (ScriptCmd_Wait and many more) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg value;                /* 0x04 value at 0x08 */
} ScriptValueCommand;

/* a command with one float argument (ScriptCmd_SetCameraUnk48) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg value;           /* 0x04 value at 0x08 */
} ScriptFloatValueCommand;

/* a command with two integer arguments (ScriptCmd_SetRecollectionPage, ScriptCmd_MoveRecollectionPage,
   ScriptCmd_StartItemPickup) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg first;                /* 0x04 value at 0x08 */
    ScriptArg second;               /* 0x0C value at 0x10 */
} ScriptPairCommand;

/* a command that works something out from `input` and hands it back through `result` */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg input;                /* 0x04 value at 0x08 */
    ScriptArg result;               /* 0x0C */
} ScriptQueryCommand;

/* the same with a float input (ScriptCmd_Sin, ScriptCmd_Cos, ScriptCmd_RandomFloat) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg input;           /* 0x04 value at 0x08 */
    ScriptArg result;               /* 0x0C */
} ScriptFloatQueryCommand;

/* a command whose only argument in use is the eighth, a result (ScriptCmd_StoreUninitializedResult9,
   ScriptCmd_StoreUninitializedResult10) */
typedef struct {
    int active;                     /* 0x00 */
    char pad04[0x38];
    ScriptArg result;               /* 0x3C */
} ScriptLateResultCommand;

/* command 2: starts a script and waits for it (ScriptCmd_RunScript) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg script_id;            /* 0x04 value at 0x08 */
    ScriptArg no_wait;              /* 0x0C value at 0x10: 1 finishes at once */
    char pad14[0x70];
    int started;                    /* 0x84 */
    int thread_index;               /* 0x88 the thread the script runs in */
} ScriptRunScriptCommand;

/* starts something with `object_id`, then waits for it to finish (ScriptCmd_ShowSplash) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg object_id;            /* 0x04 value at 0x08 */
    char pad0C[0x78];
    int started;                    /* 0x84 */
} ScriptWaitCommand;

/* shows a splash image with timing args until it finishes (ScriptCmd_ShowSplashTimed) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg splash_id;            /* 0x04 value at 0x08 */
    ScriptArg hold_frames;          /* 0x0C value at 0x10 */
    ScriptArg skippable;            /* 0x14 value at 0x18 */
    ScriptArg fade_in_frames;       /* 0x1C value at 0x20 */
    ScriptArg fade_out_frames;      /* 0x24 value at 0x28 */
    char pad2C[0x58];
    int started;                    /* 0x84 */
} ScriptSplashCommand;

/* acts on one object (ScriptCmd_ActivateMapObject and many more) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg object_id;            /* 0x04 value at 0x08 */
} ScriptObjectCommand;

/* an object and two integer values for it (ScriptCmd_PlayMapObjectAnimRunning, ScriptCmd_PlayMapObjectAnim) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg first;                /* 0x0C value at 0x10 */
    ScriptArg second;               /* 0x14 value at 0x18 */
} ScriptCallCommand;

/* an object and four integer values for it (ScriptCmd_SetMapObjectColor, ScriptCmd_GetMapObjectParams) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg values[4];            /* 0x0C values at 0x10, 0x18, 0x20, 0x28 */
} ScriptObjectValuesCommand;

/* the same with u16 values (ScriptCmd_SetMapObjectParams) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptShortArg values[4];       /* 0x0C values at 0x10, 0x18, 0x20, 0x28 */
} ScriptObjectShortsCommand;

/* makes an entity drop an item (ScriptCmd_DropItemFromEntity) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptShortArg value;           /* 0x04 value at 0x08: the item */
    ScriptArg object_id;            /* 0x0C value at 0x10 */
} ScriptSetValueCommand;

/* moves or turns an object toward or by x, y, z over `frames` (ScriptCmd_MoveMapObjectTo, ScriptCmd_MoveMapObjectBy,
   ScriptCmd_TurnPropBy, ScriptCmd_TurnEntityBy and more); a command that answers puts the object's x, y, z there */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C value at 0x10 */
    ScriptFloatArg y;               /* 0x14 value at 0x18 */
    ScriptFloatArg z;               /* 0x1C value at 0x20 */
    ScriptArg frames;               /* 0x24 value at 0x28 */
    ScriptArg animate;              /* 0x2C value at 0x30: nonzero, the entity's motion plays on (ScriptCmd_TurnEntityBy, ScriptCmd_TurnEntityToward) */
    ScriptArg motion;               /* 0x34 value at 0x38: what Motion_Step is handed then */
} ScriptMoveCommand;

/* eases three floats toward x, y, z over `frames` (ScriptCmd_MovePlayerTo, ScriptCmd_MoveCameraTo, ScriptCmd_TurnCameraTo) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg x;               /* 0x04 value at 0x08 */
    ScriptFloatArg y;               /* 0x0C value at 0x10 */
    ScriptFloatArg z;               /* 0x14 value at 0x18 */
    ScriptArg frames;               /* 0x1C value at 0x20 */
} ScriptTripleCommand;

/* eases two floats toward target_a and target_b over `frames` (ScriptCmd_TurnPlayerTo) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg target_a;        /* 0x04 value at 0x08 */
    ScriptFloatArg target_b;        /* 0x0C value at 0x10 */
    ScriptArg frames;               /* 0x14 value at 0x18 */
} ScriptBlendCommand;

/* an id and up to two points (ScriptCmd_StartEffect, ScriptCmd_LaunchShot, ScriptCmd_SetCameraLookAt, ScriptCmd_GetMapObjectPosition, ScriptCmd_GetPropPosition) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg object_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg from_x;          /* 0x0C value at 0x10 */
    ScriptFloatArg from_y;          /* 0x14 value at 0x18 */
    ScriptFloatArg from_z;          /* 0x1C value at 0x20 */
    ScriptFloatArg to_x;            /* 0x24 value at 0x28 */
    ScriptFloatArg to_y;            /* 0x2C value at 0x30 */
    ScriptFloatArg to_z;            /* 0x34 value at 0x38 */
} ScriptPointCommand;

/* shakes the camera: CameraState.unk70 and unk74 (ScriptCmd_ShakeCamera) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg reach;           /* 0x04 value at 0x08 */
    ScriptArg frames;               /* 0x0C value at 0x10 */
} ScriptCameraShakeCommand;

/* sets the fog of a block of the current map's MapSlot.unk0C (ScriptCmd_SetMapFog) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg block;                /* 0x04 value at 0x08, below 32 */
    ScriptFloatArg near_value;      /* 0x0C value at 0x10 */
    ScriptFloatArg far_value;       /* 0x14 value at 0x18 */
    ScriptFloatArg near;            /* 0x1C value at 0x20 */
    ScriptFloatArg far;             /* 0x24 value at 0x28 */
    ScriptArg red;                  /* 0x2C value at 0x30 */
    ScriptArg green;                /* 0x34 value at 0x38 */
    ScriptArg blue;                 /* 0x3C value at 0x40 */
} ScriptFogCommand;

/* sets one of the three lights of a block of the current map's MapSlot.unk10: its direction
   and its colour, 0 to 255 (ScriptCmd_SetMapLight) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg block;                /* 0x04 value at 0x08, below 32 */
    ScriptArg light;                /* 0x0C value at 0x10, 0 to 2 */
    ScriptFloatArg dir_x;           /* 0x14 value at 0x18 */
    ScriptFloatArg dir_y;           /* 0x1C value at 0x20 */
    ScriptFloatArg dir_z;           /* 0x24 value at 0x28 */
    ScriptArg red;                  /* 0x2C value at 0x30 */
    ScriptArg green;                /* 0x34 value at 0x38 */
    ScriptArg blue;                 /* 0x3C value at 0x40 */
} ScriptLightCommand;

/* sets a colour of a light block, 0 to 255 (ScriptCmd_SetMapAmbientLight) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg block;                /* 0x04 value at 0x08, below 32 */
    ScriptArg red;                  /* 0x0C value at 0x10 */
    ScriptArg green;                /* 0x14 value at 0x18 */
    ScriptArg blue;                 /* 0x1C value at 0x20 */
} ScriptColorCommand;

/* a sound at a point (ScriptCmd_PlaySoundAt) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg unk04;                /* 0x04 value at 0x08, not read */
    ScriptArg sound;                /* 0x0C value at 0x10 */
    ScriptFloatArg unk14;           /* 0x14 value at 0x18, not read */
    ScriptFloatArg x;               /* 0x1C value at 0x20 */
    ScriptFloatArg y;               /* 0x24 value at 0x28 */
    ScriptFloatArg z;               /* 0x2C value at 0x30 */
} ScriptSoundAtCommand;

/* a sound at a point, with a mode and a range (ScriptCmd_PlaySoundAtPoint) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg sound;                /* 0x04 value at 0x08 */
    ScriptArg mode;                 /* 0x0C value at 0x10 */
    ScriptFloatArg range;           /* 0x14 value at 0x18 */
    ScriptFloatArg x;               /* 0x1C value at 0x20 */
    ScriptFloatArg y;               /* 0x24 value at 0x28 */
    ScriptFloatArg z;               /* 0x2C value at 0x30 */
} ScriptPointSoundCommand;

/* a sound on a voice at a point (ScriptCmd_PlayVoiceSoundAt) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg sound;                /* 0x04 value at 0x08 */
    ScriptArg voice;                /* 0x0C value at 0x10 */
    ScriptFloatArg volume;          /* 0x14 value at 0x18 */
    ScriptFloatArg x;               /* 0x1C value at 0x20 */
    ScriptFloatArg y;               /* 0x24 value at 0x28 */
    ScriptFloatArg z;               /* 0x2C value at 0x30 */
} ScriptVoiceSoundCommand;

/* the same with a mode (ScriptCmd_PlayVoiceSoundAtWithMode) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg sound;                /* 0x04 value at 0x08 */
    ScriptArg mode;                 /* 0x0C value at 0x10 */
    ScriptArg voice;                /* 0x14 value at 0x18 */
    ScriptFloatArg volume;          /* 0x1C value at 0x20 */
    ScriptFloatArg x;               /* 0x24 value at 0x28 */
    ScriptFloatArg y;               /* 0x2C value at 0x30 */
    ScriptFloatArg z;               /* 0x34 value at 0x38 */
} ScriptVoiceSoundModeCommand;

/* a sound on a voice that moves between two points (ScriptCmd_PlayMovingSound) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg sound;                /* 0x04 value at 0x08 */
    ScriptArg voice;                /* 0x0C value at 0x10 */
    ScriptFloatArg volume;          /* 0x14 value at 0x18 */
    ScriptFloatArg from_x;          /* 0x1C value at 0x20 */
    ScriptFloatArg from_y;          /* 0x24 value at 0x28 */
    ScriptFloatArg from_z;          /* 0x2C value at 0x30 */
    ScriptFloatArg to_x;            /* 0x34 value at 0x38 */
    ScriptFloatArg to_y;            /* 0x3C value at 0x40 */
    ScriptFloatArg to_z;            /* 0x44 value at 0x48 */
} ScriptMovingSoundCommand;

/* the same with a mode (ScriptCmd_PlayMovingSoundWithMode) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg sound;                /* 0x04 value at 0x08 */
    ScriptArg mode;                 /* 0x0C value at 0x10 */
    ScriptArg voice;                /* 0x14 value at 0x18 */
    ScriptFloatArg volume;          /* 0x1C value at 0x20 */
    ScriptFloatArg from_x;          /* 0x24 value at 0x28 */
    ScriptFloatArg from_y;          /* 0x2C value at 0x30 */
    ScriptFloatArg from_z;          /* 0x34 value at 0x38 */
    ScriptFloatArg to_x;            /* 0x3C value at 0x40 */
    ScriptFloatArg to_y;            /* 0x44 value at 0x48 */
    ScriptFloatArg to_z;            /* 0x4C value at 0x50 */
} ScriptMovingSoundModeCommand;

/* a position read as four floats and copied as two doublewords (ScriptCmd_PlaySoundAt) */
typedef union {
    f32 floats[4];
    s64 halves[2];
} ScriptPosition;

/* sets the colour the screen is covered with: MapDrawStateOverlays.unk6C to unk80 (ScriptCmd_SetScreenColor) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg red;                  /* 0x04 value at 0x08 */
    ScriptArg green;                /* 0x0C value at 0x10 */
    ScriptArg blue;                 /* 0x14 value at 0x18 */
    ScriptArg alpha;                /* 0x1C value at 0x20 */
    ScriptArg blend;                /* 0x24 value at 0x28, 0 to 2 */
} ScriptScreenColorCommand;

/* makes the controller rumble (ScriptCmd_RumblePad) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg strength;             /* 0x04 value at 0x08 */
    ScriptArg duration;             /* 0x0C value at 0x10 */
    ScriptArg delay;                /* 0x14 value at 0x18 */
} ScriptShakeCommand;

/* a distance from the player, and the point it leads to (ScriptCmd_GetPointFromPlayer) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg distance;             /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C */
    ScriptFloatArg y;               /* 0x14 */
    ScriptFloatArg z;               /* 0x1C */
} ScriptPointAheadCommand;

/* hands back the angle of (x, y) through `angle` (ScriptCmd_Atan2) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg x;               /* 0x04 value at 0x08 */
    ScriptFloatArg y;               /* 0x0C value at 0x10 */
    ScriptArg angle;                /* 0x14 */
} ScriptAngleCommand;

/* opens the message box with a message (ScriptCmd_ShowMessageBox) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg message_id;           /* 0x04 value at 0x08 */
    ScriptArg font;                 /* 0x0C value at 0x10 */
    ScriptArg delay;                /* 0x14 value at 0x18 */
    char pad1C[0x68];
    int started;                    /* 0x84 */
} ScriptMessageBoxCommand;

/* puts a message up as a notice (ScriptCmd_ShowNotice) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg message_id;           /* 0x04 value at 0x08 */
    ScriptArg font;                 /* 0x0C value at 0x10 */
    ScriptArg kind;                 /* 0x14 value at 0x18 */
    ScriptArg frames;               /* 0x1C value at 0x20 */
} ScriptMessageCommand;

/* plays an entity's motion for `frames` (ScriptCmd_PlayEntityMotion) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg frames;               /* 0x0C value at 0x10 */
    ScriptArg motion;               /* 0x14 value at 0x18 */
} ScriptMotionCommand;

/* blends an entity's model into its motion by `weight` (ScriptCmd_BlendEntityMotion) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg weight;          /* 0x0C value at 0x10 */
} ScriptMotionBlendCommand;

/* turns an entity to face the player and waits for the turn (ScriptCmd_TurnEntityToPlayer) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg frames;               /* 0x0C value at 0x10 */
    char pad14[0x70];
    int step;                       /* 0x84 */
} ScriptStateWaitCommand;

/* walks an entity to a point until it is within `distance`, turning and climbing as needed
   (ScriptCmd_WalkEntityTo) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C value at 0x10 */
    ScriptFloatArg y;               /* 0x14 value at 0x18 */
    ScriptFloatArg z;               /* 0x1C value at 0x20 */
    ScriptFloatArg distance;        /* 0x24 value at 0x28 */
    ScriptArg mode;                 /* 0x2C value at 0x30: goes into Entity flag bits 11-13 */
    ScriptArg run;                  /* 0x34 value at 0x38 */
} ScriptWalkToCommand;

/* the same without the height (ScriptCmd_WalkEntityToFlat) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C value at 0x10 */
    ScriptFloatArg z;               /* 0x14 value at 0x18 */
    ScriptFloatArg distance;        /* 0x1C value at 0x20 */
    ScriptArg mode;                 /* 0x24 value at 0x28: goes into Entity flag bits 11-13 */
    ScriptArg run;                  /* 0x2C value at 0x30 */
} ScriptWalkToFlatCommand;

/* sets a flag bit on the current map's entities (ScriptCmd_SetEntitiesFlag5) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg kind;                 /* 0x04 value at 0x08: 1 only unflagged, 2 only flagged */
    ScriptArg value;                /* 0x0C value at 0x10: bit 0 is copied */
} ScriptFlagCommand;

/* hands back whether an entity's turn is within `range` degrees (ScriptCmd_IsEntityFacing) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg range;                /* 0x0C value at 0x10 */
    ScriptArg result;               /* 0x14 */
} ScriptFacingCommand;

/* reads or writes three floats of an entity (ScriptCmd_GetEntityPosition, ScriptCmd_GetEntityRotation, ScriptCmd_GetEntitySpawnPosition,
   ScriptCmd_GetEntitySpawnRotation, ScriptCmd_SetEntityPosition, ScriptCmd_SetEntityRotation) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C value at 0x10 */
    ScriptFloatArg y;               /* 0x14 value at 0x18 */
    ScriptFloatArg z;               /* 0x1C value at 0x20 */
} ScriptEntityVectorCommand;

/* hands back the distance between points a and b through `result` (ScriptCmd_Distance) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg a_x;             /* 0x04 value at 0x08 */
    ScriptFloatArg a_y;             /* 0x0C value at 0x10 */
    ScriptFloatArg a_z;             /* 0x14 value at 0x18 */
    ScriptFloatArg b_x;             /* 0x1C value at 0x20 */
    ScriptFloatArg b_y;             /* 0x24 value at 0x28 */
    ScriptFloatArg b_z;             /* 0x2C value at 0x30 */
    ScriptArg result;               /* 0x34 */
} ScriptDistanceCommand;

/* the same in the plane (ScriptCmd_Distance2D) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg a_x;             /* 0x04 value at 0x08 */
    ScriptFloatArg a_y;             /* 0x0C value at 0x10 */
    ScriptFloatArg b_x;             /* 0x14 value at 0x18 */
    ScriptFloatArg b_y;             /* 0x1C value at 0x20 */
    ScriptArg result;               /* 0x24 */
} ScriptDistance2DCommand;

/* sets the entity's flags picked by `mask` from the bits of `value` (ScriptCmd_SetEntityFlags; the
   sixteen bits are the same ones ScriptCmd_GetEntityFlags reads back) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg value;                /* 0x0C value at 0x10 */
    ScriptArg mask;                 /* 0x14 value at 0x18 */
} ScriptSetFlagsCommand;

/* Gets the interpreter ready for a map's script (the address as a word). */
void Script_Init(s32 script_data);
/* Starts a script in a free thread unless it is running already; returns the thread's index. */
int Script_Start(int script_id, int argument);
/* TRUE if a thread is running the script. */
int Script_IsRunning(int script_id);
/* Runs the script threads for a frame. */
int Script_UpdateAll(void);
/* Writes a command's answer to the variable the argument `target` names. The float form,
   Script_StoreFloatResult, is declared by its callers: they pass a double (see kf_script_world.c). */
int Script_StoreResult(ScriptArg *target, int value);

/* The command handlers are only reached through the handler tables D_004331B0, D_00433440 and
   D_00433650, which are still in the asm. */

#endif
