#include "kf_game.h"

extern u16 D_00413DAA[];
extern u16 D_00446940;
extern char D_004471E0;
extern char D_004471E1;
extern MessageSlot *D_004471E4;
extern MessageSlot *D_004471E8;
extern MessageBox D_005C58E8;

int func_0020D088(void);
void func_001CF708(void);
s16 *func_001D0658(s16 *, s32);

/* Per-frame update of the message box (typing, paging, closing) and of the message ring. */
void func_001CF4D0(void) {
    MessageBox *box;

    func_0020D088();
    if (D_004471E0 == 0 && D_004471E1 == 0) {
        func_001CF708();
        return;
    }
    if (D_004471E0 == 1) {
        box = &D_005C58E8;
        if (box->unk14 < box->delay * box->typed + 8) {
            box->unk14++;
        }
        if (box->closing == 1) {
            if (--box->close_timer <= 0) {
                D_004471E0 = 0;
            }
        } else if (*box->cursor == -1) {
            if (D_00413DAA[0] & D_00446940) {
                box->closing = 1;
                box->close_timer = 8;
            }
        } else if (box->waiting == 1) {
            if (D_00413DAA[0] & D_00446940) {
                box->waiting = 0;
                box->start = box->end + 1;
                box->unk14 = 0;
                box->end = func_001D0658(box->start, box->font);
                box->typed = 0;
            }
        } else if (D_00413DAA[0] & D_00446940) {
            while (D_005C58E8.cursor < D_005C58E8.end) {
                if ((*D_005C58E8.cursor & ~0xFFF) == 0) {
                    D_005C58E8.typed++;
                }
                D_005C58E8.cursor++;
            }
            D_005C58E8.waiting = 1;
            D_005C58E8.timer = 0;
            D_005C58E8.unk14 = D_005C58E8.delay * D_005C58E8.typed + 8;
        } else {
            box->timer++;
            if (box->timer >= box->delay) {
                if (box->cursor >= box->end) {
                    box->waiting = 1;
                } else {
                    if ((*box->cursor & ~0xFFF) == 0) {
                        box->typed++;
                    }
                    box->timer = 0;
                    box->cursor++;
                }
            }
        }
    } else if (D_004471E1 == 1) {
        MessageSlot *slot = D_004471E4;

        if (--slot->unk04 < 0) {
            if (slot == D_004471E8) {
                D_004471E1 = 0;
            } else {
                D_004471E4 = slot->next;
                if (slot->next == D_004471E8) {
                    D_004471E1 = 0;
                }
            }
        }
    }
    func_001CF708();
}
