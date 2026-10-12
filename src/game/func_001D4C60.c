#include "kf_game.h"
#include "kf_menu.h"

int sprintf(char *, char *, ...);
extern GameDefinitionWords D_004147F8[380];
extern char D_0043F750[];
extern MenuItem *D_005CE850[];
extern MenuItem *D_005CE860[];
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C38;
extern char D_00426498[];
extern char D_004264E8[];
extern char D_00426538[];
extern char D_00426588[];
extern char D_004265D8[];

/* Writes the model file names of the five items on show in a long list: the current one, its two
   neighbours, and the items seven steps on either way (or the list's ends when nearer). */
void func_001D4C60(void) {
    MenuItem *ahead;
    MenuItem *behind;
    s32 i;

    ahead = behind = D_005CE860[0];
    if (D_00446C38 >= 8) {
        for (i = 0; i < 7; i++) {
            ahead = ahead->next;
            behind = behind->prev;
            if (ahead == D_00446C24) {
                i++;
                while (i < 7 && behind != D_00446C28) {
                    i++;
                    behind = behind->prev;
                }
                break;
            } else if (behind == D_00446C28) {
                i++;
                while (i < 7 && ahead != D_00446C24) {
                    i++;
                    ahead = ahead->next;
                }
                break;
            }
        }
    } else {
        ahead = behind = D_00446C24;
    }
    sprintf(D_00426498, D_0043F750, D_004147F8[D_005CE850[0]->unk0C].file_number);
    sprintf(D_00426538, D_0043F750, D_004147F8[D_005CE850[0]->prev->unk0C].file_number);
    sprintf(D_004264E8, D_0043F750, D_004147F8[D_005CE850[0]->next->unk0C].file_number);
    sprintf(D_00426588, D_0043F750, D_004147F8[ahead->unk0C].file_number);
    sprintf(D_004265D8, D_0043F750, D_004147F8[behind->unk0C].file_number);
}
