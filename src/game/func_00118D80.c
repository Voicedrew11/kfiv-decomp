#include "kf_map.h"
#include "kf_screen.h"

extern int D_0038A130[0x28 / 4];
extern MapLoadState D_0038D610;
extern ScreenFade D_0038D680;

int func_00102230(void);
void func_00117CC8(void);
int func_00123108(void);
void func_00123288(void);
void Sound_Update(void);

/* Map change step: lets the background load and everything else in flight settle. */
void func_00118D80(void) {
    func_00117CC8();
    func_00123288();
    Sound_Update();
    if (D_0038D610.step == 0 || D_0038D610.step == 5) {
        if (func_00123108() == 0 && D_0038A130[3] == 0 && D_0038A130[1] == 0 && D_0038A130[6] == 0 &&
            func_00102230() == 0) {
            D_0038D680.step++;
        }
    }
}
