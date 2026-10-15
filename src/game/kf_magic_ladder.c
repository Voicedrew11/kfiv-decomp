#include "kf_magic_ladder.h"
#include "kf_player.h"
#include "kf_effect.h"
#include "kf_shot.h"
#include "kf_entity.h"

void *memset(void *, s32, u32);
float sqrtf(float);
float sinf(float);

extern PlayerState D_00413F80;
extern PlayerState D_00414280;
extern FxTableEntry D_00402DA8[];
extern Vector4 D_00431C80;
extern s32 D_00446960;
extern PlayerShot *D_00446964;
extern u32 D_00446ED4;
extern void (*D_004145C8[])(u16, u16);
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;

void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(void *, void *, float);
int func_001B4F30(u16, void *, void *, void *, void *);
void Player_CountMagicUse(int);
int Angle_Approach(float *, float, float);
int Sound_PlayFootstep(int);

static Vector4 ladder_foot_position;              /* 0x57B350 foot the player walks to before climbing */
static Vector4 ladder_walk_step;                  /* 0x57B360 unit step toward the foot each frame */
static Vector4 ladder_top_point;                  /* 0x57B370 ladder end from model get_point(1) */
static Vector4 ladder_bottom_point;               /* 0x57B380 ladder end from model get_point(0) */
static Vector4 ladder_span;                       /* 0x57B390 vector from bottom to top */
static PlayerShot channeled_magic_shots[16];      /* 0x57B3A0 ring of shots for held magics 7-9 */
static s32 ladder_state_index;                    /* 0x447128 index into D_004145C8 */
static ObjectReference *ladder_object;            /* 0x44712C map object being climbed */
static f32 ladder_facing_y;                       /* 0x447130 Y rotation to face the ladder */
static s32 ladder_walk_frames_remaining;          /* 0x447134 frames left walking to the foot */
static f32 ladder_rung_spacing;                   /* 0x447138 distance between rungs along the span */
static s32 ladder_rung_count;                     /* 0x44713C number of rungs on the ladder */
static s32 ladder_rung_anim_base;                 /* 0x447140 rung index at the start of a climb segment */
static s32 ladder_current_rung;                   /* 0x447144 rung the player is on or moving toward */
static s32 ladder_frames_per_rung;                /* 0x447148 frames to move one rung */
static s32 ladder_rung_anim_frame;                /* 0x44714C frame within the current rung move */
static s32 channeled_magic_active;                /* 0x447150 nonzero while held magic 7-9 is armed */
static s32 channeled_magic_unused;                /* 0x447154 never used by this code */
static s32 channeled_magic_warmup_frames;         /* 0x447158 countdown before MP drain for held magic */
static s32 channeled_shot_ring_index;             /* 0x44715C next slot in channeled_magic_shots */
static PlayerShot *channeled_shot_slot;           /* 0x447160 shot reused while magic 7-9 is held */

/* Casts magic `magic_id`: takes its cost, then launches its shot or applies what it does to the player. */
int Player_CastMagic(int magic_id) {
    Vector4 angles;
    FxTableEntry *entry;
    FxTableStats *stats;
    PlayerShot *shot;

    if (magic_id == 0) {
        return FALSE;
    }
    channeled_magic_active = 0;
    entry = &D_00402DA8[magic_id];
    stats = &entry->unkAC;
    if (entry->unk102 == 0) {
        if ((s32)(D_00413F80.unk1C8 >> 39) & 1) {
            return FALSE;
        }
        if ((s32)(D_00413F80.unk1C8 >> 38) & 1) {
            if (D_00413F80.position.y - entry->unk108 > D_00413F80.unk1DC) {
                return FALSE;
            }
        }
    }
    if (D_00413F80.equipment[EQUIP_ARMOR] == ITEM_WORN_INDEX(ITEM_ICE_ARMOR) && magic_id > 0 && (magic_id - 1) / 15 == 0) {
        return FALSE;
    }
    if (D_00413F80.unkC0 < 5000) {
        return FALSE;
    }
    if (D_00413F80.mp_now < entry->unkFC) {
        return FALSE;
    }
    D_00413F80.mp_now -= entry->unkFC;
    D_00413F80.unkC8 = entry->unk104;
    D_00413F80.unk264 = magic_id;
    D_00413F80.unkC0 = 0;
    if ((u32)(magic_id - 7) < 3) {
        channeled_magic_warmup_frames = 30;
        channeled_magic_active = 1;
        channeled_shot_slot = &channeled_magic_shots[channeled_shot_ring_index];
        memset(channeled_shot_slot, 0, sizeof(PlayerShot));
        channeled_shot_ring_index = (channeled_shot_ring_index + 1) % 16;
        return FALSE;
    }
    if (entry->unk102 == 0) {
        shot = Shot_Alloc();
        shot->unk00 = 1;
        shot->unk20 = D_00413F80.power;
        shot->unk22 = D_00413F80.magic;
        shot->unk26 = magic_id;
        shot->unk08 = D_00413F80.unk7E;
        Vector_Copy(&angles, &D_00413F80.rotation);
        angles.x += D_00413F80.unk238;
        if (angles.x > 1.012291f) {
            angles.x = 1.012291f;
        } else if (angles.x < -1.012291f) {
            angles.x = -1.012291f;
        }
        func_001B4F30(magic_id, shot, &D_00413F80, &D_00413F80.rotation, &D_00413F80); // match: the player's rotation is passed; the clamped angles above are never used
        D_00413F80.unk259 = 1;
    } else {
        if (stats->unk00[0] != 0) {
            D_00413F80.timed_flags[0] = 0;
        }
        if (stats->unk00[1] != 0) {
            D_00413F80.timed_flags[1] = 0;
        }
        if (stats->unk00[2] != 0) {
            D_00413F80.timed_flags[2] = 0;
        }
        if (stats->unk00[3] != 0) {
            D_00413F80.timed_flags[3] = 0;
        }
        if (stats->unk00[4] != 0) {
            D_00413F80.timed_flags[4] = 0;
        }
        if (stats->unk00[5] != 0) {
            D_00413F80.timed_flags[5] = 0;
        }
        if (stats->unk4A >= 999) {
            D_00413F80.hp_now = D_00413F80.hp_max;
        } else if (stats->unk4A != 0) {
            D_00413F80.hp_now += stats->unk4C;
            if (D_00413F80.hp_now > D_00413F80.hp_max) {
                D_00413F80.hp_now = D_00413F80.hp_max;
            }
            if (D_00413F80.hp_now < 0) {
                D_00413F80.hp_now = 0;
            }
            D_00413F80.unk2B0 = stats->unk4A - stats->unk4C;
            if (D_00413F80.unk2B0 == 0) {
                D_00413F80.unk2B4 = 0;
            } else {
                if (stats->unk4E > 0) {
                    D_00413F80.unk2B4 = stats->unk4E;
                } else {
                    D_00413F80.unk2B4 = 1;
                }
            }
        }
        if (stats->duration != 0) {
            if (D_00413F80.unk11C[0] <= 0 && D_00413F80.unk11C[1] <= 0 && D_00413F80.unk11C[2] <= 0
                && D_00413F80.unk122[0] <= 0 && D_00413F80.unk122[1] <= 0 && D_00413F80.unk122[2] <= 0
                && D_00413F80.unk122[3] <= 0 && D_00413F80.unk122[4] <= 0 && D_00413F80.unk122[5] <= 0) {
                D_00446960 = 0;
            }
            if (stats->unk18[0] != 0) {
                D_00413F80.unk11C[0] = stats->unk18[0];
                D_00413F80.unk158[0] = stats->duration;
                D_00413F80.unk188[0] = ++D_00446960;
            }
            if (stats->unk18[1] != 0) {
                D_00413F80.unk11C[1] = stats->unk18[1];
                D_00413F80.unk158[1] = stats->duration;
                D_00413F80.unk188[1] = ++D_00446960;
            }
            if (stats->unk18[2] != 0) {
                D_00413F80.unk11C[2] = stats->unk18[2];
                D_00413F80.unk158[2] = stats->duration;
                D_00413F80.unk188[2] = ++D_00446960;
            }
            if (stats->unk18[3] != 0) {
                D_00413F80.unk122[0] = stats->unk18[3];
                D_00413F80.unk15E[0] = stats->duration;
                D_00413F80.unk18E[0] = ++D_00446960;
            }
            if (stats->unk18[4] != 0) {
                D_00413F80.unk122[1] = stats->unk18[4];
                D_00413F80.unk15E[1] = stats->duration;
                D_00413F80.unk18E[1] = ++D_00446960;
            }
            if (stats->unk18[5] != 0) {
                D_00413F80.unk122[2] = stats->unk18[5];
                D_00413F80.unk15E[2] = stats->duration;
                D_00413F80.unk18E[2] = ++D_00446960;
            }
            if (stats->unk18[6] != 0) {
                D_00413F80.unk122[3] = stats->unk18[6];
                D_00413F80.unk15E[3] = stats->duration;
                D_00413F80.unk18E[3] = ++D_00446960;
            }
            if (stats->unk18[7] != 0) {
                D_00413F80.unk122[4] = stats->unk18[7];
                D_00413F80.unk15E[4] = stats->duration;
                D_00413F80.unk18E[4] = ++D_00446960;
            }
            if (stats->unk18[8] != 0) {
                D_00413F80.unk122[5] = stats->unk18[8];
                D_00413F80.unk15E[5] = stats->duration;
                D_00413F80.unk18E[5] = ++D_00446960;
            }
            if (stats->unk0C[0] != 0) {
                D_00413F80.unk110[0] = stats->unk0C[0];
                D_00413F80.unk14C[0] = stats->duration;
            }
            if (stats->unk0C[1] != 0) {
                D_00413F80.unk110[1] = stats->unk0C[1];
                D_00413F80.unk14C[1] = stats->duration;
            }
            if (stats->unk0C[2] != 0) {
                D_00413F80.unk110[2] = stats->unk0C[2];
                D_00413F80.unk14C[2] = stats->duration;
            }
            if (stats->unk0C[3] != 0) {
                D_00413F80.unk110[3] = stats->unk0C[3];
                D_00413F80.unk14C[3] = stats->duration;
            }
            if (stats->unk0C[4] != 0) {
                D_00413F80.unk110[4] = stats->unk0C[4];
                D_00413F80.unk14C[4] = stats->duration;
            }
            if (stats->unk0C[5] != 0) {
                D_00413F80.unk110[5] = stats->unk0C[5];
                D_00413F80.unk14C[5] = stats->duration;
            }
            if (stats->unk36[0] != 0) {
                D_00413F80.unk13A[0] = stats->unk36[0];
                D_00413F80.unk176[0] = stats->duration;
            }
            if (stats->unk36[1] != 0) {
                D_00413F80.unk13A[1] = stats->unk36[1];
                D_00413F80.unk176[1] = stats->duration;
            }
            if (stats->unk36[2] != 0) {
                D_00413F80.unk13A[2] = stats->unk36[2];
                D_00413F80.unk176[2] = stats->duration;
            }
            if (stats->unk36[3] != 0) {
                D_00413F80.unk140[0] = stats->unk36[3];
                D_00413F80.unk17C[0] = stats->duration;
            }
            if (stats->unk36[4] != 0) {
                D_00413F80.unk140[1] = stats->unk36[4];
                D_00413F80.unk17C[1] = stats->duration;
            }
            if (stats->unk36[5] != 0) {
                D_00413F80.unk140[2] = stats->unk36[5];
                D_00413F80.unk17C[2] = stats->duration;
            }
            if (stats->unk36[6] != 0) {
                D_00413F80.unk140[3] = stats->unk36[6];
                D_00413F80.unk17C[3] = stats->duration;
            }
            if (stats->unk36[7] != 0) {
                D_00413F80.unk140[4] = stats->unk36[7];
                D_00413F80.unk17C[4] = stats->duration;
            }
            if (stats->unk36[8] != 0) {
                D_00413F80.unk140[5] = stats->unk36[8];
                D_00413F80.unk17C[5] = stats->duration;
            }
            if (stats->unk2A[0] != 0) {
                D_00413F80.unk12E[0] = stats->unk2A[0];
                D_00413F80.unk16A[0] = stats->duration;
            }
            if (stats->unk2A[1] != 0) {
                D_00413F80.unk12E[1] = stats->unk2A[1];
                D_00413F80.unk16A[1] = stats->duration;
            }
            if (stats->unk2A[2] != 0) {
                D_00413F80.unk12E[2] = stats->unk2A[2];
                D_00413F80.unk16A[2] = stats->duration;
            }
            if (stats->unk2A[3] != 0) {
                D_00413F80.unk12E[3] = stats->unk2A[3];
                D_00413F80.unk16A[3] = stats->duration;
            }
            if (stats->unk2A[4] != 0) {
                D_00413F80.unk12E[4] = stats->unk2A[4];
                D_00413F80.unk16A[4] = stats->duration;
            }
            if (stats->unk2A[5] != 0) {
                D_00413F80.unk12E[5] = stats->unk2A[5];
                D_00413F80.unk16A[5] = stats->duration;
            }
        }
        switch (magic_id) {
        case 0x2E:
            D_00413F80.unk2DC = stats->duration;
            break;
        case 0x3D:
        case 0x3E:
        case 0x3F:
            D_00413F80.unk2E0 = stats->duration;
            break;
        case 0x4C:
        case 0x4D:
        case 0x4E:
            D_00413F80.unk2E4 = stats->duration;
            if (magic_id >= 0x4D) {
                D_00413F80.unk2E8 = 1000.0f;
            } else {
                D_00413F80.unk2E8 = 600.0f;
            }
            break;
        case 0x52:
            D_00413F80.unk2EC = stats->duration;
            D_00413F80.unk2F0 = 10;
            break;
        case 0x53:
            D_00413F80.unk2EC = stats->duration;
            D_00413F80.unk2F0 = 20;
            break;
        case 0x54:
            D_00413F80.unk2EC = stats->duration;
            D_00413F80.unk2F0 = 30;
            break;
        }
        Effect_FreeByParam(magic_id);
        Effect_Spawn(entry->unk100, &D_00431C80, &D_00431C80, (Entity *)&D_00413F80, magic_id, NULL);
        switch (magic_id) {
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x38:
        case 0x39:
        case 0x3A:
            if (D_00414280.hp_now < D_00414280.hp_max) {
                Player_CountMagicUse(magic_id);
            }
            break;
        case 0x2E:
        case 0x2F:
        case 0x30:
            if ((u16)D_00414280.timed_flags[0] != 0) { // match: read unsigned here
                Player_CountMagicUse(magic_id);
            }
            break;
        default:
            Player_CountMagicUse(magic_id);
            break;
        }
    }
    return TRUE;
}

/* Keeps a held magic (7 to 9) going: after the first 30 frames it costs one of unkB8 every third frame, and
   every call launches its shot again from the slot Player_CastMagic set aside. Out of unkB8, it stops. */
void Player_HoldMagic(int magic_id) {
    Vector4 angles;
    FxTableEntry *entry;

    if (magic_id == 0) {
        return;
    }
    entry = &D_00402DA8[magic_id];
    if ((u32)(magic_id - 7) < 3) {
        if (channeled_magic_active == 0) {
            return;
        }
        if (entry->unk102 == 0) {
            if ((s32)(D_00413F80.unk1C8 >> 39) & 1) {
                goto stop;
            }
            if ((s32)(D_00413F80.unk1C8 >> 38) & 1) {
                if (D_00413F80.position.y - entry->unk108 > D_00413F80.unk1DC) {
                    goto stop;
                }
            }
        }
        if (D_00413F80.equipment[EQUIP_ARMOR] == ITEM_WORN_INDEX(ITEM_ICE_ARMOR) && magic_id > 0 && (magic_id - 1) / 15 == 0) {
            return;
        }
        D_00413F80.unkC8 = entry->unk104;
        D_00413F80.unkC0 = 0;
        if (channeled_magic_warmup_frames > 0) {
            channeled_magic_warmup_frames--;
        } else {
            if (D_00413F80.mp_now <= 0) {
                goto stop;
            }
            if (D_00446ED4 % 3 == 0) {
                D_00413F80.mp_now--;
            }
        }
        if (channeled_shot_slot != NULL) {
            D_00446964 = channeled_shot_slot;
            channeled_shot_slot->unk00 = 1;
            D_00446964->unk20 = D_00413F80.power;
            D_00446964->unk22 = D_00413F80.magic;
            D_00446964->unk26 = magic_id;
            D_00446964->unk08 = D_00413F80.unk7E;
            Vector_Copy(&angles, &D_00413F80.rotation);
            angles.x += D_00413F80.unk238;
            if (angles.x > 1.012291f) {
                angles.x = 1.012291f;
            } else if (angles.x < -1.012291f) {
                angles.x = -1.012291f;
            }
            // match: the clamped angles are not what is passed
            func_001B4F30(magic_id, D_00446964, &D_00413F80, &D_00413F80.rotation, &D_00413F80);
            D_00413F80.unk259 = 1;
        }
    }
    return;
stop:
    channeled_magic_active = 0;
}

/* Puts the player on the object (a ladder, it seems): notes its two end points, the walk to the end the player
   starts from (12 units a frame) and the number of steps. */
void Ladder_Start(ObjectReference *object, int from_far_end) {
    Vector4 span;
    int step_count;
    ReferencedObject *def;

    if ((s32)(D_00413F80.unk1C8 >> 36) & 1) {
        return;
    }
    if ((s32)(D_00413F80.unk1C8 >> 34) & 1) {
        return;
    }
    Vector_Copy(&ladder_top_point, (void *)object->model->get_point(object->model, 1));
    Vector_Copy(&ladder_bottom_point, (void *)object->model->get_point(object->model, 0));
    Vector_Subtract(&ladder_span, &ladder_top_point, &ladder_bottom_point);
    PLAYER_STATUS_BITS(&D_00413F80)->bit36 = TRUE;
    PLAYER_STATUS_BITS(&D_00413F80)->bit34 = FALSE;
    PLAYER_STATUS_BITS(&D_00413F80)->bit33 = FALSE;
    D_00413F80.unk1AC = 0.0f;
    D_00413F80.unk1B0 = 0.0f;
    D_00413F80.unk1B4 = 0.0f;
    D_00413F80.unk1B8 = 0.0f;
    D_00413F80.unk1BC = 0.0f;
    ladder_state_index = 0;
    ladder_facing_y = object->model->rotation.y;
    ladder_object = object;
    if (from_far_end) {
        Vector_Copy(&ladder_foot_position, &ladder_top_point);
    } else {
        Vector_Copy(&ladder_foot_position, &ladder_bottom_point);
    }
    Vector_Subtract(&ladder_walk_step, &ladder_foot_position, &D_00413F80.position);
    ladder_walk_frames_remaining = sqrtf(ladder_walk_step.x * ladder_walk_step.x + ladder_walk_step.y * ladder_walk_step.y + ladder_walk_step.z * ladder_walk_step.z) / 12.0f;
    Vector_Scale(&ladder_walk_step, &ladder_walk_step, 1.0f / (f32)ladder_walk_frames_remaining);
    def = object->unk08;
    step_count = def->unk50;
    ladder_rung_count = step_count;
    if (from_far_end) {
        ladder_current_rung = step_count;
        ladder_rung_anim_base = step_count;
    } else {
        ladder_current_rung = 0;
        ladder_rung_anim_base = 0;
    }
    ladder_frames_per_rung = def->unk52;
    ladder_rung_anim_frame = 0;
    Vector_Subtract(&span, &ladder_bottom_point, &ladder_top_point);
    ladder_rung_spacing = sqrtf(span.x * span.x + span.y * span.y + span.z * span.z) / (f32)ladder_rung_count;
}

/* Clears ladder approach state when the player is knocked off during a climb. */
void Ladder_CancelApproach(void) {
    ladder_state_index = 0;
    D_00413F80.unk1C8 &= ~(1L << 36);
}

/* Dispatches pad input to the ladder state handler when the player is on a ladder. */
int Ladder_DispatchPad(u16 held, u16 pressed) {
    PlayerState *player = &D_00413F80;

    if ((s32)(player->unk1C8 >> 36) & 1) {
        D_004145C8[ladder_state_index](held, pressed);
    }
    return (s32)(player->unk1C8 >> 36) & 1;
}

/* Walks the player toward the ladder foot and turns to face it. */
void Ladder_WalkToFoot(void) {
    if (ladder_walk_frames_remaining > 0) {
        ladder_walk_frames_remaining--;
        if (ladder_walk_frames_remaining > 0) {
            Vector_Add(&D_00413F80.position, &D_00413F80.position, &ladder_walk_step);
        } else {
            Vector_Copy(&D_00413F80.position, &ladder_foot_position);
        }
    }
    if (Angle_Approach(&D_00413F80.rotation.y, ladder_facing_y, 0.029088821f) && ladder_walk_frames_remaining <= 0) {
        ladder_state_index++;
    }
}

/* Waits on a rung; up/down input starts climbing or leaves the ladder. */
void Ladder_WaitOnRung(u16 buttons) {
    if (!(ladder_object->unk10 & 0x10000)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (buttons & (D_00446934 | D_00446936)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if ((buttons & (D_00446930 | D_00446932)) && !(buttons & (D_0044692C | D_0044692E))) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (D_0044692C & buttons) {
        ladder_current_rung++;
        if (ladder_current_rung <= ladder_rung_count) {
            ladder_rung_anim_frame = 0;
            ladder_state_index++;
            return;
        }
    } else {
        if (!(D_0044692E & buttons)) {
            return;
        }
        ladder_current_rung--;
        if (ladder_current_rung >= 0) {
            ladder_rung_anim_frame = ladder_frames_per_rung;
            ladder_state_index += 2;
            return;
        }
    }
    D_00413F80.unk1C8 &= ~(1L << 36);
}

/* Moves the player one rung up along a sine ease and plays a step sound. */
void Ladder_ClimbRungUp(u16 buttons) {
    Vector4 offset;
    f32 angle;
    f32 fraction;

    if (!(ladder_object->unk10 & 0x10000)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (buttons & (D_00446934 | D_00446936)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if ((buttons & (D_00446930 | D_00446932)) && !(buttons & (D_0044692C | D_0044692E))) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    ladder_rung_anim_frame++;
    angle = 1.9198622f;
    fraction = 1.0f;
    fraction /= (f32)ladder_rung_count;
    fraction = fraction * sinf((f32)ladder_rung_anim_frame * angle / (f32)ladder_frames_per_rung) / sinf(angle);
    Vector_Scale(&offset, &ladder_span,
                    fraction + (f32)(ladder_rung_anim_base * ladder_frames_per_rung) / (f32)(ladder_rung_count * ladder_frames_per_rung));
    Vector_Add(&D_00413F80.position, &ladder_bottom_point, &offset);
    if (ladder_rung_anim_frame >= ladder_frames_per_rung) {
        ladder_rung_anim_base = ladder_current_rung;
        ladder_state_index--;
        Ladder_PlayStepSound();
    }
}

/* Moves the player one rung down along a sine ease and plays a step sound. */
void Ladder_ClimbRungDown(u16 buttons) {
    Vector4 offset;
    f32 angle;
    f32 fraction;

    if (!(ladder_object->unk10 & 0x10000)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (buttons & (D_00446934 | D_00446936)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if ((buttons & (D_00446930 | D_00446932)) && !(buttons & (D_0044692C | D_0044692E))) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    ladder_rung_anim_frame--;
    angle = 1.6580628f;
    fraction = 1.0f;
    fraction /= (f32)ladder_rung_count;
    fraction = fraction * sinf((f32)ladder_rung_anim_frame * angle / (f32)ladder_frames_per_rung) / sinf(angle);
    Vector_Scale(&offset, &ladder_span,
                    fraction + (f32)(ladder_current_rung * ladder_frames_per_rung) / (f32)(ladder_rung_count * ladder_frames_per_rung));
    Vector_Add(&D_00413F80.position, &ladder_bottom_point, &offset);
    if (ladder_rung_anim_frame <= 0) {
        ladder_rung_anim_base = ladder_current_rung;
        ladder_state_index -= 2;
        Ladder_PlayStepSound();
    }
}

/* Plays the sound for the kind of object ladder_object refers to. */
int Ladder_PlayStepSound(void) {
    u16 kind;

    if (ladder_object != NULL) {
        kind = ladder_object->unk08->unk30;
        switch (kind) {
        case 0x56:
            return Sound_PlayFootstep(0x22);
        case 0x167:
        case 0x1B1:
            return Sound_PlayFootstep(0x20);
        default:
            Sound_PlayFootstep(0x20);
            break;
        }
    }
}
