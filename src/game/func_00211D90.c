#include "kf_script.h"

int func_00211A58(void *, int, int);
int func_00211AD0(void *, int, int);
int func_00211B48(void *, int, int);
extern char D_00432924[];

/* Dispatch on the low byte of command[0] (0x11-0x13) with the float as the operand value. */
int func_00211D90(int *command, float value) {
    ScriptArg operand;
    FloatBits bits;
    int opcode;
    int high;
    int param;
    int result;

    bits.as_float = value;
    opcode = command[0];
    param = command[1];
    operand.value = bits.as_int;
    high = opcode >> 8;
    switch (opcode & 0xFF) {
    case 0x11:
        return func_00211A58(&operand, high, param);
    case 0x12:
        return func_00211B48(&operand, high, param);
    case 0x13:
        return func_00211AD0(&operand, high, param);
    }
    D_00432924[0] = result = TRUE;
    return result;
}
