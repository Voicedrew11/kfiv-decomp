#include "kf_game.h"

/* EE EABI varargs: the register save area a6..a7 is spilled to, as in func_001CF988 */
typedef char *va_list;

extern char D_004471E1;
extern MessageSlot *D_004471E4;
extern MessageSlot *D_004471E8;

int func_001D07C8(int);
int func_001CFDB8(u16 *, u16 *, va_list);
int func_001D05E0(s16 *, int);

/* Formats message `message_id` into a slot of the message ring, centred on (x + 320, y + 400).
   kind 1 overwrites the slot being shown; any other kind takes the next free slot, or does
   nothing when the ring is full. */
void func_001CFA70(int x, int y, int font, int kind, int frames, int message_id, ...) {
    va_list args;
    int format;
    int width;

    format = func_001D07C8(message_id);
    if (kind == 1) {
        int left; // match: the two branches have their own left and top
        int top;

        args = (char *)__builtin_next_arg(message_id) - 0x10;
        D_004471E4->length = func_001CFDB8(format, (int *)D_004471E4->text, args);
        width = func_001D05E0((s16 *)D_004471E4->text, font);
        left = x + 0x140;
        top = y + 0x190;
        D_004471E4->unk08 = frames;
        D_004471E4->unk04 = frames;
        D_004471E4->unk1C = font;
        D_004471E4->unk14 = left - width / 2;
        D_004471E4->unk18 = top;
        D_004471E4->unk20 = kind;
    } else {
        int left;
        int top;

        if (D_004471E8->next == D_004471E4) {
            return;
        }
        args = (char *)__builtin_next_arg(message_id) - 0x10;
        D_004471E8->length = func_001CFDB8(format, (int *)D_004471E8->text, args);
        width = func_001D05E0((s16 *)D_004471E8->text, font);
        left = x + 0x140;
        top = y + 0x190;
        D_004471E8->unk08 = frames;
        D_004471E8->unk04 = frames;
        D_004471E8->unk1C = font;
        D_004471E8->unk14 = left - width / 2;
        D_004471E8->unk18 = top;
        D_004471E8->unk20 = 0;
        D_004471E8 = D_004471E8->next;
    }
    D_004471E1 = 1;
}
