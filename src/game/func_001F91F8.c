#include "kf_title.h"
#include "kf_menu_screens.h"


int func_001F91F8(void) {
    int screen = 0;

    do {
        switch (Title_Run(screen)) {
        case 1:
            return 0;
        case 2:
            if (Menu_Run(1) != 0) {
                return 1;
            }
            screen = 2;
            break;
        case 3:
            Menu_Run(2);
            screen = 2;
            break;
        case 4:
            Menu_Run(3);
            screen = 2;
            break;
        }
    } while (screen != -1);
}
