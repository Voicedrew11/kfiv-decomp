/* Entities: the live objects kept on singly linked lists. */
#ifndef KF_ENTITY_H
#define KF_ENTITY_H

#include "common.h"

#define ENTITY_DESPAWN_MARGIN           300.0f
#define ENTITY_VERTICAL_DESPAWN_MARGIN  150.0f

/* what Entity.spawn_info points at */
typedef struct {
    char pad00[0x04];
    f32 spawn_y;                    /* 0x04 */
    char pad08[0x40];
    u16 vertical_despawn_range;     /* 0x48 0: no vertical limit */
    char pad4A[0x10];
    u16 despawn_range;              /* 0x5A */
} EntitySpawnInfo;

/* sub-struct at Entity+0xE0 */
typedef struct {
    char pad00[0x7A];
    char unk7A;                     /* 0x7A */
} EntityUnkE0;

typedef struct Entity {
    int *model;                     /* 0x00 position at +0xC0 */
    char pad04[0x08];
    char *unk0C;                    /* 0x0C */
    EntitySpawnInfo *spawn_info;    /* 0x10 */
    char pad14[0x08];
    struct Entity *next;            /* 0x1C */
    char pad20[0x1C];
    f32 height;                     /* 0x3C */
    char pad40[0x24];
    f32 distance_to_player;         /* 0x64 */
    char pad68[0x6B];
    char unkD3;                     /* 0xD3 */
    char padD4;
    char group_id;                  /* 0xD5 */
    char padD6[0x02];
    s64 flags;                      /* 0xD8 */
    EntityUnkE0 unkE0;              /* 0xE0 */
} Entity;

extern Entity *D_004467E0;          /* entity list head */
extern Entity *D_004470A8;          /* other lists of the same layout */
extern Entity *D_004470B0;

float func_00124DE0(Entity *entity);    /* distance to the player */
void func_00123AB8(Entity *entity);
void func_00124BC8(Entity *entity);
Entity *func_00124C08(Entity *entity);  /* free; returns the next one */
Entity *func_00124CA0(Entity *entity);  /* free; returns the next one */
Entity *func_00124D48(Entity *entity);  /* free; returns the next one */

void KF_Entity_ResetAllLists(void);
void KF_Entity_FreeAllInGroup(int group_id);
void KF_Entity_DespawnOutOfRange(void);

#endif
