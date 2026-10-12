#include "kf_link.h"
#include "kf_load.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_state.h"
#include <eeregs.h>

#define LIST_PAGES 50
#define LIST_ROWS 7

extern u8 D_0034C350[LIST_PAGES][8];
extern u8 D_0034C4E0[LIST_PAGES];
extern s8 D_00349680[];
extern f32 D_00421080[4];
extern f32 D_00425510[0x100];       /* one alpha per menu part; [253] the cursor bar's, [254] the model's, [255] the text's */
extern LinkNode D_00426470;
extern f32 D_0042B7D0[16];
extern u32 D_00446A54;              /* how many parts the screen has */
extern int D_00446AD0;
extern void *D_00446B20;
extern s32 D_00446BC0;              /* the page shown, 0 to 49 */
extern s32 D_00446BC4;              /* the page shown the frame before */
extern MenuColor D_00446C78;
extern MenuColor D_00446C88;
extern LinkNode *D_00446C90;
extern u16 D_00446DE8;
extern s32 D_00446F38;
extern SpritePacket *D_00447250;
extern u8 D_004472C6;
extern s8 D_004472CA;               /* 0: fading in, 1: open, 2: fading out */
extern s8 D_004472D8;               /* the counter the cursor is on */
extern s8 D_004472D9;               /* the counter at the top of the list */
extern s8 D_004472DA;               /* nonzero while the screen takes input */
extern ChunkUpload D_005CEEE0;

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
int sceCdBreak(void);
int func_001024B0(int);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
void func_001D45D0(void);
void func_001D7C30(void);
f32 func_001D8300(f32, s32, s32);
void func_001DD738(HandlerContext *);
int func_001E5210(u8);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
int func_001FE350(int, int);
int func_001FEEA8(int, int);
void func_001FF428(LevelDef *, s32);
void func_00201E10(LinkNode *, f32 *, MenuColor); // match: the colour is loaded unaligned, so not the definition's types
void func_00207478(int, int, int, int);

/* The offset of text `number` in the text table, 0 when it has none. */
static inline s32 func_001DCA80_TextOffset(s32 table, s32 number) {
    return ((s32 *)table)[number];
}

/* The list screen: 50 pages of up to 46 entries, seven shown at a time under a cursor bar; picking one opens
   func_001DD738 with its text number in context->unk04. */
void func_001DCA80(HandlerContext *context) {
    GsPackedWord bar_origin;
    MenuCursorBar bar;
    s8 rows[LIST_PAGES];
    f32 view[16];
    f32 local[16];
    ChunkPoolNode node;
    GsPackedWord word;
    s32 i;
    s32 row_count;
    s32 table;
    s32 first_text;
    s32 entry;
    u32 *packet;
    PackageChunk *chunk;
    s32 show_picture;
    s32 row;
    u32 full_alpha;
    s32 text;
    s32 counter = 0;                /* one work variable: the pages tried, then a bit number, then a row */

    bar_origin.raw = 0x0001869F7C307EE0;
    /* left and right step to the next page that has entries */
    if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
        do {
            D_00446BC0 += LIST_PAGES + 1; // match: two statements; one expression computes the sum in another register
            D_00446BC0 %= LIST_PAGES;
            counter++; // match: counted before the test, so it sits in the branch's delay slot
            if (D_0034C4E0[D_00446BC0] != 0) {
                break;
            }
        } while (counter < LIST_PAGES);
        D_004472D9 = 0;
        if (D_00446BC4 != 0 || D_00446BC0 != 0) {
            D_004472D8 = 0;
        }
        if (D_00446BC4 != D_00446BC0) {
            func_0019CDC0(1);
        }
        D_00446BC4 = D_00446BC0;
    } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
        do {
            D_00446BC0 += LIST_PAGES - 1;
            D_00446BC0 %= LIST_PAGES;
            counter++; // match: counted before the test, so it sits in the branch's delay slot
            if (D_0034C4E0[D_00446BC0] != 0) {
                break;
            }
        } while (counter < LIST_PAGES);
        D_004472D9 = 0;
        if (D_00446BC4 != 0 || D_00446BC0 != 0) {
            D_004472D8 = 0;
        }
        if (D_00446BC4 != D_00446BC0) {
            func_0019CDC0(1);
        }
        D_00446BC4 = D_00446BC0;
    }

    /* the entries the page has: one bit each in its row of D_0034C350, from bit 3 on */
    for (i = 0; i < LIST_PAGES; i++) {
        rows[i] = -1;
    }
    row_count = 0;
    counter = 3;
    for (i = 3; i < LIST_PAGES - 1; i++) {
        if ((D_0034C350[D_00446BC0][i / 8] >> counter) & 1) {
            rows[row_count] = i;
            row_count++;
        }
        counter++;
        counter %= 8;
    }

    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        if (row_count >= 2) {
            func_0019CDC0(1);
        }
        if (D_004472D8 == D_004472D9 && row_count >= 8) {
            D_004472D9--;
            if (D_004472D9 < 0) {
                D_004472D9 = row_count - 1;
            }
        }
        D_004472D8--;
        if (D_004472D8 < 0) {
            D_004472D8 = row_count - 1;
        }
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        if (row_count >= 2) {
            func_0019CDC0(1);
        }
        if (((D_004472D9 < D_004472D8 && D_004472D8 - D_004472D9 >= 6) ||
             (D_004472D8 < D_004472D9 && D_004472D8 + row_count - D_004472D9 >= 6)) &&
            row_count >= 8) {
            D_004472D9++;
            if (D_004472D9 >= row_count) {
                D_004472D9 = 0;
            }
        }
        D_004472D8++;
        if (D_004472D8 >= row_count) {
            D_004472D8 = 0;
        }
    }

    for (i = 0; i < D_00446A54; i++) {
        func_001E5210(D_00447250[i].unk42);
        func_00207478((int)&D_00447250[i], 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    show_picture = D_00349680[7] != 0;
    if (D_005CEEE0.done != 0 && show_picture) {
        memset(view, 0, 0x40);
        view[10] = view[5] = view[0] = 0.8f;
        view[15] = 1.0f;
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        sceVu0TransMatrix(local, view, D_00421080);
        packet = func_001FBE70();
        packet[0] = 0x8002;
        packet[1] = 0x10000000;
        packet[2] = 0xE;
        packet[3] = 0;
        packet[4] = 0x7660;
        packet[5] = 0x7890;
        packet[6] = 0x19;
        packet[7] = 0;
        packet[8] = 0x44;
        packet[9] = 0;
        packet[10] = 0x43;
        packet[11] = 0;
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        chunk = (PackageChunk *)((char *)D_005CEEE0.pool_node->buffer + 0x10);
        for (i = 0; i < D_00446BC0; i++) {
            chunk = (PackageChunk *)((char *)chunk + chunk->unk0C + chunk->unk08);
        }
        if (D_004472C6 != 2 && D_004472C6 != 3 && D_00426470.unk20 != 0) {
            D_00425510[254] += 12.8f;
            if (D_00425510[254] > 128.0f) {
                D_00425510[254] = 128.0f;
            }
            D_00446C88.alpha = (s32)D_00425510[254];
        }
        D_00426470.pool_node = &node; // match: a local of this function; the node keeps the address after it returns
        D_00426470.unk20 = 1;
        node.buffer = chunk;
        func_00201E10(&D_00426470, local, D_00446C88);
    }

    table = func_001CFC58(D_00446F38);
    first_text = D_00446BC0 * LIST_PAGES + 3001;
    entry = func_001DCA80_TextOffset(table, first_text + LIST_PAGES - 1);
    word.raw = 0x000A875081106E80;
    func_001FDB88(word);
    if (entry != 0) {
        text = table + entry;
        func_001FDCD0(D_00446C78);
        func_001FF428((LevelDef *)text, 0x17);
    }
    entry = func_001DCA80_TextOffset(table, first_text + D_0034C4E0[D_00446BC0] - 1);
    word.raw = 0x000A875083106FE0;
    func_001FDB88(word);
    if (entry != 0) {
        text = table + entry;
        func_001FDCD0(D_00446C78);
        func_001FE350(text, 0x33);
    }

    for (i = 0; i < LIST_ROWS && i < row_count; i++) {
        counter = D_004472D9 + i;
        row = counter % row_count; // match: kept in a variable, which orders the compare's operands
        if (D_004472D8 == row) {
            bar.first.f.lo16 = bar_origin.f.lo16;
            bar.first.f.mid16 = bar_origin.f.mid16 + i * 0xB0;
            bar.first.f.mid24 = bar_origin.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1250;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_004472CA == 1) {
                D_00425510[253] = func_001D8300(D_00425510[253], 0x10, 0x20);
            }
            bar.alpha = (s32)D_00425510[253];
            func_001FC5E8(&bar);
            break;
        }
    }

    word.raw = 0x000AAE607C307F80;
    func_001FDB88(word);
    for (i = 0; i < LIST_ROWS && i < row_count; i++) {
        counter = D_004472D9 + i;
        entry = ((s32 *)table + first_text)[rows[counter % row_count]]; // match: the page's first text is added to the table, not to the row
        if (entry != 0) {
            text = table + entry;
            func_001FDCD0(D_00446C78);
            func_001FEEA8(text, 0x1C);
        }
    }

    if ((D_00446DE8 & 0x4000) && D_004472CA == 1 && D_004472DA != 0) {
        func_0019CDC0(0);
        D_00446B20 = (void *)func_001DD738;
        context->unk04 = D_00446BC0 * LIST_PAGES + rows[D_004472D8] + 3000;
        D_004472DA = 0;
    } else if ((D_00446DE8 & 0x2000) && D_004472CA == 1 && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472DA = 0;
        D_004472CA = 2;
    }

    switch (D_004472CA) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[253] += 1.6f;
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446A54; i++) {
            D_00425510[i] += D_00447250[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447250[i].alpha) {
                for (i = 0; i < D_00446A54; i++) {
                    full_alpha = D_00447250[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472CA = 1;
                break;
            }
        }
        break;
    case 2:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        D_00425510[254] -= 12.8f;
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00446C88.alpha = (s32)D_00425510[254];
        D_00425510[253] -= 1.6f;
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446A54; i++) {
            D_00425510[i] -= D_00447250[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_004472D9 = 0;
                D_00446B20 = (void *)func_001D7C30;
                D_004472D8 = 0;
                D_00446BC0 = 0;
                if (D_005CEEE0.done != 0) {
                    D_005CEEE0.done = 0;
                    D_005CEEE0.stage = 0;
                    D_00446C90 = NULL;
                    ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                } else {
                    sceCdBreak();
                    func_001024B0(D_00446AD0);
                    D_00446AD0 = -1;
                    D_005CEEE0.done = 0;
                    D_005CEEE0.stage = 0;
                    D_00446C90 = NULL;
                    ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                }
                D_004472CA = 0;
                break;
            }
        }
        break;
    }
}
