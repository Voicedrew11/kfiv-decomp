#include "common.h"

void sceSifInitRpc(int);
void func_00102150(void);
int sceSifRebootIop(const char *);
int sceSifSyncIop(void);
void sceFsReset(void);
int sceSifLoadModule(const char *, int, const char *);

extern char D_00441350[];
extern char D_00441368[];
extern char D_00441388[];
extern char D_004413A8[];
extern char D_004413C0[];
extern char D_004413E0[];
extern char D_004413F8[];
extern char D_00441418[];
extern char D_00441438[];
extern char D_00441458[];
extern char D_00446F90[];

/* Reboots the IOP with the game's image and loads the nine IOP modules. */
void func_0020B458(void) {
    sceSifInitRpc(0);
    func_00102150();
    while (!sceSifRebootIop(D_00441350)) {
    }
    while (!sceSifSyncIop()) {
    }
    sceSifInitRpc(0);
    func_00102150();
    sceFsReset();
    sceSifLoadModule(D_00441368, 0, D_00446F90);
    sceSifLoadModule(D_00441388, 0, D_00446F90);
    sceSifLoadModule(D_004413A8, 0, D_00446F90);
    sceSifLoadModule(D_004413C0, 0, D_00446F90);
    sceSifLoadModule(D_004413E0, 0, D_00446F90);
    sceSifLoadModule(D_004413F8, 0, D_00446F90);
    sceSifLoadModule(D_00441418, 0, D_00446F90);
    sceSifLoadModule(D_00441438, 0, D_00446F90);
    sceSifLoadModule(D_00441458, 0, D_00446F90);
}
