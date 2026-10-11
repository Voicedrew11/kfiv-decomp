#include "common.h"
#include "kf_file.h"
#include "kf_splash.h"

extern int D_0038A134[3];
extern int D_0044735C;
extern int D_00447358;
extern int D_00447360;
extern int D_0038CC80[3];

int sprintf(char *, const char *, ...);
int func_00102230(void);
void func_00102218(void);
void func_002087D0(int, int);

/* Loads the splash image file into D_00249680 and sets up its display. */
void func_00208320(void) {
    char image_file_name[256];
    int image_fd;

    if (D_0038A134[0] != 0) {
        return;
    }
    if (func_00102230() != 0) {
        return;
    }
    sprintf(image_file_name, "msg/m%4.4d.tm2", D_0044735C);
    image_fd = func_00102238(image_file_name, 0, 1);
    if (image_fd >= 0) {
        func_00102218();
        D_00447360 = 1;
        memset(&D_00249680, 0, 0x80000);
        func_001025C0(image_fd, &D_00249680, 0x80000);
        func_001024B0(image_fd);
        func_002087D0(0x2E40, 0x3D40);
        func_00208670();
        D_00447358 += 1;
    } else {
        D_00447358 = 0;
        D_0038CC80[0] = 0;
        D_00447360 = 0;
    }
}
