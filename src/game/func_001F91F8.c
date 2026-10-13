#include "kf_title.h"

int func_001D62C8(s8);

int func_001F91F8(void) {
    int screen = 0;

    do {
        switch (KF_Title_Run(screen)) {
        case 1:
            return 0;
        case 2:
            if (func_001D62C8(1) != 0) {
                return 1;
            }
            screen = 2;
            break;
        case 3:
            func_001D62C8(2);
            screen = 2;
            break;
        case 4:
            func_001D62C8(3);
            screen = 2;
            break;
        }
    } while (screen != -1);
}
