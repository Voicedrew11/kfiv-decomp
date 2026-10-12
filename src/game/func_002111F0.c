#include "kf_script.h"

void func_00211810(ScriptThread *, u32 *, ScriptArg *);

/* The script's "if": works out up to 32 comparisons (float ones when either side has type 6), joins them with
   "or" (kind 0) or "and", and gives the number of words to skip when the answer is false, else 0. */
int func_002111F0(ScriptThread *thread, u32 *cursor) {
    int results[32];
    ScriptValue left;
    ScriptValue right;
    int i;
    u32 comparison;
    int count;
    int join_kind;
    int skip;
    int result;
    f32 left_float;
    f32 right_float;

    count = ((u8 *)cursor)[1];
    join_kind = ((u8 *)cursor)[0];
    cursor++;
    skip = *cursor;
    cursor++;
    for (i = 0; i < count; i++) {
        func_00211810(thread, cursor, &left.as_int);
        cursor += 2;
        comparison = *cursor;
        cursor++;
        func_00211810(thread, cursor, &right.as_int);
        cursor += 2;
        if (left.as_int.opcode == 6 || right.as_int.opcode == 6) {
            left_float = left.as_float.value;
            right_float = right.as_float.value;
            switch (comparison) {
            case 0:
                results[i] = left_float == right_float;
                break;
            case 1:
                results[i] = left_float != right_float;
                break;
            case 2:
                results[i] = left_float > right_float;
                break;
            case 3:
                results[i] = left_float < right_float;
                break;
            case 4:
                results[i] = left_float >= right_float;
                break;
            case 5:
                results[i] = left_float <= right_float;
                break;
            }
        } else {
            switch (comparison) {
            case 0:
                results[i] = left.as_int.value == right.as_int.value;
                break;
            case 1:
                results[i] = left.as_int.value != right.as_int.value;
                break;
            case 2:
                results[i] = left.as_int.value > right.as_int.value;
                break;
            case 3:
                results[i] = left.as_int.value < right.as_int.value;
                break;
            case 4:
                results[i] = left.as_int.value >= right.as_int.value;
                break;
            case 5:
                results[i] = left.as_int.value <= right.as_int.value;
                break;
            }
        }
    }
    result = results[0];
    if (count != 1) {
        if (join_kind == 0) {
            for (i = 1; i < count; i++) {
                result = result || results[i];
            }
        } else {
            for (i = 1; i < count; i++) {
                result = result && results[i];
            }
        }
    }
    thread->cursor = cursor;
    return result ? 0 : skip;
}
