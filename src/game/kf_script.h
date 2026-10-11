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

/* blends two floats toward target_a and target_b over `frames` (func_002134F8) */
typedef struct {
    int active;                     /* 0x00 */
    ScriptFloatArg target_a;        /* 0x04 value at 0x08 */
    ScriptFloatArg target_b;        /* 0x0C value at 0x10 */
    ScriptArg frames;               /* 0x14 value at 0x18 */
} ScriptBlendCommand;

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

#endif
