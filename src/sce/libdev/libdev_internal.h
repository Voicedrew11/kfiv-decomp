/*
 * sce/libdev/libdev_internal.h
 *
 * libdev's COP2 control-register transfers and the prototypes its members
 * call each other through.  The transfers are inline asm: the period compiler
 * has no operand for them.  Each prototype is the signature of the member
 * that defines the symbol, which includes this header.
 */
#ifndef SCE_LIBDEV_LIBDEV_INTERNAL_H
#define SCE_LIBDEV_LIBDEV_INTERNAL_H

#define LIBDEV_CFC2(dst, vi) __asm__ __volatile__("cfc2.ni %0, $" #vi : "=r"(dst))
#define LIBDEV_CTC2(src, vi) __asm__ __volatile__("ctc2.ni %0, $" #vi : : "r"(src))

/* VIF and VU0 register windows (EE user's manual addresses). */
#define LIBDEV_REG32(a) (*(volatile unsigned int *)(a))
#define VIF0_STAT   0x10003800
#define VIF0_FBRST  0x10003810
#define VIF0_ERR    0x10003820
#define VIF0_FIFO   0x10004000
#define VIF1_STAT   0x10003C00
#define VIF1_FBRST  0x10003C10
#define VIF1_ERR    0x10003C20
#define VIF1_FIFO   0x10005000

/* VIFn_STAT fields the libdev status functions test. */
#define VIF_STAT_VPS_MASK 0x3           /* bits 0-1, VPS: the VIF's pipeline state, 0 when idle */
#define VIF_STAT_WAITING_MASK 0xC       /* bits 2-3: waiting for the VU */
#define VIF_STAT_STOPPED_MASK 0x700     /* bits 8-10: stopped (VIF stop, forced break or interrupt) */
#define VIF_STAT_ERROR_MASK 0x3000      /* bits 12-13: stalled on an error */
#define VIF_STAT_FIFO_FULL 0x10000000   /* bit 28: the FIFO count's top bit, 16 quadwords queued */
#define VIF_STAT_FIFO_COUNT_MASK 0x1F000000 /* bits 24-28: quadwords waiting in the FIFO */
/* VPU_STAT: the running state of each VU (bit 0 of its nibble). */
#define VU_STAT_VU0_MASK 0xF
#define VU_STAT_VU0_RUNNING 0x1
#define VU_STAT_VU1_MASK 0xF00
#define VU_STAT_VU1_RUNNING 0x100

/* The other VIF registers: VIFn_REG(VIF0_STAT, VIF_CYCLE) and so on. */
#define VIF_MARK    0x030
#define VIF_CYCLE   0x040
#define VIF_MODE    0x050
#define VIF_NUM     0x060
#define VIF_MASK    0x070
#define VIF_CODE    0x080
#define VIF_ITOPS   0x090
#define VIF_BASE    0x0A0
#define VIF_OFST    0x0B0
#define VIF_TOPS    0x0C0
#define VIF_ITOP    0x0D0
#define VIF_TOP     0x0E0
#define VIF_ROW     0x100
#define VIF_COL     0x140
#define LIBDEV_VIF_REG32(stat, reg) LIBDEV_REG32((stat) + (reg))

typedef unsigned int LibdevUint128 __attribute__((mode(TI)));

/* One quadword of VU memory.  sceDevVu0PutMem takes one by value;
   sceDevVu0GetMem returns one by value, through the hidden pointer its
   prototype below spells out. */
typedef union {
    unsigned long dword[2];
    unsigned int word[4];
    unsigned short half[8];
} LibdevQuadword;

/* VU0's registers as read through COP2 (0x238 bytes). */
typedef struct {
    LibdevUint128 vector[32];
    unsigned int status;
    unsigned int mac;
    unsigned int clipping;
    unsigned int random;
    unsigned int imaginary;
    unsigned int quotient;
    unsigned short integer[16];
} LibdevVu0Registers;

/* VU1's registers as mapped into VU0 data memory at 0x4000 (0x23C bytes). */
typedef struct {
    LibdevUint128 vector[32];
    unsigned int status;
    unsigned int mac;
    unsigned int clipping;
    unsigned int random;
    unsigned int imaginary;
    unsigned int quotient;
    unsigned int pressure;
    unsigned short integer[16];
} LibdevVu1Registers;

/* A snapshot of VIF0 (0x38 bytes). */
typedef struct {
    unsigned int row[4];
    unsigned int col[4];
    unsigned int mask;
    unsigned int code;
    unsigned int stat;
    unsigned short itop;
    unsigned short itops;
    unsigned short mark;
    unsigned short num;
    unsigned char error;
    unsigned char cycle_length;
    unsigned char write_length;
    unsigned char mode;
} LibdevVif0Registers;

/* A snapshot of VIF1 (0x40 bytes). */
typedef struct {
    unsigned int row[4];
    unsigned int col[4];
    unsigned int mask;
    unsigned int code;
    unsigned int stat;
    unsigned short itop;
    unsigned short itops;
    unsigned short base;
    unsigned short offset;
    unsigned short top;
    unsigned short tops;
    unsigned short mark;
    unsigned short num;
    unsigned char error;
    unsigned char cycle_length;
    unsigned char write_length;
    unsigned char mode;
} LibdevVif1Registers;

int sceDevVu0IsRunning(void);
void sceDevVu0Pause(void);
int sceDevVu0Reset(void);
int sceDevVu0Continue(void);
void sceDevVu0PutStop(int on);
void sceDevVu0PutDbf(int on);
int sceDevVu0GetStop(void);
int sceDevVu0GetDbf(void);
int sceDevVu0GetStat(void);
void sceDevVu0Start(unsigned int addr);
unsigned int sceDevVu0GetTpc(void);
int sceDevVu0ChkIdle(void);
void sceDevVu0PutMem(unsigned short address, LibdevQuadword quadword);
void *sceDevVu0GetMem(void *dst, unsigned int addr);
unsigned short sceDevVu0GetTop(void);
void sceDevVu0PutTop(unsigned short top);
int sceDevVu0GetCnd(void);
int sceDevVu0ChkCnd(void);
int sceDevVu0Resume(void);
int sceDevVu0GetVi(LibdevVu0Registers *registers);
int sceDevVu0GetVf(LibdevVu0Registers *registers);
int sceDevVu0GetCtx(LibdevVu1Registers *registers);
int sceDevVu0PutCtx(LibdevVu1Registers *registers);
void sceDevVu0SetTBit(void);
void sceDevVu0PutEBit(int on);
void sceDevVu0PutMBit(int on);
int sceDevVu0GetEBit(void);
int sceDevVu0GetMBit(void);

void sceDevVif0Reset(void);
int sceDevVif0Pause(int mode);
int sceDevVif0Continue(void);
int sceDevVif0PutErr(int mask_interrupt, int mask_dmatag, int mask_vifcode);
unsigned int sceDevVif0GetErr(void);
int sceDevVif0GetStat(void);
int sceDevVif0GetRegs(LibdevVif0Registers *registers);
int sceDevVif0PutFifo(LibdevUint128 *source, int quadword_count);
void sceDevVif1Reset(void);
int sceDevVif1Pause(int mode);
int sceDevVif1Continue(void);
int sceDevVif1PutErr(int mask_interrupt, int mask_dmatag, int mask_vifcode);
unsigned int sceDevVif1GetErr(void);
int sceDevVif1GetStat(void);
int sceDevVif1GetRegs(LibdevVif1Registers *registers);
int sceDevVif1PutFifo(LibdevUint128 *source, int quadword_count);
int sceDevVif1GetFifo(LibdevUint128 *destination, int quadword_count);

#endif /* SCE_LIBDEV_LIBDEV_INTERNAL_H */
