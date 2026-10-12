#include "kf_script.h"

void func_00211810(ScriptThread *, u32 *, ScriptArg *);
void func_00211A58(ScriptArg *, u32, int);
void func_00211AD0(ScriptArg *, u32, int);
void func_00211BC0(ScriptArg *, ScriptThread *, u32, int);

/* The script's "set": reads the variable to change, then operands and operators in turn up to the closing
   word, working in float when the variable's type is 6, and writes the result back to the variable. */
u32 *func_002114D8(ScriptThread *thread, u32 *cursor) {
    ScriptValue target;
    ScriptValue result;
    ScriptValue operand;
    int running;
    int total;
    f32 float_total;
    int operator;
    int is_float;
    int want_operator;
    u32 word;
    u32 code;
    int offset;
    FloatBits bits;
    int value;

    running = TRUE;
    want_operator = FALSE;
    operator = 0xFF;
    cursor++;
    target.as_int.opcode = cursor[0];
    target.as_int.value = cursor[1];
    func_00211810(thread, cursor, &result.as_int);
    cursor += 2;
    is_float = result.as_int.opcode == 6;
    do {
        if (!want_operator) {
            func_00211810(thread, cursor, &operand.as_int);
            cursor += 2;
            if (!is_float) {
                switch (operator) {
                case 0xFF:
                    total = operand.as_int.value;
                    break;
                case 0:
                    total += operand.as_int.value;
                    break;
                case 1:
                    total -= operand.as_int.value;
                    break;
                case 2:
                    total *= operand.as_int.value;
                    break;
                case 3:
                    total /= operand.as_int.value;
                    break;
                case 4:
                    total %= operand.as_int.value;
                    break;
                case 5:
                    total <<= operand.as_int.value;
                    break;
                case 6:
                    total >>= operand.as_int.value;
                    break;
                case 7:
                    total &= operand.as_int.value;
                    break;
                case 8:
                    total |= operand.as_int.value;
                    break;
                }
            } else {
                switch (operator) {
                case 0xFF:
                    float_total = operand.as_float.value;
                    break;
                case 0:
                    float_total += operand.as_float.value;
                    break;
                case 1:
                    float_total -= operand.as_float.value;
                    break;
                case 2:
                    float_total *= operand.as_float.value;
                    break;
                case 3:
                    float_total /= operand.as_float.value;
                    break;
                }
            }
            operator = 0xFF;
        } else {
            word = *cursor;
            switch (word & 0xFF) {
            case 0xFF:
                cursor++;
                running = FALSE;
                break;
            case 4:
                operator = word >> 8;
                cursor++;
                break;
            }
        }
        want_operator ^= 1;
    } while (running);
    value = total;
    if (is_float) {
        bits.as_float = float_total;
        value = bits.as_int;
    }
    result.as_int.value = value;
    code = (u32)target.as_int.opcode >> 8;
    offset = target.as_int.value;
    switch (target.as_int.opcode & 0xFF) {
    case 1:
        func_00211A58(&result.as_int, code, offset);
        break;
    case 2:
        func_00211BC0(&result.as_int, thread, code, offset);
        break;
    case 3:
        func_00211AD0(&result.as_int, code, offset);
        break;
    }
    return cursor;
}
