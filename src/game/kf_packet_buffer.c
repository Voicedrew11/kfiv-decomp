#include "common.h"
#include <eeregs.h>
#include "kf_gs.h"
#include "kf_packet.h"

void *memset(void *, int, u32);
int puts(char *);
void FlushCache(int); // match: void, the return value is unused and int changes the code

/* the game's own data and functions, outside this file */
extern int D_00446748;          /* which of the two packet buffers is being filled */
extern int D_0044674C;          /* quadwords handed out */
extern int D_00446750;          /* bytes in both buffers together */
extern int D_00446F58;          /* start of the packet area */
extern char D_0043E4A0[];
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);


static void *packet_chain_head[2];   /* 0x447018 first DMA tag of each buffer's chain */
static void *packet_chain_tail[2]; /* 0x447020 last DMA tag of each buffer's chain */
static int gs_send_pending[2];       /* 0x447028 [0] set while the GS reads a buffer; [1] (0x44702C) is never used */
static void *packet_buffer[2];        /* 0x447030 start of each buffer */
static void *packet_next;             /* 0x447038 next free quadword */
static int packet_overflowed;        /* 0x44703C the buffer overflowed: everything after goes to its start */
static int spare_area_addr;          /* 0x447040 the area past both buffers */
static int spare_area_size;          /* 0x447044 its size */

/* Starts filling the other packet buffer. */
void func_001095E0(void) {
    D_00446748 = D_00446748 == 0;
    packet_chain_head[D_00446748] = packet_chain_tail[D_00446748] = NULL;
    packet_next = packet_buffer[D_00446748];
    memset(packet_next, 0, D_00446750 / 2);
    D_0044674C = 0;
    packet_overflowed = 0;
}

/* Appends a packet (from packet to packet_end) to the chain of the buffer being filled. */
void func_00109650(u32 *packet, u32 *packet_end) {
    void **packet_head;
    void **packet_tail;

    packet_head = &packet_chain_head[D_00446748];
    packet_tail = &packet_chain_tail[D_00446748];
    if (packet_overflowed == 0) {
        if ((u32)packet_end >= (u32)packet_next) {
            puts(D_0043E4A0);
            while (1) {
            }
        } else {
            if (*packet_head != 0) {
                DmaTag *tail_tag = *packet_tail;
                tail_tag->id_and_qwc = 0x20000000;
                tail_tag->address = (int)packet;
                tail_tag->unk0C = tail_tag->unk08 = 0;
            } else {
                *packet_head = packet;
            }
            *packet_tail = (DmaTag *)packet_end;
        }
    }
}

/* Ends the chain of the buffer being filled and sends it to the GS by DMA. */
void func_001096E8(void) {
    void **chain_head;
    DmaTag *end_tag;

    chain_head = &packet_chain_head[D_00446748];
    if (*chain_head != 0) {
        if (packet_overflowed == 0) {
            gs_send_pending[0] = 1;
            func_00109A50();
            func_00109A80(GS_REG_FINISH, 0xFFFFFFFFUL);
            func_00109AD0();
            end_tag = packet_chain_tail[D_00446748];
            end_tag->id_and_qwc = 0x70000000;
            end_tag->address = 0;
            end_tag->unk08 = 0;
            end_tag->unk0C = 0;
            FlushCache(0);
            do {
            } while (*D1_CHCR & 0x100);
            *GS_CSR |= 2;
            FlushCache(0);
            *D1_QWC = 0;
            *D1_MADR = 0;
            *D1_TADR = (u32)*chain_head;
            *D1_CHCR = 0x105;
        }
    }
}

/* Waits for the GS to finish reading the last buffer sent. */
void func_00109800(void) {
    if (gs_send_pending[0] != 0) {
        gs_send_pending[0] = 0;
        do {
        } while (*D1_CHCR & 0x100);
        sceGsSyncPath(0, 0);
        do {
        } while ((*GS_CSR & 2) == 0);
    }
}

/* Lays out the packet buffers in the packet area for the given size (0 = largest). */
void func_00109878(int size_index) {
    u32 size;
    int base;
    int half;

    D_00446748 = 0;
    packet_chain_head[1] = NULL;
    packet_chain_head[0] = NULL;
    packet_chain_tail[1] = NULL;
    packet_chain_tail[0] = NULL;
    switch (size_index) {
    case 0:
        D_00446750 = 0x280000;
        break;
    case 1:
        D_00446750 = 0x180000;
        break;
    case 2:
        D_00446750 = 0x100000;
        break;
    case 3:
        D_00446750 = 0x80000;
        break;
    }
    size = D_00446750;
    base = D_00446F58;
    packet_buffer[0] = (u32 *)base;
    half = (int)(size + (size >> 31)) >> 1;
    base += half;
    packet_buffer[1] = (u32 *)base;
    memset(packet_buffer[0], 0, half);
    base += half;
    memset(packet_buffer[1], 0, half);
    D_0044674C = 0;
    packet_next = packet_buffer[0];
    packet_overflowed = 0;
    if (size_index == 0) {
        spare_area_addr = 0;
        spare_area_size = 0;
        return;
    }
    spare_area_addr = base;
    spare_area_size = 0x280000 - D_00446750;
}

/* Takes quadwords from the packet buffer; once it overflows everything gets the start of the buffer. */
u32 *func_00109990(int quadwords) {
    u32 *result = packet_next;

    D_0044674C += quadwords;
    if (packet_overflowed == 0) {
        if (D_00446750 / 16 >= D_0044674C) {
            packet_next = result + quadwords * 4;
            return result;
        }
        packet_overflowed = 1;
        packet_next = packet_buffer[D_00446748];
        memset(packet_next, 0, D_00446750 / 2);
    }
    return packet_next;
}

/* Returns the spare area's address and size. */
void func_00109A38(int *addr, int *size) {
    int spare_addr = spare_area_addr;
    int spare_size = spare_area_size;
    *addr = spare_addr;
    *size = spare_size;
}
