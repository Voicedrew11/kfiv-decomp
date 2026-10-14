/* The save screens' memory card work (kf_save_card.c). */
#ifndef KF_SAVE_CARD_H
#define KF_SAVE_CARD_H

#include "common.h"
#include "kf_game.h"

/* what a card task is doing (CardTask.kind) */
#define CARD_TASK_NONE 0
#define CARD_TASK_CHECK 1
#define CARD_TASK_FORMAT 2
#define CARD_TASK_READ_LIST 3
#define CARD_TASK_SAVE 4
#define CARD_TASK_LOAD 5
#define CARD_TASK_RESCAN 6
#define CARD_TASK_FAILED 0x63

/* a card task and where it has got to. There are two: the check of both cards that runs in the background
   (card_check) and the task the screens asked for (card_task). */
typedef struct {
    s32 card_index;                 /* 0x00 the card the task works on, 0 or 1 */
    char pad04[0x0C];
    s16 kind;                       /* 0x10 CARD_TASK_ */
    u16 last_kind;                  /* 0x12 the kind that was started: kind becomes CARD_TASK_FAILED on a failure */
    s8 step;                        /* 0x14 */
    s8 retries;                     /* 0x15 a request is given up after five */
    s8 fail_step;                   /* 0x16 step of SaveCard_Fail */
    s8 fail_retries;                /* 0x17 */
    s8 keep_open;                   /* 0x18 card_check only: cleared by SaveCard_Close */
    char pad19[0x01];
    s8 save_index;                  /* 0x1A which of the three saves is written or read */
    s8 check_done;                  /* 0x1B card_check only: a check has finished */
    s8 check_running;               /* 0x1C card_task only: set while SaveCard_Update steps the check */
    char pad1D[0x03];
} CardTask;

/* CardRecord.status: read whole in one place, a byte at a time everywhere else */
typedef union {
    u16 word;
    struct {
        char check_state;           /* 0x20 0 checked, 1 not checked yet, 2 the check or a task failed */
        char card_type;             /* 0x21 what the card check reported; 2 is a PS2 memory card */
    } bytes;
} CardStatus;

/* what is known of the card in one port (0x28 bytes) */
typedef struct {
    char no_card;                   /* 0x00 nothing answered in the port */
    char present;                   /* 0x01 */
    char formatted;                 /* 0x02 */
    char has_save_folder;           /* 0x03 the game's directory is on the card */
    char has_save_list;             /* 0x04 the list file was read or written */
    char icon_missing;              /* 0x05 the icon file is not there, or has the wrong size */
    char damaged;                   /* 0x06 one of the game's files is missing */
    char load_failed;               /* 0x07 */
    s32 *save_data;                 /* 0x08 the save image to write or to read into; its first word is its size */
    void *icon_data;                /* 0x0C the icon file */
    void *unk10;                    /* 0x10 the icon file again; never read */
    void *unk14;                    /* 0x14 */
    s32 free_clusters;              /* 0x18 */
    s32 save_data_size;             /* 0x1C */
    CardStatus status;              /* 0x20 */
    char pad22[0x02];
    s16 message;                    /* 0x24 which card message the screen shows: 0 none, 1 no card, 2 not usable, 3 not formatted, 4 and up failures */
    char pad26[0x02];
} CardRecord;

/* one record of the list file, which holds three (0x180 bytes each): what the save list shows of a save.
   CardSaveInfo in kf_menu.h is the menu's view of the same record. */
typedef struct {
    char exists;                    /* 0x00 */
    char damaged;                   /* 0x01 the list names the save but its file is gone, or writing it failed */
    char pad02[0x02];
    int numbers[10];                /* 0x04 D_0038CC84[0] and nine words of the player state (SaveCard_FillSaveInfo) */
    char pad2C[0x14C];
    PlayTime play_time;             /* 0x178 */
} SaveInfo;

/* Starts the memory card library and clears both task records. Returns -1 when the library does not start. */
s32 SaveCard_Init(void);

/* Gets the card work ready for the save screens: takes 0x800 bytes for the save list and the directory entries,
   loads the icon file, forgets what was known of both cards and starts checking them. Returns -1 while a card
   request is still running. */
s32 SaveCard_Open(void);

/* Once a frame: steps the check of both cards while one is wanted and no card is known to have failed, then
   the task in progress. Returns 0 while there is work left, 1 when a step finished or nothing runs, and -1 after
   a failure or once SaveCard_Close has taken effect (the buffers are dropped then). */
int SaveCard_Update(void);

/* Asks for both cards to be checked again, unless a task other than a failed one is running; with reset set,
   also forgets what was known of them. */
s32 SaveCard_StartCheck(s8 reset);

/* Starts reading the save list of a card, unless a task is running. With must_exist set, a card without a
   readable list counts as a failure. */
s32 SaveCard_StartReadList(s32 card, char must_exist);

/* Starts writing save number save_index to a card, unless a task is running. data is the save image; its
   first word is its size. */
s32 SaveCard_StartSave(s32 card, s32 file, s32 *data);

/* Starts reading save number save_index from a card into data, unless a task is running. */
s32 SaveCard_StartLoad(s32 card, s32 file, s32 *data, s32 size);

/* Starts formatting a card, unless a task is running. */
s32 SaveCard_StartFormat(s32 card);

/* Starts looking through both cards again, whatever task was running. */
s32 SaveCard_StartRescan(void);

/* Asks for the card work to end: SaveCard_Update drops the buffers once the check in progress is through. */
void SaveCard_Close(void);

/* Fills the memory card icon header with the save title, icon file names and lighting. */
void SaveCard_BuildIconSys(s32 card);

/* Returns 0 when no task is running and -1 otherwise. */
s32 SaveCard_CheckBusy(void);

/* Returns the list record of save number index. */
SaveInfo *SaveCard_GetSaveInfo(s32 index);

/* Tells whether the card check found a card of any type in the port. */
s32 SaveCard_HasCard(s32 index);

/* Returns 1 when save number index exists, 0 when it does not and -1 when it is damaged. */
s8 SaveCard_GetSaveState(s32 index);

/* Returns 1 when none of the three saves is damaged and -1 otherwise. */
s32 SaveCard_CheckSavesIntact(void);

/* Returns 1 when the card has the 0x96 free clusters the first save needs and -1 otherwise. */
s32 SaveCard_HasRoomForFolder(s32 index);

/* Returns 1 when the card has room for count saves of 0x5A clusters each, plus 0x96 more with extra set, and
   -1 otherwise. */
s32 SaveCard_HasRoomFor(s32 index, s32 count, s32 extra);

/* Returns the free clusters of the card. */
s32 SaveCard_GetFreeClusters(s32 index);

/* Returns one bit per save that exists, or 0xFF when one of the three is damaged. */
s32 SaveCard_GetSaveMask(void);

/* Empties the three list records. */
void SaveCard_ClearSaveInfos(void);

/* Forgets what is known of the card at index. */
void SaveCard_ResetCard(s32 index);

/* Clears the card's load-failed mark. */
void SaveCard_ClearLoadFailed(s32 index);

/* Returns the card's load-failed mark. */
s8 SaveCard_GetLoadFailed(s32 index);

/* Tells whether a check of both cards has finished since SaveCard_Open. */
s32 SaveCard_IsCheckDone(void);

/* Tells whether the task in progress is a card check. */
s32 SaveCard_IsChecking(void);

/* Returns the flag SaveCard_Update sets while it steps the check of both cards. */
s8 SaveCard_IsCheckRunning(void);

/* Tells whether a card answered in the port. */
s8 SaveCard_IsPresent(s32 index);

/* Tells whether the card is formatted. */
s8 SaveCard_IsFormatted(s32 index);

/* Tells whether the card has the game's directory. */
s8 SaveCard_HasSaveFolder(s32 index);

/* Tells whether one of the game's files on the card is missing or has the wrong size. */
s8 SaveCard_IsDamaged(s32 index);

/* Drops the task in progress and marks both cards as not checked; the caller passes the selected card, which
   goes unused. */
void SaveCard_Cancel(s32 index);

/* What stands in the way of saving to a card: 1 it is not checked or failed the check, 2 it is not a PS2
   memory card, 8 it is formatted but has neither the game's directory nor room for it, 0 nothing. */
s32 SaveCard_GetCardProblem(s32 index);

/* Goes through the list: -1 at the first damaged record, 1 at the first save that exists, 0 when the list is empty. */
s32 SaveCard_GetSavesState(void);

/* Tells whether the list names any save. */
s32 SaveCard_HasAnySave(void);

#endif
