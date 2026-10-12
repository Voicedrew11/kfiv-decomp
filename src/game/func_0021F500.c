#include "common.h"

int func_0021F098(s64, s64, s64);

/* Packs six fields into one 64-bit word and sends it to func_0021F098 with a count of 8. */
int func_0021F500(s64 cmd_id, u64 word0, u64 word1, u64 word2, u64 word3, u64 word4, u64 word5) {
    u64 low = ((word0 << 32) >> 32) | (word5 << 34);
    u64 mid = ((word3 << 32) >> 18) | ((word2 << 32) >> 28);
    u64 high = ((word1 << 32) >> 30) | ((word4 << 32) >> 8);

    return func_0021F098(cmd_id, 8, low | mid | high);
}
