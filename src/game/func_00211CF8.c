#include "kf_script.h"

int func_00211A58(void *, int, int);
int func_00211AD0(void *, int, int);
int func_00211B48(void *, int, int);
extern char D_00432923[];

/* Dispatch on the low byte of command->opcode (0x11-0x13) with the int as the operand value. */
int func_00211CF8(ScriptArg *command, int value) {
    ScriptArg operand;
    int opcode;
    int high;
    int param;

    opcode = command->opcode;
    param = command->value;
    operand.value = value;
    high = opcode >> 8;
    switch (opcode & 0xFF) {
    case 0x11:
        return func_00211A58(&operand, high, param);
    case 0x12:
        return func_00211B48(&operand, high, param);
    case 0x13:
        return func_00211AD0(&operand, high, param);
    default:
        D_00432923[0] = 1;
    }
}
