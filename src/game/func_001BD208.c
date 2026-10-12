#include "kf_player.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;
extern FxTableEntry D_00402DA8[];
extern u32 D_00446ED4;
extern PlayerShot *D_00446964;
extern s32 D_00447150;
extern s32 D_00447158;
extern PlayerShot *D_00447160;

void func_00216778(void *, void *);
int func_001B4F30(u16, void *, void *, void *, void *);

/* Keeps a held magic (7 to 9) going: after the first 30 frames it costs one of unkB8 every third frame, and
   every call launches its shot again from the slot func_001BC9E0 set aside. Out of unkB8, it stops. */
void func_001BD208(int magic_id) {
    Vector4 angles;
    FxTableEntry *entry;

    if (magic_id == 0) {
        return;
    }
    entry = &D_00402DA8[magic_id];
    if ((u32)(magic_id - 7) < 3) {
        if (D_00447150 == 0) {
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
        if (D_00413F80.equipment[2] == 0x49 && magic_id > 0 && (magic_id - 1) / 15 == 0) {
            return;
        }
        D_00413F80.unkC8 = entry->unk104;
        D_00413F80.unkC0 = 0;
        if (D_00447158 > 0) {
            D_00447158--;
        } else {
            if (D_00413F80.unkB8 <= 0) {
                goto stop;
            }
            if (D_00446ED4 % 3 == 0) {
                D_00413F80.unkB8--;
            }
        }
        if (D_00447160 != NULL) {
            D_00446964 = D_00447160;
            D_00447160->unk00 = 1;
            D_00446964->unk20 = D_00413F80.unkAC;
            D_00446964->unk22 = D_00413F80.unkB0;
            D_00446964->unk26 = magic_id;
            D_00446964->unk08 = D_00413F80.unk7E;
            func_00216778(&angles, &D_00413F80.rotation);
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
    D_00447150 = 0;
}
