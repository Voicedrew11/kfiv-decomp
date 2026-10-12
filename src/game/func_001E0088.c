#include "kf_menu.h"

#define PAGE_ROWS 7

extern u16 D_00446DE8;
extern s32 D_00446C38;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern MenuItem *D_005CE850[];
extern MenuItem *D_005CE860[];
extern MenuItem *D_005CE870[];
s32 func_0019CDC0(s32);
void func_001D4C60(void);
void func_001E4858(s32);
void func_001E4A48(s32);
void func_001E4C38(s32);
void func_001E4DC8(s32);

/* Scrolls the window and the cursor up one row; FALSE once the window is at the top of the list. */
static inline s32 func_001E0088_ScrollUp(void) {
    D_005CE860[0] = D_005CE860[0]->prev;
    D_005CE870[0] = D_005CE870[0]->prev;
    D_005CE850[0] = D_005CE860[0];
    return D_005CE860[0] != D_00446C28;
}

/* Scrolls the window and the cursor down one row; FALSE once the window is at the end of the list. */
static inline s32 func_001E0088_ScrollDown(void) {
    D_005CE860[0] = D_005CE860[0]->next;
    D_005CE870[0] = D_005CE870[0]->next;
    D_005CE850[0] = D_005CE860[0];
    return D_005CE860[0] != D_00446C24;
}

/* Moves the list cursor one row (0x10, 0x40) or one page (0x20, 0x80, lists of 8 and more), scrolling the window. */
s32 func_001E0088(void) {
    MenuItem *cursor = D_005CE850[0];
    MenuItem *next;
    s32 rows;

    if (cursor != NULL) {
        if (D_00446DE8 & 0x10) {
            next = cursor->next;
            if (next != NULL) {
                if (cursor == D_005CE860[0] && next != D_005CE870[0]->prev) {
                    D_005CE860[0] = next;
                    D_005CE870[0] = D_005CE870[0]->next;
                }
                func_0019CDC0(1);
                D_005CE850[0] = cursor->next;
            }
            func_001E4858(0);
        } else if (D_00446DE8 & 0x40) {
            if (cursor->prev != NULL) {
                if (D_005CE870[0]->prev == cursor && D_005CE860[0]->next != cursor) {
                    D_005CE870[0] = cursor;
                    D_005CE860[0] = D_005CE860[0]->prev;
                }
                func_0019CDC0(1);
                D_005CE850[0] = cursor->prev;
            }
            func_001E4A48(0);
        } else if ((D_00446DE8 & 0x20) && D_00446C38 >= 8) {
            if (cursor->prev != NULL) {
                if (D_005CE860[0]->next != D_005CE870[0]->prev) {
                    rows = 0;
                    while (func_001E0088_ScrollUp()) {
                        if (++rows >= PAGE_ROWS) {
                            break;
                        }
                    }
                }
                func_0019CDC0(1);
            }
            func_001E4C38(0);
        } else if ((D_00446DE8 & 0x80) && D_00446C38 >= 8) {
            if (cursor->next != NULL) {
                if (D_005CE860[0]->next != D_005CE870[0]->prev) {
                    rows = 0;
                    while (func_001E0088_ScrollDown()) {
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
        func_001D4C60();
        return TRUE;
    }
    return FALSE;
}
