#include "kf_menu.h"
#include "kf_state.h"

#define PAGE_ROWS 7

extern u16 D_00446DE8;
extern s32 D_00446C48;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
s32 func_0019CDC0(s32);
void func_001E4660(void);
void func_001E4858(s32);
void func_001E4A48(s32);
void func_001E4C38(s32);
void func_001E4DC8(s32);

/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 func_001DFC60_ScrollUp(void) {
    D_00426120[0] = D_00426120[0]->prev;
    D_00426130[0] = D_00426130[0]->prev;
    D_00426110[0] = D_00426120[0];
    return D_00426120[0] != D_00446C44;
}

/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 func_001DFC60_ScrollDown(void) {
    D_00426120[0] = D_00426120[0]->next;
    D_00426130[0] = D_00426130[0]->next;
    D_00426110[0] = D_00426120[0];
    return D_00426120[0] != D_00446C40;
}

/* Like func_001E0088 for the D_00426110 list: moves the cursor a row or a page, unless the list is locked. */
s32 func_001DFC60(HandlerContext *context, s32 locked) {
    MenuItem *cursor = D_00426110[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if ((D_00446DE8 & 0x10) && !locked) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == D_00426120[0] && next != D_00426130[0]->prev) {
                    D_00426120[0] = next;
                    D_00426130[0] = D_00426130[0]->next;
                }
                D_00426110[0] = next;
                func_0019CDC0(1);
            }
            func_001E4858(0);
        } else if ((D_00446DE8 & 0x40) && !locked) {
            if (cursor->prev != NULL) {
                if (D_00426130[0]->prev == D_00426110[0] && D_00426120[0]->next != D_00426130[0]->prev) {
                    D_00426130[0] = D_00426130[0]->prev;
                    D_00426120[0] = D_00426120[0]->prev;
                }
                func_0019CDC0(1);
                D_00426110[0] = cursor->prev;
            }
            func_001E4A48(0);
        } else if ((D_00446DE8 & 0x20) && D_00446C48 >= 8 && !locked) {
            if (cursor->prev != NULL) {
                if (D_00426120[0]->next != D_00426130[0]->prev) {
                    rows = 0;
                    while (func_001DFC60_ScrollUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                func_0019CDC0(1);
            }
            func_001E4C38(0);
        } else if ((D_00446DE8 & 0x80) && D_00446C48 >= 8 && !locked) {
            if (cursor->next != NULL) {
                if (D_00426120[0]->next != D_00426130[0]->prev) {
                    rows = 0;
                    while (func_001DFC60_ScrollDown()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                func_0019CDC0(1);
            }
            func_001E4DC8(0);
        } else {
            return FALSE;
        }
        func_001E4660();
        return TRUE;
    }
    return FALSE;
}
