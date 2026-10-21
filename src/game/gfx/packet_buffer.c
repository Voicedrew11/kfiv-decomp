/* The GS packet buffers: start-up (packet area and VU1 microprograms), the two double-buffered DMA
   chains, and lists of GS register writes. */
#include "common.h"
#include <eeregs.h>
#include <libdma.h>
#include <eekernel.h>
#include "gfx/gs.h"
#include "gfx/packet.h"
#include "core/file.h"
#include "core/memory.h"

/* Packet_LayOutBuffers' size_index: the size of the packet buffers (0x280000, 0x180000, 0x100000, 0x80000 bytes) */
#define PACKET_SIZE_LARGEST                 0
#define PACKET_SIZE_LARGE                   1
#define PACKET_SIZE_MEDIUM                  2
#define PACKET_SIZE_SMALL                   3

/* sends a packet to VIF1 and waits for the transfer to end */
#define SEND_TO_VIF1(packet)                    \
    channel = sceDmaGetChan(1);                 \
    channel->chcr |= D_CHCR_TTE;                \
    sceDmaSend(channel, (u32)(packet));         \
    while (*D1_CHCR & D_CHCR_STR) {             \
    }

/* the VIF1 packets that upload the VU1 microprograms, by the VU1 address each one starts at (the MSCAL
   that runs it) */

static void *packet_chain_head[2];   /* 0x447018 first DMA tag of each buffer's chain */
static void *packet_chain_tail[2]; /* 0x447020 last DMA tag of each buffer's chain */
static int gs_send_pending[2];       /* 0x447028 [0] set while the GS reads a buffer; [1] (0x44702C) is never used */
static void *packet_buffer[2];        /* 0x447030 start of each buffer */
static void *packet_next;             /* 0x447038 next free quadword */
static int packet_overflowed;        /* 0x44703C the buffer overflowed: everything after goes to its start */
static int spare_area_addr;          /* 0x447040 the area past both buffers */
static int spare_area_size;          /* 0x447044 its size */

/* Lays out the largest packet buffers and uploads the VU1 microprograms. */
void Packet_Init(void) {
    Packet_LayOutBuffers(0);
    Packet_UploadVu1Programs();
}

/* Uploads the five VU1 microprograms. */
void Packet_UploadVu1Programs(void) {
    DmaChan *channel;

    SEND_TO_VIF1(vu1_packed_vertex_program);
    SEND_TO_VIF1(vu1_gif_kick_program);
    SEND_TO_VIF1(vu1_float_vertex_program);
    SEND_TO_VIF1(vu1_posed_model_program);
    SEND_TO_VIF1(vu1_program_700);
}

/* Called before a frame's drawing; empty. */
void Packet_OnFrameStart(void) {
}

/* Called after a frame's drawing; empty. */
void Packet_OnFrameEnd(void) {
}

/* Starts filling the other packet buffer: its chain is emptied and the buffer cleared. */
void Packet_SwapBuffers(void) {
    packet_fill_index = packet_fill_index == 0;
    packet_chain_head[packet_fill_index] = packet_chain_tail[packet_fill_index] = NULL;
    packet_next = packet_buffer[packet_fill_index];
    memset(packet_next, 0, packet_buffers_size / 2);
    packet_quadwords_used = 0;
    packet_overflowed = 0;
}

/* Appends a packet (from packet to packet_end) to the chain of the buffer being filled. */
void Packet_AppendToChain(u32 *packet, u32 *packet_end) {
    void **packet_head;
    void **packet_tail;

    packet_head = &packet_chain_head[packet_fill_index];
    packet_tail = &packet_chain_tail[packet_fill_index];
    if (packet_overflowed == 0) {
        if ((u32)packet_end >= (u32)packet_next) {
            puts(packet_overflow_message);
            while (1) {
            }
        } else {
            if (*packet_head != 0) {
                DmaTag *tail_tag = *packet_tail;
                tail_tag->id_and_qwc = 0x20000000;
                tail_tag->address = (int)packet;
                tail_tag->spare_word_high = tail_tag->spare_word_low = 0;
            } else {
                *packet_head = packet;
            }
            *packet_tail = (DmaTag *)packet_end;
        }
    }
}

/* Ends the chain of the buffer being filled (with a GS FINISH write and an END tag) and starts its DMA to
   VIF1. */
void Packet_SendChain(void) {
    void **chain_head;
    DmaTag *end_tag;

    chain_head = &packet_chain_head[packet_fill_index];
    if (*chain_head != 0) {
        if (packet_overflowed == 0) {
            gs_send_pending[0] = 1;
            Packet_BeginGsRegisters();
            Packet_AddGsRegister(GS_REG_FINISH, 0xFFFFFFFFUL);
            Packet_EndGsRegisters();
            end_tag = packet_chain_tail[packet_fill_index];
            end_tag->id_and_qwc = 0x70000000;
            end_tag->address = 0;
            end_tag->spare_word_low = 0;
            end_tag->spare_word_high = 0;
            FlushCache(0);
            do {
            } while (*D1_CHCR & D_CHCR_STR);
            *GS_CSR |= GS_CSR_FINISH;
            FlushCache(0);
            *D1_QWC = 0;
            *D1_MADR = 0;
            *D1_TADR = (u32)*chain_head;
            *D1_CHCR = D_CHCR_STR | D_CHCR_DIR_FROM_MEMORY | D_CHCR_MOD_CHAIN;
        }
    }
}

/* Waits for the GS to finish reading the last buffer sent. */
void Packet_WaitForSend(void) {
    if (gs_send_pending[0] != 0) {
        gs_send_pending[0] = 0;
        do {
        } while (*D1_CHCR & D_CHCR_STR);
        sceGsSyncPath(0, 0);
        do {
        } while ((*GS_CSR & GS_CSR_FINISH) == 0);
    }
}

/* Lays out the packet buffers in the packet area for the given size (0 = largest). */
void Packet_LayOutBuffers(int size_index) {
    u32 size;
    int base;
    int half;

    packet_fill_index = 0;
    packet_chain_head[1] = NULL;
    packet_chain_head[0] = NULL;
    packet_chain_tail[1] = NULL;
    packet_chain_tail[0] = NULL;
    switch (size_index) {
    case PACKET_SIZE_LARGEST:
        packet_buffers_size = 0x280000;
        break;
    case PACKET_SIZE_LARGE:
        packet_buffers_size = 0x180000;
        break;
    case PACKET_SIZE_MEDIUM:
        packet_buffers_size = 0x100000;
        break;
    case PACKET_SIZE_SMALL:
        packet_buffers_size = 0x80000;
        break;
    }
    size = packet_buffers_size;
    base = packet_area;
    packet_buffer[0] = (u32 *)base;
    half = (int)(size + (size >> 31)) >> 1;
    base += half;
    packet_buffer[1] = (u32 *)base;
    memset(packet_buffer[0], 0, half);
    base += half;
    memset(packet_buffer[1], 0, half);
    packet_quadwords_used = 0;
    packet_next = packet_buffer[0];
    packet_overflowed = 0;
    if (size_index == 0) {
        spare_area_addr = 0;
        spare_area_size = 0;
        return;
    }
    spare_area_addr = base;
    spare_area_size = 0x280000 - packet_buffers_size;
}

/* Takes quadwords from the packet buffer; once it overflows everything gets the start of the buffer. */
u32 *Packet_Alloc(int quadwords) {
    u32 *result = packet_next;

    packet_quadwords_used += quadwords;
    if (packet_overflowed == 0) {
        if (packet_buffers_size / 16 >= packet_quadwords_used) {
            packet_next = result + quadwords * 4;
            return result;
        }
        packet_overflowed = 1;
        packet_next = packet_buffer[packet_fill_index];
        memset(packet_next, 0, packet_buffers_size / 2);
    }
    return packet_next;
}

/* Returns the spare area's address and size. */
void Packet_GetSpareArea(int *addr, int *size) {
    int spare_addr = spare_area_addr;
    int spare_size = spare_area_size;
    *addr = spare_addr;
    *size = spare_size;
}

/* Starts a list of GS register writes. */
void Packet_BeginGsRegisters(void) {
    if (gs_register_list == NULL) {
        gs_register_list = Packet_Alloc(3);
        gs_register_count = 0;
    }
}

/* Adds a GS register write to the list. */
void Packet_AddGsRegister(int reg, s64 value) {
    if (gs_register_list != NULL) {
        s64 *entry = (s64 *)Packet_Alloc(1);

        entry[0] = value;
        entry[1] = reg;
        gs_register_count++;
    }
}

/* Ends the list of GS register writes: fills in its DMA, VIF and GIF tags and queues it. */
void Packet_EndGsRegisters(void) {
    u32 *packet;
    int i;
    int count;
    int blocks;
    u64 descriptors = 0xE;

    if (gs_register_list != NULL) {
        packet = gs_register_list;
        i = 0;
        *packet++ = (gs_register_count + 4) | DMA_TAG_ID_CNT;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = ((gs_register_count + 1) << 16) | VIF_UNPACK_V4_32_ADDR_C000;
        count = gs_register_count / 16;
        blocks = count;
        count = gs_register_count - blocks * 16;
        *(u64 *)packet = (u64)((blocks + 1) | GIF_TAG_EOP) | ((u64)count << 60);
        packet += 2;
        *(u64 *)packet = 0;
        /* one "A+D" descriptor per register */
        if (count > 0) {
            // match: a for loop is turned into a count-down
            do {
                // match: volatile keeps the load and the store inside the loop
                *(volatile u64 *)packet |= descriptors;
                descriptors <<= 4;
                i++;
            } while (i < count);
        }
        packet = Packet_Alloc(2);
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x15000276;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x13000000;
        Packet_AppendToChain(gs_register_list, Packet_Alloc(1));
        gs_register_list = NULL;
        gs_register_count = 0;
    }
}
