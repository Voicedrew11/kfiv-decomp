#include "kf_game.h"
#include "kf_menu.h"

int sprintf(char *, char *, ...);
extern GameDefinitionBits D_004147F8[380];
extern char D_00426498[];
extern char D_004264E8[];
extern char D_00426538[];
extern char D_00426588[];
extern char D_004265D8[];
extern char D_004404A8[];
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern int D_00446C48;
extern MenuItem *D_005CF390[];
extern MenuItem *D_005CF3B0[];

/* Writes the model file names of the selected item, its two neighbours, and the two items just outside the visible
   window (found by walking at most 7 steps out from D_005CF390 towards the list's ends) into the five link nodes. */
void func_001EF648(void) {
    MenuItem *before = D_005CF390[0];
    MenuItem *after = before;
    int steps;

    if (D_00446C48 >= 8) {
        for (steps = 0; steps < 7; steps++) {
            after = after->next;
            before = before->prev;
            if (after == D_00446C40) {
                for (steps++; steps < 7 && before != D_00446C44; steps++) {
                    before = before->prev;
                }
                break;
            }
            if (before == D_00446C44) {
                for (steps++; steps < 7 && after != D_00446C40; steps++) {
                    after = after->next;
                }
                break;
            }
        }
    } else {
        after = before = D_00446C40;
    }
    sprintf(D_00426498, D_004404A8, D_004147F8[D_005CF3B0[0]->unk0C].file_number);
    sprintf(D_00426538, D_004404A8, D_004147F8[D_005CF3B0[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, D_004404A8, D_004147F8[D_005CF3B0[0]->next->unk0C].file_number);
    sprintf(D_00426588, D_004404A8, D_004147F8[after->unk0C].file_number);
    sprintf(D_004265D8, D_004404A8, D_004147F8[before->unk0C].file_number);
}
