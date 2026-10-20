/* The save screens' memory card work: the check of the two cards, and the tasks that format a card, read its
   save list, and write or read one of the three saves. Each task is a state machine that SaveCard_Update steps
   once a frame on top of the card requests of save/mc.h. */
#include "save/save_card.h"
#include "core/file.h"
#include "save/mc.h"
#include <libmc.h>

void *memset(void *, int, u32);
int sprintf(char *, const char *, ...);
void *memcpy(void *, const void *, u32);
int strcmp(const char *, const char *);
void SyncDCache(void *start, void *end);

void *MenuHeap_Alloc(int);
int File_OpenUnflagged(char *, int, int);

extern GameStatus game_status;
extern int save_loaded;
extern s32 save_area_name_id[];
extern s32 player_state[];
extern PlayTime current_play_time;

static sceMcTblGetDir *dir_entries;     /* 0x447334 the eleven directory entries a directory request fills */
static McIconSys *icon_sys;             /* 0x447338 */
static SaveInfo *save_infos;            /* 0x44733C the list file: one record per save */
static s8 list_must_exist;              /* 0x447340 SaveCard_StartReadList's second argument */

static CardTask card_check;             /* 0x5CF3F0 */
static CardTask card_task;              /* 0x5CF410 */
static char icon_file[0xDBF0];          /* 0x5CF430 menu/kf4_icon.ico */
static McIconSys icon_sys_buffer;       /* 0x5DD020 */

static char read_list_path[0x40] = { 0 }; /* 0x428A80 */
static char save_path[0x40] = { 0 };      /* 0x428AC0 */
static char load_path[0x40] = { 0 };      /* 0x428B00 */
static char rescan_path[0x40] = { 0 };    /* 0x428B40 */
static CardRecord card_records[2] = { { 0 } }; /* 0x428B80 */

static void SaveCard_ResetRecord(CardRecord *card);
static s32 SaveCard_StepCheck(void);
static s32 SaveCard_StepReadList(void);
static s32 SaveCard_StepSave(void);
static s32 SaveCard_StepLoad(void);
static s32 SaveCard_StepFormat(void);
static s32 SaveCard_StepRescan(void);
static int SaveCard_StepTask(void);
static s32 SaveCard_Fail(void);
static void SaveCard_SetFailureMessage(void);
static int SaveCard_FindFile(char *name, int size);
static int SaveCard_VerifySaves(void);
static void SaveCard_FillSaveInfo(void *unused_arg0, s32 index);

/* Starts the memory card library and clears both task records. Returns -1 when the library does not start. */
s32 SaveCard_Init(void) {
    if (Mc_Init() != 0) {
        return -1;
    }
    card_task.kind = CARD_TASK_NONE;
    card_task.step = 0;
    card_task.retries = 0;
    card_check = card_task;
    return 0;
}

/* Gets the card work ready for the save screens: takes 0x800 bytes for the save list and the directory entries,
   loads the icon file, forgets what was known of both cards and starts checking them. Returns -1 while a card
   request is still running. */
s32 SaveCard_Open(void) {
    CardRecord *card;
    s32 file_handle;
    s32 base;
    s32 addr;

    if (Mc_Update() > 0) {
        return -1;
    }
    card_check.check_done = 0;
    card_check.step = 0;
    card_check.retries = 0;
    card_check.kind = CARD_TASK_CHECK;
    card_check.keep_open = 1;
    base = (s32)MenuHeap_Alloc(0x800);
    addr = (base & 0x3F) == 0 ? base : base + (base & 0x3F);
    save_infos = (SaveInfo *)addr;
    addr += 0x480;
    if ((addr & 0x3F) != 0) {
        addr += addr & 0x3F;
    }
    dir_entries = (sceMcTblGetDir *)addr;
    icon_sys = &icon_sys_buffer;
    file_handle = File_OpenUnflagged("menu/kf4_icon.ico", 0, 1);
    File_Read(file_handle, icon_file, 0xDBF0);
    File_Close(file_handle);
    card_records[0].icon_data = icon_file;
    card_records[0].icon_data_copy = icon_file;
    card_records[0].icon_data_backup = icon_file;
    card_records[1].icon_data = icon_file;
    card_records[1].icon_data_copy = icon_file;
    card_records[1].icon_data_backup = icon_file;
    memset(dir_entries, 0, 0x2C0);
    memset(icon_sys, 0, 0x3C4);
    memset((void *)save_infos, 0, 0x480);
    card = card_records;
    do {
        SaveCard_ResetRecord(card);
        card++;
    } while ((s32)card < (s32)(card_records + 2));
    SaveCard_ClearSaveInfos();
    return 0;
}

/* Forgets what is known of one card. */
static void SaveCard_ResetRecord(CardRecord *card) {
    card->status.bytes.check_state = 1;
    card->status.bytes.card_type = 0;
    card->free_clusters = 0;
    card->present = 0;
    card->formatted = 0;
    card->has_save_list = 0;
    card->has_save_folder = 0;
    card->no_card = 0;
    card->icon_missing = 0;
    card->load_failed = 0;
    card->damaged = 0;
    card->message = 0;
}

/* Once a frame: steps the check of both cards while one is wanted and no card is known to have failed, then
   the task in progress. Returns 0 while there is work left, 1 when a step finished or nothing runs, and -1 after
   a failure or once SaveCard_Close has taken effect (the buffers are dropped then). */
int SaveCard_Update(void) {
    CardTask *check;
    CardTask *task;
    int result;

    check = &card_check;
    if (check->kind == CARD_TASK_CHECK && card_records[0].status.bytes.check_state != 2 &&
        card_records[1].status.bytes.check_state != 2) {
        result = SaveCard_StepCheck();
        task = &card_task;
        task->check_running = 1;
        if (task->kind == CARD_TASK_CHECK) {
            if (result != 1) {
                return 0;
            }
            task->kind = CARD_TASK_NONE;
            task->step = 0;
            task->retries = 0;
            task->check_running = 0;
            return 1;
        }
        if (task->kind == CARD_TASK_NONE) {
            if (result == 1) {
                if (check->keep_open == 0) {
                    check->check_done = 0;
                    check->kind = CARD_TASK_NONE;
                    icon_sys = NULL;
                    dir_entries = NULL;
                    save_infos = NULL;
                    return -1;
                }
                check->check_done = result;
                task->check_running = 0;
                check->step = 0;
            }
            return 1;
        }
        if (result == 0) {
            return 0;
        }
    }
    return SaveCard_StepTask();
}

/* Asks for both cards to be checked again, unless a task other than a failed one is running; with reset set,
   also forgets what was known of them. */
s32 SaveCard_StartCheck(s8 reset) {
    s32 i;

    if (card_task.kind != CARD_TASK_NONE && card_task.kind != CARD_TASK_FAILED) {
        return -1;
    }
    card_task.kind = card_task.last_kind = CARD_TASK_CHECK;
    for (i = 0; i < 2; i++) {
        if (card_records[i].status.bytes.check_state != 0) {
            card_records[i].status.bytes.check_state = 1;
        }
    }
    if (reset != 0) {
        SaveCard_ResetRecord(&card_records[0]);
        SaveCard_ResetRecord(&card_records[1]);
    }
    return 0;
}

/* Steps the check of both memory cards: five steps per card (probe, card type, free space, open the
   directory, look for the save), then the same for the second card. Returns 1 when finished. */
static s32 SaveCard_StepCheck(void) {
    CardTask *task = &card_check;
    s32 index;
    CardRecord *card;
    s32 status;

    if (task->step >= 10) {
        return 1;
    }
    index = task->step / 5;
    card = &card_records[index];
    switch (task->step % 5) {
    case 0:
        if (Mc_StartCheck(index, 0) != 0) {
            if (++task->retries == 5) {
                card->no_card = 1;
                card->status.bytes.check_state = 2;
                card->message = 1;
                task->retries = 0;
                return 1;
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 1:
        if (Mc_Update() > 0) {
            return 0;
        }
        switch (Mc_GetStatus()) {
        case MC_STATUS_ERROR:
            card->no_card = 1;
            card->status.bytes.check_state = 2;
            card->message = 1;
            if (task->step == 6) {
                return 1;
            }
            task->step = 5;
            break;
        case MC_STATUS_UNFORMATTED:
        case 4:
        case 0x15:
            if (++task->retries == 5) {
                card->present = 1;
                card->formatted = 0;
                card->has_save_folder = 0;
                card->has_save_list = 0;
                card->free_clusters = 0;
                card->message = 3;
                task->step++;
                task->retries = 0;
            }
            break;
        case MC_STATUS_OK:
        case 2:
            status = Mc_GetCheckResult(index, 0);
            if (status <= 0) {
                if (status >= -2) {
                    card->formatted = card->present = 1;
                    task->step++;
                    break;
                }
            }
            card->status.bytes.card_type = 0;
            card->present = 0;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
            card->message = 2;
            if (task->step == 6) {
                return 1;
            }
            task->step = 5;
            return 0;
        case MC_STATUS_NO_CARD:
        default:
            card->status.bytes.check_state = 0;
            card->status.bytes.card_type = 0;
            card->present = 0;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
            card->message = 2;
            if (task->step == 6) {
                task->step = 10;
                return 1;
            }
            task->step = 5;
            break;
        }
        break;
    case 2:
        card->status.bytes.card_type = Mc_GetCardType(index, 0);
        if (card->formatted != 0 && card->status.bytes.card_type == 2) {
            card->free_clusters = Mc_GetFreeClusters(index, 0);
            task->step++;
            task->retries = 0;
            card->status.bytes.check_state = 0;
        } else {
            card->status.bytes.check_state = 0;
            if (task->step == 7) {
                return 1;
            }
            task->step = 5;
        }
        break;
    case 3:
        if (Mc_StartGetDir(index, 0, "/BASLUS-20318*", 0, 0xB, dir_entries) != 0) {
            if (++task->retries == 5) {
                if (task->step == 8) {
                    return 1;
                }
                task->step = 5;
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 4:
        if (Mc_Update() > 0) {
            return 0;
        }
        if (Mc_GetStatus() == MC_STATUS_OK) {
            if (Mc_GetDirCount() > 0) {
                card->has_save_folder = 1;
            } else {
                card->has_save_folder = 0;
            }
            if (++task->step >= 10) {
                return 1;
            }
        } else {
            if (task->step == 9) {
                return 1;
            }
            task->step = 5;
        }
        break;
    }
    return 0;
}

/* Starts reading the save list of a card, unless a task is running. With must_exist set, a card without a
   readable list counts as a failure. */
s32 SaveCard_StartReadList(s32 card, char must_exist) {
    if (card_task.kind != CARD_TASK_NONE) {
        return -1;
    }
    card_records[0].damaged = 0;
    card_records[1].damaged = 0;
    SaveCard_ClearSaveInfos();
    card_task.card_index = card;
    card_task.kind = card_task.last_kind = CARD_TASK_READ_LIST;
    card_task.fail_step = 0;
    card_task.step = 0;
    card_task.fail_retries = 0;
    card_task.retries = 0;
    list_must_exist = must_exist;
    return 0;
}

/* Steps the reading of the save list: look at the card, read the game's directory, check that the icon files
   are there, then read the list file (the three SaveInfo records). */
static s32 SaveCard_StepReadList(void) {
    CardTask *task = &card_task;
    CardRecord *card = &card_records[task->card_index];
    s32 result = 0;

    switch (task->step) {
    case 0:
        if (Mc_Update() > 0) {
            break;
        }
        if (card->present == 0 || card->status.bytes.card_type != 2) {
            result = SaveCard_Fail();
            break;
        }
        card->status.bytes.check_state = 1;
        if (card->formatted == 0) {
            result = 1;
            break;
        }
        task->step++;
    case 1:
        if (Mc_StartGetDir(task->card_index, 0, "/BASLUS-20318*", 0, 4, dir_entries) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 2:
        if (Mc_Update() > 0) {
            break;
        }
        if (Mc_GetStatus() != MC_STATUS_OK) {
            result = SaveCard_Fail();
            break;
        }
        if (Mc_GetDirCount() != 0) {
            task->retries = 0;
            task->step++;
            break;
        }
        if (Mc_GetCheckResult(task->card_index, 0) != 0) {
            result = SaveCard_Fail();
        } else {
            result = 1;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        }
        break;
    case 3:
        memset(dir_entries, 0, 0x2C0);
        if (Mc_StartGetDir(task->card_index, 0, "/BASLUS-20318S/*", 0, 0xB, dir_entries) == 0) {
            task->retries = 0;
            task->step++;
        } else if (++task->retries >= 5) {
            result = SaveCard_Fail();
        }
        break;
    case 4:
        if (Mc_Update() > 0) {
            break;
        }
        if (Mc_GetStatus() != MC_STATUS_OK) {
            result = SaveCard_Fail();
            break;
        }
        if (Mc_GetDirCount() == 0) {
            if (Mc_GetCheckResult(task->card_index, 0) != 0) {
                result = SaveCard_Fail();
            } else {
                card->has_save_list = 0;
                result = 1;
            }
            break;
        }
        sprintf(read_list_path, "%s", "KF4GAME.ICO");
        if (SaveCard_FindFile(read_list_path, 0xDBF0) == 0) {
            card->icon_missing = 0;
        } else {
            card->damaged = card->icon_missing = 1;
        }
        sprintf(read_list_path, "icon.sys");
        if (SaveCard_FindFile(read_list_path, 0) != 0) {
            card->damaged = 1;
            result = SaveCard_Fail();
            break;
        }
        sprintf(read_list_path, "%s%s", "BASLUS-20318", "S");
        if (SaveCard_FindFile(read_list_path, 0) == 0) {
            sprintf(read_list_path, "%s%s%s%s", "/BASLUS-20318", "S", "/BASLUS-20318", "S");
            task->step++;
        } else {
            card->has_save_list = 0;
            card->damaged = 1;
            result = 1;
        }
        break;
    case 5:
        if (Mc_StartReadChecked(task->card_index, 0, read_list_path, save_infos, 0x480) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 6:
        if (Mc_Update() > 0) {
            break;
        }
        if (Mc_GetStatus() == MC_STATUS_READ_DONE) {
            card->has_save_list = 1;
            SaveCard_VerifySaves();
            result = 1;
        } else {
            card->has_save_folder = 0;
            card->has_save_list = 0;
            if (list_must_exist == 0) {
                result = 1;
            } else {
                result = SaveCard_Fail();
            }
        }
        break;
    }
    return result;
}

/* Starts writing save number save_index to a card, unless a task is running. data is the save image; its
   first word is its size. */
s32 SaveCard_StartSave(s32 card, s32 file, s32 *data) {
    CardTask *task = &card_task;
    CardRecord *records = card_records;

    if (task->kind != CARD_TASK_NONE) {
        return -1;
    }
    records[1].save_data = records[0].save_data = data;
    card_task.card_index = card;
    task->save_index = file;
    task->last_kind = CARD_TASK_SAVE;
    task->kind = CARD_TASK_SAVE;
    task->fail_step = 0;
    task->step = 0;
    task->fail_retries = 0;
    task->retries = 0;
    return 0;
}

/* Steps the save: make the game's directory if the card has none, write the icon file if it is missing, write
   the save file, then the list file and icon.sys. */
static s32 SaveCard_StepSave(void) {
    CardTask *task = &card_task;
    CardRecord *card = &card_records[task->card_index];
    s32 result = 0;
    s32 file = task->save_index;
    s32 status;
    char *flag;

    switch (task->step) {
    case 0:
        if (Mc_Update() <= 0) {
            if (Mc_StartCheck(task->card_index, 0) != 0) {
                if (++task->retries == 5) {
                    result = SaveCard_Fail();
                }
            } else {
                task->retries = 0;
                task->step++;
            }
        }
        break;
    case 1:
        if (Mc_Update() <= 0) {
            if (Mc_GetStatus() != MC_STATUS_OK) {
                return SaveCard_Fail();
            }
            if (card->status.word == 0x200 && card->no_card == 0 && card->present != 0 && card->formatted != 0) {
                if (card->has_save_folder == 0) {
                    sprintf(save_path, "%s%s", "/BASLUS-20318", "S");
                    task->step++;
                } else {
                    if (card->has_save_list == 0 || card->icon_missing == 1) {
                        task->step = 4;
                    } else {
                        task->step = 7;
                    }
                }
                card->status.bytes.check_state = 1;
            } else {
                result = SaveCard_Fail();
            }
        }
        break;
    case 2:
        if (Mc_StartMkdir(task->card_index, 0, save_path) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 3:
        status = Mc_Update();
        if (status > 0) {
            break;
        }
        if (Mc_GetStatus() == MC_STATUS_ERROR || status < 0) {
            card->has_save_folder = 0;
            result = SaveCard_Fail();
        } else {
            card->has_save_folder = 1;
            task->step++;
        }
        break;
    case 4:
        sprintf(save_path, "%s%s/%s", "/BASLUS-20318", "S", "KF4GAME.ICO");
        task->step++;
    case 5:
        if (Mc_StartWrite(task->card_index, 0, save_path, card->icon_data, 0xDBF0) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 6:
        status = Mc_Update();
        if (status > 0) {
            break;
        }
        if (Mc_GetStatus() != MC_STATUS_WRITTEN || status < 0) {
            result = SaveCard_Fail();
        } else {
            task->step++;
        }
        break;
    case 7:
        sprintf(save_path, "%s%s%s%s%02d", "/BASLUS-20318", "S", "/BASLUS-20318", "G", file);
        task->step++;
    case 8:
        if (Mc_StartWriteChecked(task->card_index, 0, save_path, card->save_data, *card->save_data) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 9:
        status = Mc_Update();
        if (status > 0) {
            break;
        }
        if (Mc_GetStatus() != MC_STATUS_WRITTEN || status < 0) {
            flag = &save_infos[file].damaged;
            *flag = 1;
            result = SaveCard_Fail();
        } else {
            flag = &save_infos[file].damaged;
            *flag = 0;
            task->step++;
        }
        break;
    case 10:
        SaveCard_FillSaveInfo((void *)task->card_index, file);
        sprintf(save_path, "%s%s%s%s", "/BASLUS-20318", "S", "/BASLUS-20318", "S");
        task->step++;
    case 11:
        if (Mc_StartWriteChecked(task->card_index, 0, save_path, save_infos, 0x480) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 12:
        status = Mc_Update();
        if (status > 0) {
            break;
        }
        if (Mc_GetStatus() != MC_STATUS_WRITTEN || status < 0) {
            result = SaveCard_Fail();
        } else {
            card->has_save_list = 1;
            SaveCard_BuildIconSys(task->card_index);
            sprintf(save_path, "%s%s/icon.sys", "/BASLUS-20318", "S");
            task->step++;
        }
        break;
    case 13:
        if (Mc_StartWrite(task->card_index, 0, save_path, icon_sys, 0x3C4) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 14:
        status = Mc_Update();
        if (status > 0) {
            break;
        }
        if (Mc_GetStatus() != MC_STATUS_WRITTEN || status < 0) {
            return SaveCard_Fail();
        }
        result = 1;
        break;
    }
    return result;
}

/* Starts reading save number save_index from a card into data, unless a task is running. */
s32 SaveCard_StartLoad(s32 card, s32 file, s32 *data, s32 size) {
    CardRecord *records = card_records;
    CardTask *task = &card_task;
    CardRecord *chosen = &card_records[card];

    records[0].save_data = data;
    records[0].save_data_size = size;
    records[1].save_data = data;
    records[1].save_data_size = size;
    if (task->kind != CARD_TASK_NONE) {
        return -1;
    }
    if (chosen->save_data == NULL) {
        return -1;
    }
    task->card_index = card;
    task->last_kind = CARD_TASK_LOAD;
    task->kind = CARD_TASK_LOAD;
    task->fail_step = 0;
    task->step = 0;
    task->fail_retries = 0;
    task->retries = 0;
    task->save_index = file;
    return 0;
}

/* Steps the load: probe the card, then read the 0x20000-byte save file. */
static s32 SaveCard_StepLoad(void) {
    CardTask *task = &card_task;
    CardRecord *card = &card_records[task->card_index];
    s32 result = 0;
    s32 file = task->save_index;
    s32 status;
    s32 code;

    switch (task->step) {
    case 0:
        if (Mc_Update() <= 0) {
            if (Mc_StartCheck(task->card_index, 0) != 0) {
                if (++task->retries == 5) {
                    result = SaveCard_Fail();
                }
            } else {
                task->retries = 0;
                task->step++;
            }
        }
        break;
    case 1:
        if (Mc_Update() <= 0) {
            if (Mc_GetStatus() != MC_STATUS_OK) {
                return SaveCard_Fail();
            }
            task->step++;
        }
        break;
    case 2:
        if (Mc_Update() > 0) {
            break;
        }
        sprintf(load_path, "%s%s%s%s%02d", "/BASLUS-20318", "S", "/BASLUS-20318", "G", file);
        task->step++;
    case 3:
        if (Mc_StartReadChecked(task->card_index, 0, load_path, card->save_data, 0x20000) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 4:
        status = Mc_Update();
        if (status > 0) {
            break;
        }
        code = Mc_GetStatus();
        if (code != MC_STATUS_READ_DONE || status < 0) {
            card->load_failed = 1;
            result = SaveCard_Fail();
        } else {
            save_loaded = 1;
            result = 1;
            game_status.bit9 = 1;
        }
        break;
    }
    return result;
}

/* Starts formatting a card, unless a task is running. */
s32 SaveCard_StartFormat(s32 card) {
    if (card_task.kind != CARD_TASK_NONE) {
        return -1;
    }
    card_task.card_index = card;
    card_task.retries = 0;
    card_task.last_kind = CARD_TASK_FORMAT;
    card_task.fail_step = 0;
    card_task.step = 0;
    card_task.fail_retries = 0;
    card_task.kind = CARD_TASK_FORMAT;
    return 0;
}

/* Steps the format: probe the card, make sure it still answers as unformatted, format it, then wait for the
   result. */
static s32 SaveCard_StepFormat(void) {
    CardTask *task = &card_task;
    CardRecord *card = &card_records[task->card_index];
    s32 result = 0;
    s32 status;

    switch (task->step) {
    case 0:
        if (Mc_Update() <= 0) {
            if (Mc_StartCheck(task->card_index, 0) != 0) {
                if (++task->retries == 5) {
                    result = SaveCard_Fail();
                }
            } else {
                task->retries = 0;
                task->step++;
            }
        }
        break;
    case 1:
        if (Mc_Update() <= 0) {
            status = Mc_GetStatus();
            if (status != MC_STATUS_UNFORMATTED) {
                result = SaveCard_Fail();
            } else if (++task->retries == 5) {
                card->message = status;
                card->present = 1;
                card->has_save_folder = 0;
                card->has_save_list = 0;
                card->free_clusters = 0;
                card->formatted = 0;
                task->retries = 0;
                task->step++;
            }
        }
        break;
    case 2:
        if (Mc_StartFormat(task->card_index, 0) != 0) {
            if (++task->retries == 5) {
                result = SaveCard_Fail();
            }
        } else {
            task->retries = 0;
            task->step++;
        }
        break;
    case 3:
        if (Mc_Update() <= 0) {
            if (Mc_GetStatus() == MC_STATUS_FORMATTED) {
                card->formatted = 1;
                card->message = 0;
                card->status.bytes.check_state = 0;
                result = 1;
            } else {
                result = SaveCard_Fail();
            }
        }
        break;
    }
    return result;
}

/* Starts looking through both cards again, whatever task was running. */
s32 SaveCard_StartRescan(void) {
    card_task.retries = 0;
    card_task.last_kind = CARD_TASK_RESCAN;
    card_task.fail_step = 0;
    card_task.step = 0;
    card_task.fail_retries = 0;
    card_task.kind = CARD_TASK_RESCAN;

    return 0;
}

/* Steps the second look through both cards: step / 7 is the card, step % 7 the stage (probe, card type, free
   space, the game's directory, the save directory, the files in it). */
static s32 SaveCard_StepRescan(void) {
    CardTask *task = &card_task;
    s32 index = task->step / 7;
    CardRecord *card = &card_records[index];
    s32 result = 0;

    switch (task->step % 7) {
    case 0:
        if (Mc_Update() > 0) {
            break;
        }
        if (Mc_StartCheck(index, 0) == 0) {
            task->retries = 0;
            task->step++;
        } else if (++task->retries == 5) {
            card->status.bytes.check_state = 2;
            card->message = 1;
            result = 1;
        }
        break;
    case 1:
        if (Mc_Update() > 0) {
            break;
        }
        switch (Mc_GetStatus()) {
        case MC_STATUS_ERROR:
            card->status.bytes.check_state = 2;
            card->message = 1;
            if (task->step == 8) {
                result = 1;
                break;
            }
            task->step = 7;
            break;
        case MC_STATUS_UNFORMATTED:
        case 4:
            if (++task->retries == 5) {
                card->present = 1;
                card->message = 3;
                task->step++;
                task->retries = 0;
                card->has_save_folder = 0;
                card->has_save_list = 0;
                card->free_clusters = 0;
                card->formatted = 0;
            }
            break;
        case MC_STATUS_OK:
        case 2:
            card->formatted = card->present = 1;
            card->has_save_folder = 0;
            task->step++;
            break;
        case MC_STATUS_NO_CARD:
        case 0x15:
        default:
            card->message = 2;
            card->present = 0;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
            card->free_clusters = 0;
            card->status.bytes.check_state = 0;
            card->status.bytes.card_type = 0;
            if (task->step == 8) {
                result = 1;
                break;
            }
            task->step = 7;
            break;
        }
        break;
    case 2:
        if ((card->status.bytes.card_type = Mc_GetCardType(index, 0)) == 2) {
            if (card->formatted != 0) {
                card->free_clusters = Mc_GetFreeClusters(index, 0);
                if (card->free_clusters < 0x96) {
                    task->step++;
                    break;
                }
            }
            card->status.bytes.check_state = 0;
            result = 1;
        }
        break;
    case 3:
        if (Mc_StartGetDir(index, 0, "/BASLUS-20318*", 0, 0xB, dir_entries) == 0) {
            task->retries = 0;
            task->step++;
        } else if (++task->retries == 5) {
            card->status.bytes.check_state = 2;
            if (task->step >= 0xA) {
                result = 1;
                break;
            }
            task->step = 7;
            break;
        }
        break;
    case 4:
        if (Mc_Update() > 0) {
            break;
        }
        if (Mc_GetStatus() == MC_STATUS_OK) {
            card->status.bytes.check_state = 0;
            if (Mc_GetDirCount() > 0) {
                task->retries = 0;
                task->step++;
                break;
            }
            if (task->step == 0xB) {
                result = 1;
                break;
            }
            task->step = 7;
            break;
        }
        card->status.bytes.check_state = 2;
        if (task->step == 0xB) {
            result = 1;
            break;
        }
        task->step = 7;
        break;
    case 5:
        memset(dir_entries, 0, 0x2C0);
        if (Mc_StartGetDir(index, 0, "/BASLUS-20318S/*", 0, 0xB, dir_entries) != 0) {
            if (++task->retries < 5) {
                break;
            }
            card->status.bytes.check_state = 2;
            if (task->step >= 0xC) {
                result = 1;
                break;
            }
            task->step = 7;
            break;
        }
        task->retries = 0;
        task->step++;
        break;
    case 6:
        if (Mc_Update() > 0) {
            break;
        }
        if (Mc_GetStatus() == MC_STATUS_OK) {
            if (Mc_GetDirCount() == 0) {
                if (Mc_GetCheckResult(index, 0) == 0) {
                    card->has_save_list = 0;
                    // match: retail repeats the check tail here instead of falling into check
                    if (task->step == 0xD) {
                        result = 1;
                        break;
                    }
                    task->step = 7;
                    break;
                }
                if (task->step == 0xD) {
                    result = 1;
                    break;
                }
                task->step = 7;
                break;
            }
            sprintf(rescan_path, "%s", "KF4GAME.ICO");
            if (SaveCard_FindFile(rescan_path, 0xDBF0) != 0) {
                if (task->step == 0xD) {
                    result = 1;
                    break;
                }
                task->step = 7;
                break;
            }
            sprintf(rescan_path, "icon.sys");
            if (SaveCard_FindFile(rescan_path, 0) != 0) {
                card->has_save_folder = 0;
                if (task->step == 0xD) {
                    result = 1;
                    break;
                }
                task->step = 7;
                break;
            }
            sprintf(rescan_path, "%s%s", "BASLUS-20318", "S");
            if (SaveCard_FindFile(rescan_path, 0) == 0) {
                sprintf(rescan_path, "%s%s%s%s", "/BASLUS-20318", "S", "/BASLUS-20318", "S");
                card->has_save_folder = 1;
                result = 1;
                task->step++;
                break;
            }
            card->has_save_folder = 0;
            if (task->step != 0xD) {
                task->step = 7;
            }
            result = 1;
            break;
        }
        result = SaveCard_Fail();
        break;
    }
    return result;
}

/* Runs the step function of the task in progress; a result of 1 makes the task idle. */
static int SaveCard_StepTask(void) {
    int result;

    switch (card_task.kind) {
    case CARD_TASK_SAVE:
        result = SaveCard_StepSave();
        break;
    case CARD_TASK_LOAD:
        result = SaveCard_StepLoad();
        break;
    case CARD_TASK_FORMAT:
        result = SaveCard_StepFormat();
        break;
    case CARD_TASK_READ_LIST:
        result = SaveCard_StepReadList();
        break;
    case CARD_TASK_RESCAN:
        result = SaveCard_StepRescan();
        break;
    case CARD_TASK_FAILED:
        result = SaveCard_Fail();
        break;
    case CARD_TASK_NONE:
    default:
        result = 1;
        break;
    }
    if (result == 1) {
        card_task.kind = CARD_TASK_NONE;
        card_task.step = 0;
        card_task.retries = 0;
    }
    return result;
}

/* Asks for the card work to end: SaveCard_Update drops the buffers once the check in progress is through. */
void SaveCard_Close(void) {
    card_check.keep_open = 0;
}

/* What every step function calls when its card request failed: marks the card as failed, probes it once more
   and has SaveCard_SetFailureMessage pick the message. Returns -1 once that is done, 0 while it waits. */
static s32 SaveCard_Fail(void) {
    CardTask *task = &card_task;
    CardRecord *card;
    s32 result;

    card = &card_records[task->card_index];
    task->kind = CARD_TASK_FAILED;
    result = 0;
    card->status.bytes.check_state = 2;
    switch (task->fail_step) {
    case 0:
        if (Mc_Update() <= 0) {
            Mc_StartCheck(task->card_index, 0);
            if (++task->fail_retries == 5) {
                card->message = 1;
                card->no_card = 1;
                result = -1;
            } else {
                task->fail_retries = 0;
                task->fail_step++;
            }
        }
        break;
    case 1:
        if (Mc_Update() <= 0) {
            result = -1;
            Mc_GetStatus();
            SaveCard_SetFailureMessage();
            task->fail_step++;
        }
        break;
    case 2:
        result = -1;
        break;
    default:
        result = -1;
        break;
    }
    return result;
}

/* Sets the card's message and clears what is known about it after a card task failed. */
static void SaveCard_SetFailureMessage(void) {
    CardTask *task = &card_task;
    CardRecord *card = &card_records[task->card_index];
    s32 answer = Mc_GetCardStatus(task->card_index, 0);

    switch ((s16)(task->last_kind - 2)) {
    case 0:
        if (answer == 0) {
            card->message = 6;
        } else {
            card->message = 6;
            if (answer == 2) {
                card->formatted = 0;
            } else if (answer < 0) {
                card->status.bytes.card_type = 0;
                card->present = 0;
                card->formatted = 0;
                card->has_save_folder = 0;
                card->has_save_list = 0;
            }
        }
        break;
    case 1:
        if (answer == 0) {
            card->message = 10;
        } else if (answer == 1) {
            card->message = 4;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        } else if (answer == 2) {
            card->message = 4;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        } else {
            card->message = 5;
            card->status.bytes.card_type = 0;
            card->present = 0;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        }
        break;
    case 2:
        if (answer < 0) {
            card->message = 9;
            card->status.bytes.card_type = 0;
            card->present = 0;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        } else if (answer == 1) {
            card->message = 9;
            card->has_save_list = 0;
        } else if (answer == 2) {
            card->message = 9;
            card->formatted = 0;
            card->has_save_list = 0;
        } else {
            card->message = 9;
        }
        break;
    case 3:
        if (answer < 0) {
            card->message = 7;
            card->status.bytes.card_type = 0;
            card->present = 0;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        } else if (answer == 1) {
            card->message = 7;
            card->has_save_list = 0;
        } else if (answer == 2) {
            card->message = 7;
            card->formatted = 0;
            card->has_save_folder = 0;
            card->has_save_list = 0;
        } else {
            card->message = 7;
        }
        break;
    case 4:
    default:
        card->message = 1;
        break;
    }
}

/* Fills the memory card icon header with the save title, icon file names and lighting. */
void SaveCard_BuildIconSys(s32 card) {
    s32 bg_color[4][4] = {
        { 50, 158, 133, 0 },
        { 6, 118, 131, 0 },
        { 6, 93, 130, 0 },
        { 1, 49, 108, 0 },
    };
    f32 light_dir[3][4] = {
        { 0.5f, 0.5f, 0.5f, 0.0f },
        { 0.0f, -0.4f, -0.1f, 0.0f },
        { -0.5f, -0.5f, 0.5f, 0.0f },
    };
    f32 light_color[3][4] = {
        { 0.7f, 0.65f, 0.5f, 0.0f },
        { 0.4f, 0.5f, 0.5f, 0.0f },
        { 0.2f, 0.3f, 0.35f, 0.0f },
    };
    f32 ambient[4] = { 0.15f, 0.15f, 0.15f, 0.0f };

    memset(icon_sys, 0, 0x3C4);
    memcpy(icon_sys->head, "PS2D", 5);
    // the title in Shift-JIS full-width letters: KING'S FIELD The Ancient City
    memcpy(icon_sys->title, "\202\152\202\150\202\155\202\146\201\146\202\162\201\100\202\145\202\150\202\144\202\153\202\143\201\100\202\163\202\210\202\205\201\100\202\140\202\216\202\203\202\211\202\205\202\216\202\224\201\100\202\142\202\211\202\224\202\231", 59);
    memcpy(icon_sys->view_name, "KF4GAME.ICO", 12);
    memcpy(icon_sys->copy_name, "KF4GAME.ICO", 12);
    memcpy(icon_sys->delete_name, "KF4GAME.ICO", 12);
    icon_sys->transparency = 0x40;
    memcpy(icon_sys->bg_color, bg_color, sizeof(bg_color));
    memcpy(icon_sys->light_dir, light_dir, sizeof(light_dir));
    memcpy(icon_sys->light_color, light_color, sizeof(light_color));
    memcpy(icon_sys->ambient, ambient, sizeof(ambient));
    icon_sys->line_break = 0x20;
}

/* Returns 0 when no task is running and -1 otherwise. */
s32 SaveCard_CheckBusy(void) {
    return (card_task.kind == CARD_TASK_NONE) ? 0 : -1;
}

/* Looks for a name among the eleven directory entries read last. Returns 0 when it is there (and, with a size
   given, has that size), -1 otherwise. */
static int SaveCard_FindFile(char *name, int size) {
    sceMcTblGetDir *entry;
    int i;
    int found;
    int result;

    if (dir_entries == NULL || name == NULL) {
        return -1;
    }
    entry = dir_entries;
    result = 0;
    found = 0;
    for (i = 0; i < 11; i++) {
        if (strcmp((char *)entry->EntryName, name) == 0) {
            found = 1;
            if (size > 0) {
                result = (entry->FileSizeByte == size) ? 0 : -1;
            }
            break;
        }
        entry++;
    }
    return found ? result : -1;
}

/* Marks as damaged every save the list names whose file is not in the directory. Returns -1 when it found one. */
static int SaveCard_VerifySaves(void) {
    char name[0x40];
    int i;
    int offset; // match: the byte offset of the save's record is kept, not a pointer to it
    int result;
    char exists;

    if (dir_entries == NULL) {
        return -1;
    }
    if (save_infos == NULL) {
        return -1;
    }
    result = 0;
    i = 0;
    do {
        offset = i * sizeof(SaveInfo);
        exists = ((SaveInfo *)((char *)save_infos + offset))->exists;
        if (exists == 1) {
            sprintf(name, "%s%s%02d", "BASLUS-20318", "G", i);
            if (SaveCard_FindFile(name, 0) == -1) {
                result = -1;
                ((SaveInfo *)((char *)save_infos + offset))->damaged = exists;
            }
        }
        i++;
    } while (i < 3);
    return result;
}

/* Writes what the save list shows of the game being saved into the list record of save number index:
   save_area_name_id[0], then the player's experience, level, hit and magic points with their maximums, two more
   words and gold, and the play time. */
static void SaveCard_FillSaveInfo(void *unused_arg0, s32 index) {
    SaveInfo *info;

    if (save_infos == NULL) {
        return;
    }
    info = save_infos + index;
    info->exists = 1;
    info->damaged = 0;
    info->numbers[0] = save_area_name_id[0];
    info->numbers[1] = player_state[20];
    info->numbers[2] = player_state[21];
    info->numbers[3] = player_state[45];
    info->numbers[4] = player_state[22];
    info->numbers[5] = player_state[46];
    info->numbers[6] = player_state[23];
    info->numbers[7] = player_state[10];
    info->numbers[8] = player_state[11];
    info->numbers[9] = player_state[19];
    info->play_time = current_play_time;
    SyncDCache(save_infos, save_infos + 3);
}

/* Returns the list record of save number index. */
SaveInfo *SaveCard_GetSaveInfo(s32 index) {
    return &save_infos[index];
}

/* Tells whether the card check found a card of any type in the port. */
s32 SaveCard_HasCard(s32 index) {
    return card_records[index].status.bytes.card_type != 0;
}

/* Returns 1 when save number index exists, 0 when it does not and -1 when it is damaged. */
s8 SaveCard_GetSaveState(s32 index) {
    SaveInfo *info = &save_infos[index];

    if (info->damaged == 0) {
        return info->exists;
    }
    return -1;
}

/* Returns 1 when none of the three saves is damaged and -1 otherwise. */
s32 SaveCard_CheckSavesIntact(void) {
    char *damaged;
    s32 i;

    damaged = &save_infos->damaged;
    for (i = 0; i < 3; i++) {
        if (*damaged != 0) {
            return -1;
        }
        damaged += sizeof(SaveInfo);
    }
    return 1;
}

/* Returns 1 when the card has the 0x96 free clusters the first save needs and -1 otherwise. */
s32 SaveCard_HasRoomForFolder(s32 index) {
    return (card_records[index].free_clusters >= 0x96) ? 1 : -1;
}

/* Returns 1 when the card has room for count saves of 0x5A clusters each, plus 0x96 more with extra set, and
   -1 otherwise. */
s32 SaveCard_HasRoomFor(s32 index, s32 count, s32 extra) {
    s32 limit = count * 0x5A;

    if (extra != 0) {
        limit += 0x96;
    }
    return (card_records[index].free_clusters >= limit) ? 1 : -1;
}

/* Returns the free clusters of the card. */
s32 SaveCard_GetFreeClusters(s32 index) {
    return card_records[index].free_clusters;
}

/* Returns one bit per save that exists, or 0xFF when one of the three is damaged. */
s32 SaveCard_GetSaveMask(void) {
    SaveInfo *info;
    s32 flags;
    s32 i;

    info = save_infos;
    flags = 0;
    for (i = 0; i < 3; i++) {
        if (info->damaged != 0) {
            return 0xFF;
        }
        if (info->exists != 0) {
            flags = (flags | (1 << i)) & 0xFF;
        }
        info++;
    }
    return flags;
}

/* Empties the three list records. */
void SaveCard_ClearSaveInfos(void) {
    SaveInfo *info;
    int i;

    for (i = 0; i < 3; i++) {
        info = &save_infos[i];
        info->numbers[9] = info->numbers[8] = info->numbers[7] = info->numbers[6] = info->numbers[5] = info->numbers[4] = info->numbers[3] = info->numbers[2] = info->numbers[1] = info->numbers[0] = info->damaged = info->exists = 0;
        memset(&info->play_time, 0, sizeof(info->play_time));
    }
}

/* Forgets what is known of the card at index. */
void SaveCard_ResetCard(s32 index) {
    SaveCard_ResetRecord(&card_records[index]);
}

/* Clears the card's load-failed mark. */
void SaveCard_ClearLoadFailed(s32 index) {
    card_records[index].load_failed = 0;
}

/* Returns the card's load-failed mark. */
s8 SaveCard_GetLoadFailed(s32 index) {
    return card_records[index].load_failed;
}

/* Tells whether a check of both cards has finished since SaveCard_Open. */
s32 SaveCard_IsCheckDone(void) {
    return card_check.check_done != 0;
}

/* Tells whether the task in progress is a card check. */
s32 SaveCard_IsChecking(void) {
    return card_task.kind == CARD_TASK_CHECK;
}

/* Returns the flag SaveCard_Update sets while it steps the check of both cards. */
s8 SaveCard_IsCheckRunning(void) {
    return card_task.check_running;
}

/* Tells whether a card answered in the port. */
s8 SaveCard_IsPresent(s32 index) {
    return card_records[index].present;
}

/* Tells whether the card is formatted. */
s8 SaveCard_IsFormatted(s32 index) {
    return card_records[index].formatted;
}

/* Tells whether the card has the game's directory. */
s8 SaveCard_HasSaveFolder(s32 index) {
    return card_records[index].has_save_folder;
}

/* Tells whether one of the game's files on the card is missing or has the wrong size. */
s8 SaveCard_IsDamaged(s32 index) {
    return card_records[index].damaged;
}

/* Drops the task in progress and marks both cards as not checked; the caller passes the selected card, which
   goes unused. */
void SaveCard_Cancel(s32 index) {
    card_records[0].status.bytes.check_state = 1;
    card_task.kind = CARD_TASK_NONE;
    card_records[1].status.bytes.check_state = 1;
}

/* What stands in the way of saving to a card: 1 it is not checked or failed the check, 2 it is not a PS2
   memory card, 8 it is formatted but has neither the game's directory nor room for it, 0 nothing. */
s32 SaveCard_GetCardProblem(s32 index) {
    CardRecord *card = &card_records[index];

    if (card->status.bytes.check_state != 0) {
        return 1;
    } else {
        if (card->status.bytes.card_type != 2) {
            return 2;
        }
        if (card->formatted == 0 || card->free_clusters >= 0x96 || card->has_save_folder != 0) {
            return 0;
        }
        return 8;
    }
}

/* Goes through the list: -1 at the first damaged record, 1 at the first save that exists, 0 when the list is empty. */
s32 SaveCard_GetSavesState(void) {
    SaveInfo *info = save_infos;
    SaveInfo *end = info + 3;

    do {
        if (info->damaged == 0) {
            if (info->exists != 0) {
                return 1;
            }
        } else {
            return -1;
        }
        info++;
    } while ((s32)info < (s32)end);
    return 0;
}

/* Tells whether the list names any save. */
s32 SaveCard_HasAnySave(void) {
    SaveInfo *info;
    SaveInfo *end;

    info = save_infos;
    end = info + 3;
    do {
        if (info->exists != 0) {
            return 1;
        }
        info++;
    } while ((s32)info < (s32)end);
    return 0;
}
