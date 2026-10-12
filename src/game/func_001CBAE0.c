#include "kf_sound.h"

/* EE EABI varargs: the register save area a1..a7 is spilled to, as in libsdr.c */
typedef char *va_list;
#define va_start(ap, last) ((ap) = (char *)__builtin_next_arg(last) - 0x38)
#define va_arg(ap, type) (*(type *)((ap += 8) - 8))

int func_001CBA58(void);
int sceSifCallRpc(int *, int, int, char *, int, int *, int, int, int);
extern SoundRpcRequest D_005BF280;
extern int D_005BF890[];

/* Sends a command and its arguments to the sound driver on the IOP (the game's own sceSdRemote)
   and returns the driver's answer. */
int func_001CBAE0(int blocking, ...) {
    va_list ap;
    int command;
    int no_wait;
    int result;
    int i;

    va_start(ap, blocking);
    D_005BF280.self = &D_005BF280;
    command = va_arg(ap, int);
    // match: copies eight words although the save area only holds six more
    for (i = 0; i < 8; i++) {
        D_005BF280.args[i] = va_arg(ap, int);
    }
    no_wait = FALSE;
    if (blocking == 0) {
        no_wait = TRUE;
    }
    result = func_001CBA58();
    if (result < 0) {
        return result;
    }
    if (command & 0x80000) {
        sceSifCallRpc(D_005BF890, command, no_wait, (char *)&D_005BF280, 0xA0, (int *)D_005BF280.args[0], 0xA0, 0,
                      D_005BF280.args[0]);
    } else if (command & 0x40000) {
        sceSifCallRpc(D_005BF890, command, no_wait, (char *)D_005BF280.args[0], 0x40, (int *)&D_005BF280, 0x10, 0,
                      (int)D_005BF280.self);
        result = (int)D_005BF280.self;
    } else {
        sceSifCallRpc(D_005BF890, command, no_wait, (char *)&D_005BF280, 0x40, (int *)&D_005BF280, 0x10, 0,
                      (int)&D_005BF280);
        result = (int)D_005BF280.self;
    }
    return result;
}
