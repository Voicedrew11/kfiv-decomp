#include "kf_game.h"
#include "kf_menu.h"

int sprintf(char *, char *, ...);
extern GameDefinitionBits D_004147F8[380];
extern char D_0043F750[];
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern char D_00426498[];           /* the names of the five link nodes at D_00426470 */
extern char D_004264E8[];
extern char D_00426538[];
extern char D_00426588[];
extern char D_004265D8[];
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern s32 D_00446C48;

/* Builds the model file names of the five link nodes: the selected item, its two neighbours, and the items that
   come into view next on either side (found by walking both ways from the page mark, seven steps in all). */
void func_001E4660(void) {
    MenuItem *backward;
    MenuItem *forward;
    s32 i;

    forward = backward = D_00426120[0];
    if (D_00446C48 >= 8) {
        for (i = 0; i < 7; i++) {
            forward = forward->next;
            backward = backward->prev;
            if (forward == D_00446C40) {
                while (++i < 7 && backward != D_00446C44) {
                    backward = backward->prev;
                }
                break;
            }
            if (backward == D_00446C44) {
                while (++i < 7 && forward != D_00446C40) {
                    forward = forward->next;
                }
                break;
            }
        }
    } else {
        forward = backward = D_00446C40;
    }
    sprintf(D_00426498, D_0043F750, D_004147F8[D_00426110[0]->unk0C].file_number);
    sprintf(D_00426538, D_0043F750, D_004147F8[D_00426110[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, D_0043F750, D_004147F8[D_00426110[0]->next->unk0C].file_number);
    sprintf(D_00426588, D_0043F750, D_004147F8[forward->unk0C].file_number);
    sprintf(D_004265D8, D_0043F750, D_004147F8[backward->unk0C].file_number);
}
