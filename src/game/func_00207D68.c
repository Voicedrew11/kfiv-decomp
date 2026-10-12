#include "kf_player.h"

extern u8 D_00369830[380];
extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];

int atoi(const char *);
char *strchr(const char *, int);
char *strrchr(const char *, int);

/* ini/NNNN.ini, key "item_NNN=count,level,time": how many of item NNN the player has; for a weapon (150 to 219)
   also its level (50 to 100) and unk16/unk14, for 220 and up its level. */
void func_00207D68(char *text) {
    int item;
    int index;
    int number;
    int time;
    char *comma;

    if ((u32)((u8)text[-4] - '0') < 10 && text[-3] >= '0' && text[-3] <= '9' && text[-2] >= '0' && text[-2] <= '9') {
        item = atoi(text - 4);
        D_00369830[item] = atoi(text); // match: item can be 0 to 999 here; the table has 380 entries
        index = item - 150;
        if ((u32)index < 70) {
            comma = strchr(text, ',');
            if (comma == NULL) {
                return;
            }
            number = atoi(comma + 1);
            if (number < 50) {
                number = 50;
            } else if (number > 100) {
                number = 100;
            }
            D_003679A0[index].level = number;
            comma = strchr(text, ',');
            if (comma != NULL) {
                comma = strrchr(comma + 1, ',');
                if (comma == NULL) {
                    return;
                }
                time = atoi(comma + 1) * 20;
                if (time < 0) {
                    time = 0;
                } else if (time > 480) {
                    time = 480;
                }
                D_003679A0[index].unk16 = time;
                if (time >= 480) {
                    D_003679A0[index].unk14 = 2;
                } else if (time >= 180) {
                    D_003679A0[index].unk14 = 1;
                } else {
                    D_003679A0[index].unk14 = 0;
                }
            }
        } else if ((u32)(item - 160) < 160) { // match: tests 160 to 319 where the index below is item - 220, so 320 to 379 get no level
            comma = strchr(text, ',');
            if (comma == NULL) {
                return;
            }
            number = atoi(comma + 1);
            if (number < 50) {
                number = 50;
            } else if (number > 100) {
                number = 100;
            }
            D_003682F0[item - 220].level = number;
        }
    }
}
