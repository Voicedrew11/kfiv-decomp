/*
 * sce/libkernl/eeregs.h
 *
 * The EE's memory-mapped I/O registers, each defined once as the volatile
 * pointer the tree's accesses use, so `*D4_CHCR = 0x101;` is the same store
 * the literal cast was.  A site that reads a register with a different type
 * (unsigned, 64-bit, or without the qualifier) keeps that cast on the name.
 * The three quadword FIFOs are untyped: each TU names its own 128-bit type
 * (u_long128, u128, u128_ipu) and casts the FIFO to it.  A mask defined under a
 * register is one of that register's bit fields, from the same public map.
 *
 * The header is this project's own.  Its file name follows the public naming
 * of the PS2 SDK header for these registers; nothing is copied from an SDK
 * header.  Names the disc prints or links are unflagged: D1_CHCR, D1_MADR,
 * D1_QWC, D1_TADR, D2_CHCR, D2_MADR, D2_QWC, D2_TADR, VIF1_STAT and GIF_STAT
 * are the labels of graph012.c's register dump, D_CTRL the label of
 * mv_main.c's debug print, and D3_CHCR and D4_CHCR are named by libmpeg's
 * setD3_CHCR and setD4_CHCR.  Every other name is the public EE hardware
 * map's name for that address, marked per block.
 *
 * It lives in libkernl because the SDK archives are its heaviest users and
 * this directory is on every TU's include path, game and SDK alike.
 */
#ifndef SCE_LIBKERNL_EEREGS_H
#define SCE_LIBKERNL_EEREGS_H

/* Timers. */ /* derived name */
#define T0_COUNT ((volatile int *)0x10000000)
#define T0_MODE ((volatile int *)0x10000010)
#define TIMER_MODE_EQUF 0x400 /* T0_MODE bit 10, EQUF: the compare flag; Timer_CountOverflow writes it to acknowledge */
#define T1_COUNT ((volatile int *)0x10000800)
#define T1_MODE ((volatile int *)0x10000810)
#define T3_MODE ((volatile int *)0x10001810)

/* IPU. */ /* derived name */
#define IPU_CMD ((volatile int *)0x10002000)
#define IPU_CTRL ((volatile int *)0x10002010)
#define IPU_BP ((volatile int *)0x10002020)
#define IPU_TOP ((volatile int *)0x10002030)
#define IPU_CTRL_OFC_MASK 0xF0 /* IPU_CTRL bits 4-7, OFC: the output FIFO's qword count */
#define IPU_BP_BIT_POSITION_MASK 0x7F /* IPU_BP bits 0-6, BP: the bit position in the current qword */
#define IPU_BP_IFC_SHIFT 8 /* IPU_BP bits 8-11, IFC: the input FIFO's qword count */
#define IPU_BP_IFC_MASK 0xF
#define IPU_BP_FP_SHIFT 16 /* IPU_BP bits 16-17, FP: the qwords held in the bit decoder's buffer (0 to 2) */
#define IPU_BP_FP_MASK 0x3
#define IPU_in_FIFO ((volatile void *)0x10007010)

/* GIF. */
#define GIF_CTRL ((volatile int *)0x10003000) /* derived name */
#define GIF_STAT ((volatile int *)0x10003020)
#define GIF_STAT_APATH 0xC00 /* derived name; bits 10-11, the path in use (0 = idle) */

/* VIF0. */ /* derived name */
#define VIF0_FBRST ((volatile int *)0x10003810)
#define VIF0_ERR ((volatile int *)0x10003820)
#define VIF0_MARK ((volatile int *)0x10003830)
#define VIF0_FIFO ((volatile void *)0x10004000)

/* VIF1. */
#define VIF1_STAT ((volatile int *)0x10003C00)
#define VIF1_FBRST ((volatile int *)0x10003C10) /* derived name */
#define VIF1_FIFO ((volatile void *)0x10005000)  /* derived name */

/* DMA channel 0, VIF0. */ /* derived name */
#define D0_CHCR ((volatile int *)0x10008000)

/* DMA channel 1, VIF1. */
#define D1_CHCR ((volatile int *)0x10009000)
#define D1_MADR ((volatile int *)0x10009010)
#define D1_QWC ((volatile int *)0x10009020)
#define D1_TADR ((volatile int *)0x10009030)

/* DMA channel 2, GIF. */
#define D2_CHCR ((volatile int *)0x1000A000)
#define D2_MADR ((volatile int *)0x1000A010)
#define D2_QWC ((volatile int *)0x1000A020)
#define D2_TADR ((volatile int *)0x1000A030)

/* DMA channel 3, from IPU. */
#define D3_CHCR ((volatile int *)0x1000B000)
#define D3_MADR ((volatile int *)0x1000B010) /* derived name */
#define D3_QWC ((volatile int *)0x1000B020)  /* derived name */

/* DMA channel 4, to IPU. */
#define D4_CHCR ((volatile int *)0x1000B400)
#define D4_MADR ((volatile int *)0x1000B410) /* derived name */
#define D4_QWC ((volatile int *)0x1000B420)  /* derived name */
#define D4_TADR ((volatile int *)0x1000B430) /* derived name */

/* DMA channels 5 to 7, SIF0 to SIF2. */ /* derived name */
#define D5_CHCR ((volatile int *)0x1000C000)
#define D6_CHCR ((volatile int *)0x1000C400)
#define D7_CHCR ((volatile int *)0x1000C800)

/* DMA channel 8, from scratchpad. */ /* derived name */
#define D8_CHCR ((volatile int *)0x1000D000)
#define D8_MADR ((volatile int *)0x1000D010)
#define D8_QWC ((volatile int *)0x1000D020)
#define D8_SADR ((volatile int *)0x1000D080)

/* DMA channel 9, to scratchpad. */ /* derived name */
#define D9_CHCR ((volatile int *)0x1000D400)
#define D9_MADR ((volatile int *)0x1000D410)
#define D9_QWC ((volatile int *)0x1000D420)
#define D9_TADR ((volatile int *)0x1000D430)
#define D9_SADR ((volatile int *)0x1000D480)

/* Dn_CHCR fields, shared by every channel's control register. */ /* derived name */
#define D_CHCR_DIR_FROM_MEMORY 0x1 /* bit0: DIR, the transfer reads memory */
#define D_CHCR_MOD_CHAIN 0x4       /* bits2-3: MOD, DMAtag chain */
#define D_CHCR_TTE 0x40            /* bit6: TTE, transfer the DMAtag too */
#define D_CHCR_STR 0x100           /* bit8: STR, start; set while the transfer runs */
#define D_MADR_SPR 0x80000000      /* MADR bit 31: the address is in the scratchpad */
#define D_CHCR_TAG_ID_BITS 0xF0000000 /* CHCR bits 28-31: the DMAtag id and IRQ bits a channel takes from its tag */

/* DMAtag words: the id in bits 28-30, the address below it. */
#define DMA_TAG_ID_MASK 0x70000000 /* the id field */
#define DMA_TAG_ID_CNT 0x10000000  /* id 1: the data follows the tag */
#define DMA_TAG_ID_REF 0x30000000  /* id 3: the data is at the tag's address */
#define DMA_TAG_ID_END 0x70000000  /* id 7: the data ends the chain */

/* the top bits of an address pick its alias: uncached, and uncached accelerated */
#define EE_UNCACHED_ALIAS 0x20000000
#define EE_UNCACHED_ACCELERATED_ALIAS 0x30000000
#define EE_PHYSICAL_ADDRESS_MASK 0x0FFFFFFF /* strips the segment bits: an address's physical part */

/* DMA controller. */
#define D_CTRL ((volatile int *)0x1000E000)
#define D_CTRL_DMAE 0x1 /* D_CTRL bit 0, DMAE: the DMA controller is enabled */
#define D_CTRL_RELE 0x2 /* D_CTRL bit 1, RELE: releases the bus cycle */
#define D_STAT ((volatile int *)0x1000E010)    /* derived name */
#define D_PCR ((volatile int *)0x1000E020)     /* derived name */
#define D_SQWC ((volatile int *)0x1000E030)    /* derived name */
#define D_RBSR ((volatile int *)0x1000E040)    /* derived name */
#define D_RBOR ((volatile int *)0x1000E050)    /* derived name */
#define D_STADR ((volatile int *)0x1000E060)   /* derived name */
#define D_ENABLER ((volatile int *)0x1000F520) /* derived name */
#define D_ENABLEW ((volatile int *)0x1000F590) /* derived name */
#define D_ENABLE_HOLD 0x10000 /* D_ENABLER/W bit 16: holds the DMA controller while a channel's CHCR is written */

/* Interrupt controller. */ /* derived name */
#define INTC_STAT ((volatile int *)0x1000F000)

/* GS privileged registers, 64 bits wide. */ /* derived name */
#define GS_PMODE ((volatile long *)0x12000000)
#define GS_SMODE2 ((volatile long *)0x12000020)
#define GS_DISPFB1 ((volatile long *)0x12000070)
#define GS_DISPLAY1 ((volatile long *)0x12000080)
#define GS_DISPFB2 ((volatile long *)0x12000090)
#define GS_DISPLAY2 ((volatile long *)0x120000A0)
#define GS_EXTDATA ((volatile long *)0x120000C0)
#define GS_BGCOLOR ((volatile long *)0x120000E0)
#define GS_CSR ((volatile unsigned long *)0x12001000)
#define GS_BUSDIR ((volatile unsigned long *)0x12001040)
#define GS_CSR_FINISH 0x2       /* bit1: FINISH, the GS finished a drawing; written 1 to clear */
#define GS_CSR_FIELD 0x2000     /* bit13: FIELD, the field the GS is on */

#endif /* SCE_LIBKERNL_EEREGS_H */
