#include "common.h"

int RotateThreadReadyQueue(int);

/* Lets the other movie threads run (the SDK sample's switchThread). */
void func_0021B9E0(void) {
    RotateThreadReadyQueue(1);
}
