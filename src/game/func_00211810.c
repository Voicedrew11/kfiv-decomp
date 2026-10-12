#include "kf_script.h"

void func_002118D8(ScriptArg *, u32, int);
void func_00211958(ScriptArg *, u32, int);
void func_002119D8(ScriptArg *, ScriptThread *, u32, int);
extern char D_00432922[];

/* Works out one argument of a command: a constant, or the value of a game, thread or shared variable;
   kinds 0x11 to 0x13 are passed on as they are, for a command that writes its answer there. */
void func_00211810(ScriptThread *thread, u32 *source, ScriptArg *result) {
    u32 word;
    int value;
    u32 code;
    u32 type;
    u64 kind; // match: a 64-bit switch value gives retail's dsll32/dsra32 on the table index
    char unused[0x50]; // match: retail reserves 0x50 bytes it never uses

    word = source[0];
    value = source[1];
    kind = word & 0xFF;
    type = word >> 16;
    code = word >> 8;
    switch (kind) {
    case 1:
        func_002118D8(result, code, value);
        break;
    case 2:
        func_002119D8(result, thread, code, value);
        break;
    case 3:
        func_00211958(result, code, value);
        break;
    case 0:
        result->opcode = type;
        result->value = value;
        break;
    case 0x11:
    case 0x12:
    case 0x13:
        result->opcode = word;
        result->value = value;
        break;
    default:
        D_00432922[0] = 1;
        break;
    }
}
