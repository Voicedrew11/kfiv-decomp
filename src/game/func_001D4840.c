#include "kf_game.h"
#include "kf_link.h"
#include "kf_player.h"

int sprintf(char *, char *, ...);
extern PlayerState D_00413F80;
extern GameDefinitionWords D_004147F8[380];
extern char D_0043F7B0[];
extern char D_005CE9F8[];
extern char D_005CEA98[];
extern char D_005CEAE8[];
extern char D_005CEB38[];
extern char D_005CEB88[];
extern char D_005CEBD8[];
extern char D_005CEC28[];
extern char D_005CEC78[];
extern char D_005CECC8[];
extern LinkNode D_005CED40;

/* Writes the model file names of the player's weapon and the eight things worn into their upload nodes. */
void func_001D4840(void) {
    sprintf(D_005CE9F8, D_0043F7B0, D_004147F8[D_00413F80.unk3C + 150].file_number);
    sprintf(D_005CEA98, D_0043F7B0, D_004147F8[D_00413F80.equipment[0] + 220].file_number);
    sprintf(D_005CEAE8, D_0043F7B0, D_004147F8[D_00413F80.equipment[2] + 220].file_number);
    sprintf(D_005CEB38, D_0043F7B0, D_004147F8[D_00413F80.equipment[1] + 220].file_number);
    sprintf(D_005CEB88, D_0043F7B0, D_004147F8[D_00413F80.equipment[3] + 220].file_number);
    sprintf(D_005CEBD8, D_0043F7B0, D_004147F8[D_00413F80.equipment[4] + 220].file_number);
    sprintf(D_005CEC28, D_0043F7B0, D_004147F8[D_00413F80.equipment[5] + 220].file_number);
    sprintf(D_005CEC78, D_0043F7B0, D_004147F8[D_00413F80.equipment[6] + 220].file_number);
    sprintf(D_005CECC8, D_0043F7B0, D_004147F8[D_00413F80.equipment[7] + 220].file_number);
    D_005CED40.unk20 = 0;
}
