#include "kf_script.h"

/* Steps over one word of a script and its operands; gives the next word. */
u32 *func_00210CD0(u32 *cursor) {
    u32 word;
    int operand;

    word = *cursor;
    operand = word & 0xFFFF;
    cursor++;
    switch (word >> 16) {
    case 0x190:
    case 0x191:
    case 0x194:
    case 0x195:
    case 0x197:
        break;
    case 0x192:
        cursor += (operand >> 8) * 5 + 1;
        break;
    case 0x196:
        cursor += operand;
        break;
    case 0x19D:
        cursor += operand + 1;
        break;
    default:
        cursor += operand * 2;
        break;
    }
    return cursor;
}
