/* Maps: the per-map slot table and the map's event triggers. */
#ifndef KF_MAP_H
#define KF_MAP_H

#include "common.h"
#include "kf_model.h"

/* a map's flag block, MapSlot.flag_blocks[n] */
typedef struct {
    u8 bits[0x20];                  /* 0x00 one bit per flag of this map */
    u8 tags[0x10];                  /* 0x20 other maps' tags... */
    u8 values[0x10];                /* 0x30 ...and the flag set for each */
} MapFlagBlock;

/* one 0x60-byte entry of D_0038CD20, indexed by the current map/slot D_00446770 */
typedef struct {
    u8 unk00;                       /* 0x00 */
    u8 tag;                         /* 0x01 matched against MapFlagBlock.tags */
    char pad02[0x06];
    void *unk08;                    /* 0x08 */
    char pad0C[0x04];
    void *unk10;                    /* 0x10 */
    char *records;                  /* 0x14 0x80-byte records, by PlayerState.map_index */
    MapFlagBlock *flag_blocks;      /* 0x18 */
    Vector4 *origins;               /* 0x1C by EntityController.unk43A */
    char pad20[0x40];
} MapSlot;

/* the trigger's one-shot state, MapTrigger+0x40 */
typedef struct {
    int unk00[6];                   /* 0x00 */
    int fired;                      /* 0x18 the type it fired for, 0 before */
} MapTriggerShot;

/* what MapTrigger.unk08 points at, as func_0011F0D8 reads it */
typedef struct {
    char pad00[0x44];
    u16 unk44;                      /* 0x44 */
} TriggerSource;

/* a map event trigger (func_00122538) */
typedef struct {
    char pad00[0x04];
    u8 *unk04;                      /* 0x04 */
    char *unk08;                    /* 0x08 TriggerSource; byte 0x3A is passed to func_00103BA8 */
    char pad0C[0x04];
    u8 unk10;                       /* 0x10 bit 0 picks the MapSlot */
    u8 type;                        /* 0x11 */
    char pad12[0x2E];
    MapTriggerShot shot;            /* 0x40 */
} MapTrigger;

#define MAP_TRIGGER_SOURCE(trigger) ((TriggerSource *)(trigger)->unk08)

/* D_0041A6F8[]: 0x20-byte definitions whose first byte is a trigger type */
typedef struct {
    u8 type;                        /* 0x00 */
    char pad01[0x1F];
} TriggerTypeDef;

#endif
