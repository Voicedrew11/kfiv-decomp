#include "kf_game.h"
#include "kf_link.h"
#include "kf_menu.h"

int sprintf(char *, char *, ...);
void func_001D40C0(LinkNode *node);
extern const KeyItemNumbers D_0043F7C8;
extern const KeyItemNumbers D_0043F7F8;
extern char D_0043F828[];
extern char D_0043F840[];
extern u8 D_00369830[380];
extern GameDefinitionWords D_004147F8[380];
extern LinkNode D_005CEF30[12];
extern LinkNode *D_00446C90;

/* Sets up the loads of the twelve key item models (the model of each one held, a stand-in file
   for the others) and chains their nodes behind the node waiting. */
void func_001D50C0(void) {
    KeyItemNumbers numbers = D_0043F7C8;
    KeyItemNumbers unused_numbers = D_0043F7F8; // match: a second table is copied and never read
    s32 i;

    for (i = 0; i < 12; i++) {
        func_001D4598(&D_005CEF30[i]);
        D_005CEF30[i].callback = (void (*)(void))func_001D40C0;
        D_005CEF30[i].unk1C = 0;
        D_005CEF30[i].unk20 = 0;
        if (D_00369830[numbers.number[i]] != 0) {
            sprintf(D_005CEF30[i].name, D_0043F828, D_004147F8[numbers.number[i]].file_number);
        } else {
            sprintf(D_005CEF30[i].name, D_0043F840);
        }
    }
    if (D_00446C90 != NULL) {
        D_00446C90->next = &D_005CEF30[0];
    } else {
        D_00446C90 = &D_005CEF30[0];
    }
    for (i = 10; i >= 0; i--) {
        D_005CEF30[i].next = &D_005CEF30[i + 1];
    }
    D_005CEF30[11].next = NULL;
}
