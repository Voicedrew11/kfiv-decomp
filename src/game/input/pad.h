#ifndef KF_PAD_H
#define KF_PAD_H

#include "common.h"
#include "core/game.h"

/* PadSlot.features */
#define PAD_FEATURE_MODE_SWITCH 0x01 /* can switch between digital and analog */
#define PAD_FEATURE_ACTUATORS 0x02   /* has rumble motors */
#define PAD_FEATURE_PRESSURE 0x04    /* pressure-sensitive buttons */

/* PadSlot.mode and PadSlot.mode_request */
#define PAD_MODE_DIGITAL 0
#define PAD_MODE_ANALOG 1
#define PAD_MODE_PRESSURE 2 /* analog with pressure-sensitive buttons */
#define PAD_MODE_NONE 0xFF  /* mode: not known yet; mode_request: nothing asked */

/* the digital pad word Pad_Read builds: a set bit is a button that is down (SELECT first, SQUARE last) */
#define PAD_SELECT   0x0001
#define PAD_L3       0x0002
#define PAD_R3       0x0004
#define PAD_START    0x0008
#define PAD_UP       0x0010
#define PAD_RIGHT    0x0020
#define PAD_DOWN     0x0040
#define PAD_LEFT     0x0080
#define PAD_L2       0x0100
#define PAD_R2       0x0200
#define PAD_L1       0x0400
#define PAD_R1       0x0800
#define PAD_TRIANGLE 0x1000
#define PAD_CIRCLE   0x2000
#define PAD_CROSS    0x4000
#define PAD_SQUARE   0x8000
#define PAD_DIRECTIONS (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)

/* what Pad_GetReport returns: the controller's own report as scePadRead gave it, then the
   buttons Pad_Update worked out */
typedef struct {
    u8 status;                      /* 0x00 0 when the read succeeded */
    u8 type_and_length;             /* 0x01 controller type in the high nibble, half the data length in the low */
    u16 held;                       /* 0x02 a set bit is a pressed button */
    u8 right_stick_x;               /* 0x04 the four axes, 0x80 at rest */
    u8 right_stick_y;               /* 0x05 */
    u8 left_stick_x;                /* 0x06 */
    u8 left_stick_y;                /* 0x07 */
    char pad08[0x18];
    u16 pressed;                    /* 0x20 buttons that went down this frame */
    u16 repeated;                   /* 0x22 buttons that fired through auto-repeat */
} PadReport;

/* PadReport's axis bytes two at a time: zero when the controller sends none */
typedef struct {
    char pad00[0x04];
    u16 right_stick;                /* 0x04 */
    u16 left_stick;                 /* 0x06 */
} PadReportAxes;

/* PadReport.pressed inside a union: a read through it may alias any store, so it stays behind one
   (Title_UpdateMenu and Title_UpdateGameOver only match this way) */
typedef union {
    u16 buttons;
    u8 bytes[2];
} PadButtonsUnion;

typedef struct {
    char pad00[0x20];
    PadButtonsUnion pressed;        /* 0x20 */
} PadReportUnion;

/* auto-repeat timing of one button of a PadSlot (set by Pad_SetRepeat, stepped by Pad_StepRepeat) */
typedef struct {
    u8 delay;                       /* 0x00 frames before the first repeat */
    u8 interval;                    /* 0x01 frames between repeats */
    u8 countdown;                   /* 0x02 */
    u8 fired;                       /* 0x03 0 none, 1 first press, 2 repeat */
} PadRepeatKey;

/* one controller (0x1C0 bytes), kept up to date by Pad_Update */
typedef struct {
    u8 dma_buffer[0x100];           /* 0x000 work area handed to scePadPortOpen */
    PadReport report;               /* 0x100 the 0x24 bytes scePadRead gave (Pad_Read) */
    PadRepeatKey repeat_keys[16];   /* 0x124 */
    u16 repeat_mask;                /* 0x164 buttons that auto-repeat */
    char pad166[0x02];
    int state;                      /* 0x168 scePadGetState */
    int request_state;              /* 0x16C scePadGetReqState */
    int actuator_count;             /* 0x170 scePadInfoAct(-1) */
    u8 actuators[6];                /* 0x174 [0] small motor on/off, [1] big motor power */
    u8 phase;                       /* 0x17A step of the set-up state machine in Pad_Update */
    u8 next_phase;                  /* 0x17B phase to go to after the actuators are aligned */
    u8 pad_type;                    /* 0x17C report.type_and_length >> 4 */
    u8 features;                    /* 0x17D PAD_FEATURE_* */
    u8 mode;                        /* 0x17E PAD_MODE_* */
    u8 mode_request;                /* 0x17F PAD_MODE_* asked for, PAD_MODE_NONE: none */
    u8 mode_lock;                   /* 0x180 passed to scePadSetMainMode */
    u8 actuators_aligned;           /* 0x181 */
    char pad182[0x3E];
} PadSlot;

/* two ports with four multitap slots each */
typedef struct {
    PadSlot slots[8];               /* 0x000 index is port * 4 + slot */
    u8 open_slots[2];               /* 0xE00 per port, bit n: slot n was opened */
} PadSystem;

int Pad_Init(int mode);
int Pad_OpenPort(int port, int slot);
void Pad_Update(void);
int Pad_GetState(int port, int slot);
int Pad_GetRequestState(int port, int slot);
PadReport *Pad_GetReport(int port, int slot);
int Pad_GetButtons(int port, int slot);
int Pad_GetPressed(int port, int slot);
int Pad_GetRepeated(int port, int slot);
void Pad_SetRepeat(int port, int slot, u16 buttons, u16 delay, u16 interval);
void Pad_ClearRepeat(int port, int slot, u16 buttons);
void Pad_SetActuators(int port, int slot, u8 *values);
void Pad_SetRumble(int port, int slot, int power);
void Pad_RequestDigital(int port, int slot, int lock);
int Pad_RequestAnalog(int port, int slot, int lock);
int Pad_RequestPressure(int port, int slot);
int Pad_GetMode(int port, int slot);

/* input/pad_state.c */
extern PadState pad_states[2];
extern u16 pad_pressed[];

#endif
