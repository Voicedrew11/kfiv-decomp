#include "kf_player.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;
extern FxTableEntry D_00402DA8[];
extern Vector4 D_00431C80;
extern PlayerShot D_0057B3A0[16];
extern s32 D_00446960;
extern s32 D_00447150;
extern s32 D_00447158;
extern s32 D_0044715C;
extern PlayerShot *D_00447160;

void *memset(void *, s32, u32);
void *func_001B1390(void);
void func_00216778(void *, void *);
int func_001B4F30(u16, void *, void *, void *, void *);
void func_001A34D0(int);
void func_001BBC88(int);

/* Casts magic `magic_id`: takes its cost, then launches its shot or applies what it does to the player. */
int func_001BC9E0(int magic_id) {
    Vector4 angles;
    FxTableEntry *entry;
    FxTableStats *stats;
    PlayerShot *shot;

    if (magic_id == 0) {
        return FALSE;
    }
    D_00447150 = 0;
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
    if (D_00413F80.equipment[2] == 0x49 && magic_id > 0 && (magic_id - 1) / 15 == 0) {
        return FALSE;
    }
    if (D_00413F80.unkC0 < 5000) {
        return FALSE;
    }
    if (D_00413F80.unkB8 < entry->unkFC) {
        return FALSE;
    }
    D_00413F80.unkB8 -= entry->unkFC;
    D_00413F80.unkC8 = entry->unk104;
    D_00413F80.unk264 = magic_id;
    D_00413F80.unkC0 = 0;
    if ((u32)(magic_id - 7) < 3) {
        D_00447158 = 30;
        D_00447150 = 1;
        D_00447160 = &D_0057B3A0[D_0044715C];
        memset(D_00447160, 0, sizeof(PlayerShot));
        D_0044715C = (D_0044715C + 1) % 16;
        return FALSE;
    }
    if (entry->unk102 == 0) {
        shot = func_001B1390();
        shot->unk00 = 1;
        shot->unk20 = D_00413F80.unkAC;
        shot->unk22 = D_00413F80.unkB0;
        shot->unk26 = magic_id;
        shot->unk08 = D_00413F80.unk7E;
        func_00216778(&angles, &D_00413F80.rotation);
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
            D_00413F80.unkB4 = D_00413F80.unk58;
        } else if (stats->unk4A != 0) {
            D_00413F80.unkB4 += stats->unk4C;
            if (D_00413F80.unkB4 > D_00413F80.unk58) {
                D_00413F80.unkB4 = D_00413F80.unk58;
            }
            if (D_00413F80.unkB4 < 0) {
                D_00413F80.unkB4 = 0;
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
        func_001A34D0(magic_id);
        func_001A1E28(entry->unk100, &D_00431C80, &D_00431C80, (Entity *)&D_00413F80, magic_id, NULL);
        switch (magic_id) {
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x38:
        case 0x39:
        case 0x3A:
            if (D_00414280.unkB4 < D_00414280.unk58) {
                func_001BBC88(magic_id);
            }
            break;
        case 0x2E:
        case 0x2F:
        case 0x30:
            if ((u16)D_00414280.timed_flags[0] != 0) { // match: read unsigned here
                func_001BBC88(magic_id);
            }
            break;
        default:
            func_001BBC88(magic_id);
            break;
        }
    }
    return TRUE;
}
