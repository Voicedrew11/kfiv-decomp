#include "kf_script.h"

extern ScriptThread D_01DFB3B0[];

/* TRUE while the script slot `index` is in use. */
int func_00210C28(int index) {
    return D_01DFB3B0[index].script_id != 0;
}
