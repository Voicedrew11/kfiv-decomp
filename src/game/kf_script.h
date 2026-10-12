/* Script commands: records that start with an active word followed by 8-byte
   arguments (an opcode word and its value). A command stays active, stepping
   once a frame, until it clears `active`. */
#ifndef KF_SCRIPT_H
#define KF_SCRIPT_H

#include "common.h"

/* one argument: what func_00211CF8/func_00211D90 read (opcode, param) */
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

/* moves or turns an object toward/by x, y, z over `frames` (func_00212B00,
   func_00213090, func_00214080, func_00215850) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C value at 0x10 */
    ScriptFloatArg y;               /* 0x14 value at 0x18 */
    ScriptFloatArg z;               /* 0x1C value at 0x20 */
    ScriptArg frames;               /* 0x24 value at 0x28 */
    ScriptArg unk2C;                /* 0x2C value at 0x30 */
    ScriptArg unk34;                /* 0x34 value at 0x38 */
} ScriptMoveCommand;

/* plays a motion on an object for `frames` (func_002151C0) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg frames;               /* 0x0C value at 0x10 */
    ScriptArg motion;               /* 0x14 value at 0x18 */
} ScriptMotionCommand;

/* calls a map object routine with two integer arguments (func_00212CB8, func_00213320) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg first;                /* 0x0C value at 0x10 */
    ScriptArg second;               /* 0x14 value at 0x18 */
} ScriptCallCommand;

/* blends two floats toward target_a and target_b over `frames` (func_002134F8) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg target_a;        /* 0x04 value at 0x08 */
    ScriptFloatArg target_b;        /* 0x0C value at 0x10 */
    ScriptArg frames;               /* 0x14 value at 0x18 */
} ScriptBlendCommand;

/* eases three floats toward the shared target over `frames` (func_00213BA8) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg x;               /* 0x04 value at 0x08 */
    ScriptFloatArg y;               /* 0x0C value at 0x10 */
    ScriptFloatArg z;               /* 0x14 value at 0x18 */
    ScriptArg frames;               /* 0x1C value at 0x20 */
} ScriptTripleCommand;

/* sets a flag bit on the current map's entities (func_00215A18) */
typedef struct {
    s32 active;                     /* 0x00 cleared when handled */
    char pad04[0x04];
    s32 kind;                       /* 0x08 1: only unflagged, 2: only flagged */
    char pad0C[0x04];
    u32 value;                      /* 0x10 bit 0 is copied */
} ScriptFlagCommand;

/* a command with an id and up to two points (func_00213868, func_00213A60,
   func_00214130, func_00214EE8) */
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

/* starts something with `id`, then waits for it to finish (func_002120E0) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg object_id;            /* 0x04 value at 0x08 */
    char pad0C[0x78];
    int started;                    /* 0x84 */
} ScriptWaitCommand;

/* acts on one object (func_002166F8) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg object_id;            /* 0x04 value at 0x08 */
} ScriptObjectCommand;

/* runs func_001C8CB0 on an object with its spawn_info->unk38 set to `value`
   (func_00212690) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptShortArg value;           /* 0x04 value at 0x08 */
    ScriptArg object_id;            /* 0x0C value at 0x10 */
} ScriptSetValueCommand;

/* stores the distance between points a and b in `result` (func_00215FE0) */
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

/* stores the angle of (x, y) in `angle` (func_00214FF0) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg x;               /* 0x04 value at 0x08 */
    ScriptFloatArg y;               /* 0x0C value at 0x10 */
    ScriptArg angle;                /* 0x14 value at 0x18 */
} ScriptAngleCommand;

/* stores the distance between points a and b in `result`, ignoring z (func_00215F50) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg a_x;             /* 0x04 value at 0x08 */
    ScriptFloatArg a_y;             /* 0x0C value at 0x10 */
    ScriptFloatArg b_x;             /* 0x14 value at 0x18 */
    ScriptFloatArg b_y;             /* 0x1C value at 0x20 */
    ScriptArg result;               /* 0x24 value at 0x28 */
} ScriptDistance2DCommand;

/* shakes the controller (func_00214C20) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg strength;             /* 0x04 value at 0x08 */
    ScriptArg duration;             /* 0x0C value at 0x10 */
    ScriptArg delay;                /* 0x14 value at 0x18 */
} ScriptShakeCommand;

/* stores something worked out from `input` in `result` (func_00214C58, func_00215040,
   func_00216660) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg input;                /* 0x04 value at 0x08 */
    ScriptArg result;               /* 0x0C value at 0x10 */
} ScriptLookupCommand;

/* stores something worked out from `input` in `result` (func_00214C58, func_00215040,
   func_00216660, func_00212040) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg input;                /* 0x04 value at 0x08 */
    ScriptArg result;               /* 0x0C value at 0x10 */
} ScriptQueryCommand;

/* sends an object walking to a point until it is within `distance`, turning and
   climbing as needed (func_00215520) */
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

/* the same without the height (func_002156F0) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptFloatArg x;               /* 0x0C value at 0x10 */
    ScriptFloatArg z;               /* 0x14 value at 0x18 */
    ScriptFloatArg distance;        /* 0x1C value at 0x20 */
    ScriptArg mode;                 /* 0x24 value at 0x28: goes into Entity flag bits 11-13 */
    ScriptArg run;                  /* 0x2C value at 0x30 */
} ScriptWalkToFlatCommand;

/* puts an object in state 4 and waits `frames` and for state 5 (func_00215310) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg frames;               /* 0x0C value at 0x10 */
    char pad14[0x70];
    int step;                       /* 0x84 */
} ScriptStateWaitCommand;

/* sets the object's flags picked by `mask` from the bits of `value` (func_00216088; the
   sixteen bits are the same ones func_00216408 reads back) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg value;                /* 0x0C value at 0x10 */
    ScriptArg mask;                 /* 0x14 value at 0x18 */
} ScriptSetFlagsCommand;

/* shows a message (func_00215118) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg message_id;           /* 0x04 value at 0x08 */
    ScriptArg font;                 /* 0x0C value at 0x10 */
    ScriptArg kind;                 /* 0x14 value at 0x18 */
    ScriptArg frames;               /* 0x1C value at 0x20 */
} ScriptMessageCommand;

/* stores in `result` whether an object's turn is within `range` degrees (func_00215AF8) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg target_id;            /* 0x04 value at 0x08 */
    ScriptArg range;                /* 0x0C value at 0x10 */
    ScriptArg result;               /* 0x14 value at 0x18 */
} ScriptFacingCommand;

/* one running command of a script thread: 0xA4 bytes, the Script*Command structs are
   views of it */
typedef struct {
    int active;                     /* 0x00 the command's number, 0: free or finished */
    ScriptArg args[20];             /* 0x04 */
} ScriptThreadCommand;

/* the same 0xC4C bytes as ScriptSlot, with the parts named: what the interpreter
   (func_002109C8 to func_00211BC0) works on */
typedef struct {
    u16 script_id;                  /* 0x00 0: free */
    char pad02[0x02];
    u32 *cursor;                    /* 0x04 next word of the script: opcode << 16 | operand */
    int argument;                   /* 0x08 what the starter passed */
    ScriptThreadCommand commands[16]; /* 0x0C */
    u8 locals[0x200];               /* 0xA4C the script's own variables */
} ScriptThread;

/* an argument as the interpreter's own locals hold it (func_002111F0, func_002114D8):
   16 bytes apart on the stack, read as an int or as a float by its type (6: float) */
typedef union {
    ScriptArg as_int;
    ScriptFloatArg as_float;
} ScriptValue __attribute__((aligned(16)));

/* D_01DFB3B0[24]: a script slot, 0xC4C bytes. `active` is nonzero while it runs; the
   commands (ScriptMoveCommand and the like) sit in the area from 0x0C. */
typedef struct {
    u16 active;                     /* 0x00 */
    char pad02[0x02];
    s32 unk04;                      /* 0x04 */
    char pad08[0x04];
    u8 commands[0xC40];             /* 0x0C */
} ScriptSlot;

/* shakes the camera: CameraState.unk70 and unk74 (func_00213C78) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg reach;           /* 0x04 value at 0x08 */
    ScriptArg frames;               /* 0x0C value at 0x10 */
} ScriptCameraShakeCommand;

/* sets one of the three lights of a block of the current map's MapSlot.unk10: its direction
   and its colour, 0 to 255 (func_00214358) */
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

/* hands two points to func_0019DAA8 (func_00214700) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg unk04;                /* 0x04 value at 0x08, passed as a char */
    ScriptArg unk0C;                /* 0x0C value at 0x10, passed as a u8 */
    ScriptFloatArg unk14;           /* 0x14 value at 0x18 */
    ScriptFloatArg from_x;          /* 0x1C value at 0x20 */
    ScriptFloatArg from_y;          /* 0x24 value at 0x28 */
    ScriptFloatArg from_z;          /* 0x2C value at 0x30 */
    ScriptFloatArg to_x;            /* 0x34 value at 0x38 */
    ScriptFloatArg to_y;            /* 0x3C value at 0x40 */
    ScriptFloatArg to_z;            /* 0x44 value at 0x48 */
} ScriptTwoPointCommand;

/* sets the colour func_0010A4E8 covers the screen with: MapDrawStateOverlays.unk6C to unk80
   (func_00214B88) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg red;                  /* 0x04 value at 0x08 */
    ScriptArg green;                /* 0x0C value at 0x10 */
    ScriptArg blue;                 /* 0x14 value at 0x18 */
    ScriptArg alpha;                /* 0x1C value at 0x20 */
    ScriptArg blend;                /* 0x24 value at 0x28, 0 to 2 */
} ScriptScreenColorCommand;

/* a command with one integer argument (func_00211E38, func_00212248, func_00212450) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg value;                /* 0x04 value at 0x08 */
} ScriptValueCommand;

/* a command with one float argument (func_00211FF0) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg value;           /* 0x04 value at 0x08 */
} ScriptFloatValueCommand;

/* a command with two integer arguments (func_002120A0, func_00212218, func_00212480) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptArg first;                /* 0x04 value at 0x08 */
    ScriptArg second;               /* 0x0C value at 0x10 */
} ScriptPairCommand;

/* a command whose only argument in use is the eighth, a result (func_00211F60, func_00211F90) */
typedef struct {
    int active;                     /* 0x00 */
    char pad04[0x38];
    ScriptArg result;               /* 0x3C */
} ScriptLateResultCommand;

#endif
