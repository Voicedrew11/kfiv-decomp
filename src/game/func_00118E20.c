#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_gold.h"
#include "kf_linked_point.h"
#include "kf_map.h"
#include "kf_screen.h"
#include "kf_effect.h"

extern MapSlot D_0038CD20[2];
extern MapProp *D_0038CDE0[2][0xFF];
extern ScreenFade D_0038D680;
extern int D_00446770;
extern int D_00446778[2];
extern int D_00446780[2];
extern AreaNode *D_00446788;
extern AreaNode *D_0044678C;

void *memset(void *, int, u32);
void func_00115878(int);
void func_00117C60(void);
void func_0011CE80(int);

void func_001A15F8(void);
void func_001BC128(void);
void func_001C8938(int);

/* Map change step: frees everything both map slots hold and clears the map tables. */
void func_00118E20(void) {
    int slot;

    for (slot = 0; slot < 2; slot++) {
        if (D_0038CD20[slot].unk00 != 0) {
            func_00115878(slot);
            KF_Effect_FreeForMap(slot);
            func_0011CE80(slot);
            KF_Entity_FreeAllInGroup(slot);
            KF_Entity_ClearMapSlot(slot);
            KF_Gold_UnlinkMap(slot);
            func_001C8938(slot);
            KF_LinkedPoint_ReleaseByKind(slot);
        }
    }
    func_00117C60();
    func_001A15F8();
    KF_Entity_ResetAllLists();
    func_001BC128();
    KF_Effect_FreePlayerMagic();
    D_00446770 = 0;
    memset(D_0038CD20, 0, sizeof(D_0038CD20));
    memset(D_00446778, 0, sizeof(D_00446778));
    memset(D_00446780, 0, sizeof(D_00446780));
    memset(D_0038CDE0, 0, sizeof(D_0038CDE0));
    D_00446788 = NULL;
    D_0044678C = NULL;
    D_0038D680.step++;
}
