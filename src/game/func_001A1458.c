#include "common.h"
#include "kf_sound.h"

int FlushCache(int);
int func_001A1360(char *, char *, SoundJobSpec *, int, int);
int func_001A1438(void);
int func_001CD6B8(int);
int func_001CD7E0(int);

/* Loads the two files named in the slot, then flushes the cache. */
void func_001A1458(SoundJob *slot) {
    while (func_001A1438() == 0);
    slot->result = func_001A1360(slot->name_a, slot->name_b, slot->spec, slot->unk18, slot->unk1C);
    while (func_001A1438() == 0);
    while (func_001CD6B8(0) == 0);
    func_001CD7E0(slot->result);
    FlushCache(0);
}
