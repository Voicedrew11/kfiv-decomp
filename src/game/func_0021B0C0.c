#include "kf_mc.h"

int func_002183C0(void);
int func_00218570(void);
int func_002186E0(void);
int func_00218F68(void);
int func_00219700(void);
int func_00219CD8(void);
int func_0021A7F8(void);
int func_0021A910(void);
int func_0021AA40(void);
int func_0021A368(void);
int func_0021AB58(void);

/* Steps the memory card request in progress. Returns 0 when idle or just finished, the request's number
   while it runs, and minus that number when it failed. */
int func_0021B0C0(void) {
    int result;

    switch (D_01E4F278.state) {
    case 3:
        result = func_002183C0();
        break;
    case 4:
        result = func_00218570();
        break;
    case 5:
        result = func_002186E0();
        break;
    case 6:
        result = func_00218F68();
        break;
    case 7:
        result = func_00219700();
        break;
    case 8:
        result = func_00219CD8();
        break;
    case 9:
        result = func_0021A7F8();
        break;
    case 10:
        result = func_0021A910();
        break;
    case 11:
        result = func_0021AA40();
        break;
    case 12:
        result = func_0021A368();
        break;
    case 13:
        result = func_0021AB58();
        break;
    case 0:
        return 0;
    default:
        result = -1;
        break;
    }
    if (result < 0) {
        result = -D_01E4F278.state;
        D_01E4F278.state = 0;
        D_01E4F278.unk8C = 0;
        if (result == 0) {
            return -100;
        }
        return result;
    }
    if (result > 0) {
        D_01E4F278.state = 0;
        D_01E4F278.unk8C = 0;
        return 0;
    }
    return D_01E4F278.state;
}
