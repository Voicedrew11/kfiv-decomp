/* The map script interpreter and script commands 0 to 99.

   A map's script is a run of words, opcode << 16 | operand. Up to 24 script threads run it at once, each
   from its own label (word 0x190); words 0x190 to 0x19D steer the thread (labels, end, "if", "set", jumps,
   blocks that do not wait) and every other word starts a command with that number, whose arguments
   follow it. A thread waits until all its commands have finished before it reads on. Commands are
   numbered in hundreds: 0 to 99 here, 100 to 199 in kf_script_world.c and 200 to 299 in
   kf_script_entity.c, each hundred with a handler table of its own (D_004331B0, D_00433440 and
   D_00433650). Scripts keep values in game variables (blocks of game state listed in D_00433340), in
   0x400 bytes all threads share (shared_vars) and in each thread's own 0x200 bytes. */
#include "kf_script.h"
#include "kf_entity.h"
#include "kf_entity_world.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_sound.h"
#include "kf_splash.h"

void *memset(void *, int, u32);
void Sound_SetBgmVolume(int level);
void Sound_SetBankVolumes(int level);
void func_001BB5C0(void);
int func_00103548(int map, int entry);
void Game_MoveRecollectionPage(u32 source_page, u32 dest_page);
void Game_MarkMessageHeard(int flag);

extern char D_00432920[];
extern u8 *D_00433340[];
extern ScriptCommandHandler D_004331B0[];
extern ScriptCommandHandler D_00433440[];
extern ScriptCommandHandler D_00433650[];
extern f32 D_004325E8[];
extern u8 recollection_pages[50];
extern u8 D_00349680[];
extern SoundLoadQueue D_0038A130;
extern Entity D_003BEDF0[][0x80];
extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;

static ScriptThread *current_thread = NULL; /* 0x446FD0 the thread being stepped */
static int command_count;                   /* 0x4473E4 commands in use, as Script_StepHeldCommands counts them */
static u32 *script_top;                     /* 0x4473E8 the current map's script */
static u8 shared_vars[0x400];               /* 0x1DFAFB0 the variables all threads share */
static ScriptThread threads[24];            /* 0x1DFB3B0 */

static int Script_StartAlways(int script_id, int argument);
static int Script_IsThreadBusy(int index);
static int Script_FindFreeCommand(ScriptThread *thread);
static u32 *Script_SkipWord(u32 *cursor);
static int Script_UpdateAllHeld(void);
static int Script_StepCommands(ScriptThread *thread);
static int Script_StepHeldCommands(ScriptThread *thread);
static int Script_Advance(ScriptThread *thread);
static int Script_EvalIf(ScriptThread *thread, u32 *cursor);
static u32 *Script_EvalSet(ScriptThread *thread, u32 *cursor);
static void Script_ReadArg(ScriptThread *thread, u32 *source, ScriptArg *result);
static void Script_ReadGameVar(ScriptArg *result, u32 code, int offset);
static void Script_ReadSharedVar(ScriptArg *result, u32 code, int offset);
static void Script_ReadLocalVar(ScriptArg *result, ScriptThread *thread, u32 code, int offset);
static void Script_WriteGameVar(ScriptArg *source, u32 code, int offset);
static void Script_WriteSharedVar(ScriptArg *source, u32 code, int offset);
static void Script_WriteCurrentLocalVar(ScriptArg *source, u32 code, int offset);
static void Script_WriteLocalVar(ScriptArg *source, ScriptThread *thread, u32 code, int offset);
static u32 *Script_StartCommand(ScriptThread *thread, u32 *cursor);
static int Script_HaltCursor(void);
static int Script_UnusedStub(void);

/* Fills count words at dest with value. */
void Script_FillWords(void *dest, int count, int value) {
    int *word = dest;

    while (count-- != 0) {
        *word++ = value;
    }
}

/* Gets the interpreter ready for a map's script (MapSlot.unk5C, its address as a word): every thread is
   freed and set at the top of it, and the shared variables are cleared. */
void Script_Init(s32 script_data) {
    ScriptThread *thread;
    u8 *first_command;
    u8 *end;

    script_top = (u32 *)script_data;
    command_count = 0;
    memset(shared_vars, 0, 0x400);
    thread = threads;
    first_command = (u8 *)thread->commands;
    end = (u8 *)thread + 24 * sizeof(ScriptThread);
    do {
        thread->cursor = script_top;
        Script_FillWords(first_command, sizeof(ScriptThreadCommand) / 4, 0);
        thread->script_id = 0;
        thread++;
        first_command += sizeof(ScriptThread);
    } while ((s32)thread < (s32)end);
}

/* Starts the script `script_id` in a free thread unless it is running already: looks its label (word
   0x190) up from the top of the script. Returns the thread's index, 0 when nothing was started. */
int Script_Start(int script_id, int argument) {
    ScriptThread *thread;
    u32 slot_index;
    u32 word;
    int operand;
    u32 command_index;
    char unused[0x50]; // match: retail reserves 0x50 bytes it never uses

    if (Script_IsRunning(script_id)) {
        return 0;
    }
    thread = NULL;
    for (slot_index = 0; slot_index < 24; slot_index++) {
        if (threads[slot_index].script_id == 0) {
            thread = &threads[slot_index];
            break;
        }
    }
    if (thread == NULL) {
        D_00432920[SCRIPT_ERROR_NO_FREE_THREAD] = 1;
        return 0;
    }
    *(void **)&thread->argument = (void *)argument; // match: stored as a pointer, so the word read below does not wait for it
    thread->script_id = script_id;
    thread->cursor = script_top;
    while (TRUE) {
        word = *thread->cursor;
        operand = word & 0xFFFF;
        word >>= 16;
        if (word == SCRIPT_WORD_LABEL) {
            if (operand == script_id) {
                break;
            }
            if (operand == SCRIPT_LABEL_END) {
                return 0; // match: the end label; the thread stays claimed and 0 is also thread 0's index
            }
        }
        thread->cursor = Script_SkipWord(thread->cursor);
    }
    for (command_index = 0; command_index < 16; command_index++) {
        thread->commands[command_index].active = 0;
    }
    return slot_index;
}

/* Starts the script `script_id` in a free thread, even if it is running already: looks its label (word
   0x190) up from the top of the script. Returns the thread's index, 0 when no thread is free. */
static int Script_StartAlways(int script_id, int argument) {
    ScriptThread *thread;
    u32 slot_index;
    u32 word;
    int operand;
    u32 command_index;
    char unused[0x50]; // match: retail reserves 0x50 bytes it never uses

    thread = NULL;
    for (slot_index = 0; slot_index < 24; slot_index++) {
        if (threads[slot_index].script_id == 0) {
            thread = &threads[slot_index];
            break;
        }
    }
    if (thread == NULL) {
        D_00432920[SCRIPT_ERROR_NO_FREE_THREAD] = 1;
        return 0;
    }
    *(void **)&thread->argument = (void *)argument; // match: stored as a pointer, so the word read below does not wait for it
    thread->script_id = script_id;
    thread->cursor = script_top;
    while (TRUE) { // match: no end-label test here; an unknown id reads past the script
        word = *thread->cursor;
        operand = word & 0xFFFF;
        word >>= 16;
        if (word == SCRIPT_WORD_LABEL && operand == script_id) {
            break;
        }
        thread->cursor = Script_SkipWord(thread->cursor);
    }
    for (command_index = 0; command_index < 16; command_index++) {
        thread->commands[command_index].active = 0;
    }
    return slot_index;
}

/* TRUE while the thread `index` is in use. */
static int Script_IsThreadBusy(int index) {
    return threads[index].script_id != 0;
}

/* TRUE if some thread is running the script `script_id`. */
int Script_IsRunning(int script_id) {
    ScriptThread *thread;
    ScriptThread *end;

    thread = threads;
    end = (ScriptThread *)((u8 *)thread + 24 * sizeof(ScriptThread));
    do {
        if (thread->script_id == script_id) {
            return TRUE;
        }
        thread++;
    } while ((s32)thread < (s32)end);
    return FALSE;
}

/* Index of the thread's first free command; sets an error flag and returns 0 when all 16 are busy. */
static int Script_FindFreeCommand(ScriptThread *thread) {
    int i;

    for (i = 0; i < 16; i++) {
        if (thread->commands[i].active == 0) {
            return i;
        }
    }
    if (i == 16) {
        D_00432920[SCRIPT_ERROR_NO_FREE_COMMAND] = 1;
        return 0;
    }
    // match: no return here (never reached: the loop only ends with i == 16)
}

/* Steps over one word of a script and its operands; returns where the next word is. */
static u32 *Script_SkipWord(u32 *cursor) {
    u32 word;
    int operand;

    word = *cursor;
    operand = word & 0xFFFF;
    cursor++;
    switch (word >> 16) {
    case SCRIPT_WORD_LABEL:
    case SCRIPT_WORD_END:
    case SCRIPT_WORD_BEGIN_NO_WAIT:
    case SCRIPT_WORD_END_NO_WAIT:
    case SCRIPT_WORD_JUMP:
        break;
    case SCRIPT_WORD_IF:
        cursor += (operand >> 8) * 5 + 1;
        break;
    case SCRIPT_WORD_SET:
        cursor += operand;
        break;
    case SCRIPT_WORD_HALT:
        cursor += operand + 1;
        break;
    default:
        cursor += operand * 2;
        break;
    }
    return cursor;
}

/* Runs the script threads for a frame: steps the commands of each live one and, when none is left, reads
   on in its script. Returns command_count, which it clears and only Script_StepHeldCommands counts up:
   always 0. */
int Script_UpdateAll(void) {
    ScriptThread *thread;
    int i;
    char unused[0x30]; // match: retail reserves 0x30 bytes it never uses

    command_count = 0;
    for (i = 0; i < 24; i++) {
        thread = &threads[i];
        if (thread->script_id != 0) {
            current_thread = thread;
            while (!Script_StepCommands(thread)) {
                if (!Script_Advance(thread)) {
                    break;
                }
            }
        }
    }
    return command_count;
}

/* Script_UpdateAll with Script_StepHeldCommands in place of Script_StepCommands: steps only the commands whose number has bit
   0x8000 set. Nothing calls it. Returns the number of commands in use in the last thread stepped. */
static int Script_UpdateAllHeld(void) {
    ScriptThread *thread;
    int i;
    char unused[0x30]; // match: retail reserves 0x30 bytes it never uses

    command_count = 0;
    for (i = 0; i < 24; i++) {
        thread = &threads[i];
        if (thread->script_id != 0) {
            current_thread = thread;
            while (!Script_StepHeldCommands(thread)) {
                if (!Script_Advance(thread)) {
                    break;
                }
            }
        }
    }
    return command_count;
}

/* Steps each command of the thread once through the handler table of its hundred, except those whose
   number has bit 0x8000 set. Returns the number of commands in use. */
static int Script_StepCommands(ScriptThread *thread) {
    int i;
    int count;
    int number;

    count = 0;
    for (i = 0; i < 16; i++) {
        number = thread->commands[i].active;
        if (number != 0) {
            if (!(number & SCRIPT_COMMAND_HELD)) {
                if (number >= 200) {
                    D_00433650[number - 200](&thread->commands[i]);
                } else if (number >= 100) {
                    D_00433440[number - 100](&thread->commands[i]);
                } else {
                    D_004331B0[number](&thread->commands[i]);
                }
            }
            count++;
        }
    }
    return count;
}

/* Steps the thread's commands whose number has bit 0x8000 set and counts every command in use into
   command_count, which it returns. */
static int Script_StepHeldCommands(ScriptThread *thread) {
    int i;
    int number;
    int index;

    command_count = 0;
    for (i = 0; i < 16; i++) {
        number = thread->commands[i].active;
        if (number != 0) {
            if (number & SCRIPT_COMMAND_HELD) {
                // match: the mask keeps bit 0x8000 and the tables are indexed with the unmasked number
                index = number & 0x7FFFF;
                if (index >= 200) {
                    D_00433650[number - 200](&thread->commands[i]);
                } else if (index >= 100) {
                    D_00433440[number - 100](&thread->commands[i]);
                } else {
                    D_004331B0[number](&thread->commands[i]);
                }
            }
            command_count++;
        }
    }
    return command_count;
}

/* Reads on in the thread's script until a word makes it wait: control words (0x190 to 0x19D) are
   handled here, anything else starts a command. TRUE if a command was started. */
static int Script_Advance(ScriptThread *thread) {
    int started;
    int no_wait;
    int running;
    u32 *cursor;
    u32 word;
    int operand;
    int skip;

    started = FALSE;
    no_wait = FALSE;
    running = TRUE;
    cursor = thread->cursor;
    do {
        word = *cursor;
        operand = word & 0xFFFF;
        switch (word >> 16) {
        case SCRIPT_WORD_END:
            running = FALSE;
            thread->script_id = 0;
            break;
        case SCRIPT_WORD_BEGIN_NO_WAIT:
            no_wait = TRUE;
        case SCRIPT_WORD_LABEL:
        next_word:
            cursor++;
            break;
        case SCRIPT_WORD_END_NO_WAIT:
            running = FALSE;
            no_wait = FALSE;
            goto next_word; // match: retail jumps back to the shared step; a second cursor++ here merges the other way
            break;
        case SCRIPT_WORD_IF:
            skip = Script_EvalIf(thread, cursor);
            cursor = thread->cursor;
            if (skip != 0) {
                cursor += skip;
            }
            break;
        case SCRIPT_WORD_JUMP:
            cursor++;
            cursor += (s16)operand;
            break;
        case SCRIPT_WORD_SET:
            cursor = Script_EvalSet(thread, cursor);
            break;
        case SCRIPT_WORD_HALT:
            // match: Script_HaltCursor is a stub that takes nothing and returns 0, so the cursor becomes NULL
            cursor = (u32 *)((int (*)(u32 *))Script_HaltCursor)(cursor);
            running = no_wait;
            started = TRUE;
            thread->commands[Script_FindFreeCommand(thread)].active = 4;
            break;
        default:
            cursor = Script_StartCommand(thread, cursor);
            running = no_wait;
            started = TRUE;
            break;
        }
    } while (running);
    thread->cursor = cursor;
    return started;
}

/* The script's "if": works out up to 32 comparisons (float ones when either side has type 6), joins them
   with "or" (kind 0) or "and", and returns the number of words to skip when the answer is false, else 0. */
static int Script_EvalIf(ScriptThread *thread, u32 *cursor) {
    int results[32];
    ScriptValue left;
    ScriptValue right;
    int i;
    u32 comparison;
    int count;
    int join_kind;
    int skip;
    int result;
    f32 left_float;
    f32 right_float;

    count = ((u8 *)cursor)[1];
    join_kind = ((u8 *)cursor)[0];
    cursor++;
    skip = *cursor;
    cursor++;
    for (i = 0; i < count; i++) {
        Script_ReadArg(thread, cursor, &left.as_int);
        cursor += 2;
        comparison = *cursor;
        cursor++;
        Script_ReadArg(thread, cursor, &right.as_int);
        cursor += 2;
        if (left.as_int.opcode == SCRIPT_TYPE_FLOAT || right.as_int.opcode == SCRIPT_TYPE_FLOAT) {
            left_float = left.as_float.value;
            right_float = right.as_float.value;
            switch (comparison) {
            case 0:
                results[i] = left_float == right_float;
                break;
            case 1:
                results[i] = left_float != right_float;
                break;
            case 2:
                results[i] = left_float > right_float;
                break;
            case 3:
                results[i] = left_float < right_float;
                break;
            case 4:
                results[i] = left_float >= right_float;
                break;
            case 5:
                results[i] = left_float <= right_float;
                break;
            }
        } else {
            switch (comparison) {
            case 0:
                results[i] = left.as_int.value == right.as_int.value;
                break;
            case 1:
                results[i] = left.as_int.value != right.as_int.value;
                break;
            case 2:
                results[i] = left.as_int.value > right.as_int.value;
                break;
            case 3:
                results[i] = left.as_int.value < right.as_int.value;
                break;
            case 4:
                results[i] = left.as_int.value >= right.as_int.value;
                break;
            case 5:
                results[i] = left.as_int.value <= right.as_int.value;
                break;
            }
        }
    }
    result = results[0];
    if (count != 1) {
        if (join_kind == 0) {
            for (i = 1; i < count; i++) {
                result = result || results[i];
            }
        } else {
            for (i = 1; i < count; i++) {
                result = result && results[i];
            }
        }
    }
    thread->cursor = cursor;
    return result ? 0 : skip;
}

/* The script's "set": reads the variable to change, then operands and operators in turn up to the closing
   word, working in float when the variable's type is 6, and writes the result back to the variable.
   Returns where the script goes on. */
static u32 *Script_EvalSet(ScriptThread *thread, u32 *cursor) {
    ScriptValue target;
    ScriptValue result;
    ScriptValue operand;
    int running;
    int total;
    f32 float_total;
    int operator;
    int is_float;
    int want_operator;
    u32 word;
    u32 code;
    int offset;
    FloatBits bits;
    int value;

    running = TRUE;
    want_operator = FALSE;
    operator = 0xFF;
    cursor++;
    target.as_int.opcode = cursor[0];
    target.as_int.value = cursor[1];
    Script_ReadArg(thread, cursor, &result.as_int);
    cursor += 2;
    is_float = result.as_int.opcode == SCRIPT_TYPE_FLOAT;
    do {
        if (!want_operator) {
            Script_ReadArg(thread, cursor, &operand.as_int);
            cursor += 2;
            if (!is_float) {
                switch (operator) {
                case 0xFF:
                    total = operand.as_int.value;
                    break;
                case 0:
                    total += operand.as_int.value;
                    break;
                case 1:
                    total -= operand.as_int.value;
                    break;
                case 2:
                    total *= operand.as_int.value;
                    break;
                case 3:
                    total /= operand.as_int.value;
                    break;
                case 4:
                    total %= operand.as_int.value;
                    break;
                case 5:
                    total <<= operand.as_int.value;
                    break;
                case 6:
                    total >>= operand.as_int.value;
                    break;
                case 7:
                    total &= operand.as_int.value;
                    break;
                case 8:
                    total |= operand.as_int.value;
                    break;
                }
            } else {
                switch (operator) {
                case 0xFF:
                    float_total = operand.as_float.value;
                    break;
                case 0:
                    float_total += operand.as_float.value;
                    break;
                case 1:
                    float_total -= operand.as_float.value;
                    break;
                case 2:
                    float_total *= operand.as_float.value;
                    break;
                case 3:
                    float_total /= operand.as_float.value;
                    break;
                }
            }
            operator = 0xFF;
        } else {
            word = *cursor;
            switch (word & 0xFF) {
            case 0xFF:
                cursor++;
                running = FALSE;
                break;
            case 4:
                operator = word >> 8;
                cursor++;
                break;
            }
        }
        want_operator ^= 1;
    } while (running);
    value = total;
    if (is_float) {
        bits.as_float = float_total;
        value = bits.as_int;
    }
    result.as_int.value = value;
    code = (u32)target.as_int.opcode >> 8;
    offset = target.as_int.value;
    switch (target.as_int.opcode & 0xFF) {
    case SCRIPT_ARG_GAME_VAR:
        Script_WriteGameVar(&result.as_int, code, offset);
        break;
    case SCRIPT_ARG_LOCAL_VAR:
        Script_WriteLocalVar(&result.as_int, thread, code, offset);
        break;
    case SCRIPT_ARG_SHARED_VAR:
        Script_WriteSharedVar(&result.as_int, code, offset);
        break;
    }
    return cursor;
}

/* Works out one argument of a command: a constant, or the value of a game, thread or shared variable;
   kinds 0x11 to 0x13 are passed on as they are, for a command that writes its answer there. */
static void Script_ReadArg(ScriptThread *thread, u32 *source, ScriptArg *result) {
    u32 word;
    int value;
    u32 code;
    u32 type;
    u64 kind; // match: a 64-bit switch value gives retail's dsll32/dsra32 on the table index
    char unused[0x50]; // match: retail reserves 0x50 bytes it never uses

    word = source[0];
    value = source[1];
    kind = word & 0xFF;
    type = word >> 16;
    code = word >> 8;
    switch (kind) {
    case SCRIPT_ARG_GAME_VAR:
        Script_ReadGameVar(result, code, value);
        break;
    case SCRIPT_ARG_LOCAL_VAR:
        Script_ReadLocalVar(result, thread, code, value);
        break;
    case SCRIPT_ARG_SHARED_VAR:
        Script_ReadSharedVar(result, code, value);
        break;
    case SCRIPT_ARG_CONSTANT:
        result->opcode = type;
        result->value = value;
        break;
    case SCRIPT_ARG_OUT_GAME_VAR:
    case SCRIPT_ARG_OUT_LOCAL_VAR:
    case SCRIPT_ARG_OUT_SHARED_VAR:
        result->opcode = word;
        result->value = value;
        break;
    default:
        D_00432920[SCRIPT_ERROR_BAD_ARGUMENT] = 1;
        break;
    }
}

/* Reads a game variable: the low byte of `code` picks the block in D_00433340, the byte above it the
   value's type. */
static void Script_ReadGameVar(ScriptArg *result, u32 code, int offset) {
    u8 *source;
    u16 type;

    source = D_00433340[code & 0xFF] + offset;
    type = code >> 8;
    switch (type) {
    case 0:
        result->value = *(s8 *)source;
        break;
    case 1:
        result->value = *source;
        break;
    case 2:
        result->value = *(s16 *)source;
        break;
    case 3:
        result->value = *(u16 *)source;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        result->value = *(s32 *)source;
        break;
    }
    result->opcode = type;
}

/* Reads one of the variables all threads share (the 0x400 bytes at shared_vars); the value's type is above
   the low byte of `code`. */
static void Script_ReadSharedVar(ScriptArg *result, u32 code, int offset) {
    u8 *source;
    u16 type;

    type = code >> 8;
    source = shared_vars + offset;
    if (offset > 0x3FC) {
        D_00432920[SCRIPT_ERROR_BAD_SHARED_VAR] = 1;
    }
    switch (type) {
    case 0:
        result->value = *(s8 *)source;
        break;
    case 1:
        result->value = *source;
        break;
    case 2:
        result->value = *(s16 *)source;
        break;
    case 3:
        result->value = *(u16 *)source;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        result->value = *(s32 *)source;
        break;
    }
    result->opcode = type;
}

/* Reads one of the thread's own variables; the value's type is above the low byte of `code`. */
static void Script_ReadLocalVar(ScriptArg *result, ScriptThread *thread, u32 code, int offset) {
    u8 *source;
    u16 type;

    type = code >> 8;
    source = thread->locals + offset;
    if (offset > 0x1FC) {
        D_00432920[SCRIPT_ERROR_BAD_LOCAL_VAR] = 1;
    }
    switch (type) {
    case 0:
        result->value = *(s8 *)source;
        break;
    case 1:
        result->value = *source;
        break;
    case 2:
        result->value = *(s16 *)source;
        break;
    case 3:
        result->value = *(u16 *)source;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        result->value = *(s32 *)source;
        break;
    }
    result->opcode = type;
}

/* Writes a game variable: the low byte of `code` picks the block in D_00433340, the byte above it the
   value's type. */
static void Script_WriteGameVar(ScriptArg *source, u32 code, int offset) {
    u8 *dest;
    u16 type;

    dest = D_00433340[code & 0xFF] + offset;
    type = code >> 8;
    switch (type) {
    case 0:
    case 1:
        *dest = source->value;
        break;
    case 2:
    case 3:
        *(u16 *)dest = source->value;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        *(s32 *)dest = source->value;
        break;
    }
}

/* Writes one of the variables all threads share; the value's type is above the low byte of `code`. */
static void Script_WriteSharedVar(ScriptArg *source, u32 code, int offset) {
    u8 *dest;
    u16 type;

    type = code >> 8;
    dest = shared_vars + offset;
    if (offset > 0x3FC) {
        D_00432920[SCRIPT_ERROR_BAD_SHARED_VAR] = 1;
    }
    switch (type) {
    case 0:
    case 1:
        *dest = source->value;
        break;
    case 2:
    case 3:
        *(u16 *)dest = source->value;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        *(s32 *)dest = source->value;
        break;
    }
}

/* Writes one of the own variables of the thread being stepped (current_thread); the value's type is above the
   low byte of `code`. */
static void Script_WriteCurrentLocalVar(ScriptArg *source, u32 code, int offset) {
    u8 *dest;
    u16 type;

    type = code >> 8;
    dest = current_thread->locals + offset;
    if (offset > 0x1FC) {
        D_00432920[SCRIPT_ERROR_BAD_LOCAL_VAR] = 1;
    }
    switch (type) {
    case 0:
    case 1:
        *dest = source->value;
        break;
    case 2:
    case 3:
        *(u16 *)dest = source->value;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        *(s32 *)dest = source->value;
        break;
    }
}

/* Writes one of the thread's own variables; the value's type is above the low byte of `code`. */
static void Script_WriteLocalVar(ScriptArg *source, ScriptThread *thread, u32 code, int offset) {
    u8 *dest;
    u16 type;

    type = code >> 8;
    dest = thread->locals + offset;
    if (offset > 0x1FC) {
        D_00432920[SCRIPT_ERROR_BAD_LOCAL_VAR] = 1;
    }
    switch (type) {
    case 0:
    case 1:
        *dest = source->value;
        break;
    case 2:
    case 3:
        *(u16 *)dest = source->value;
        break;
    case 4:
    case 5:
    case SCRIPT_TYPE_FLOAT:
        *(s32 *)dest = source->value;
        break;
    }
}

/* Starts a command: takes a free command of the thread, sets its number from the high half of the script
   word, reads as many arguments as the low half says and clears its work words. Returns where the script
   goes on. */
static u32 *Script_StartCommand(ScriptThread *thread, u32 *cursor) {
    u32 header;
    u32 number;
    int arg_count;
    int index;
    int arg;

    header = *cursor++;
    arg_count = header & 0xFFFF;
    number = header >> 16;
    index = Script_FindFreeCommand(thread);
    thread->commands[index].active = number;
    for (arg = 0; arg < arg_count; arg++) {
        Script_ReadArg(thread, cursor, &thread->commands[index].args[arg]);
        cursor += 2;
    }
    Script_FillWords(&thread->commands[index].args[16], 8, 0);
    return cursor;
}

/* The handler of word 0x19D: returns 0, so the thread's cursor becomes NULL and it halts. */
static int Script_HaltCursor(void) {
    return 0;
}

/* Hands a command's answer back to the script: writes `value` to the variable the argument `target` names
   (kinds 0x11 to 0x13); any other kind sets an error flag. */
int Script_StoreResult(ScriptArg *target, int value) {
    ScriptArg operand;
    int kind;
    int code;
    int offset;

    kind = target->opcode;
    offset = target->value;
    operand.value = value;
    code = kind >> 8;
    switch (kind & 0xFF) {
    case SCRIPT_ARG_OUT_GAME_VAR:
        return ((int (*)(ScriptArg *, int, int))Script_WriteGameVar)(&operand, code, offset); // match: called as returning int
    case SCRIPT_ARG_OUT_LOCAL_VAR:
        return ((int (*)(ScriptArg *, int, int))Script_WriteCurrentLocalVar)(&operand, code, offset); // match: called as returning int
    case SCRIPT_ARG_OUT_SHARED_VAR:
        return ((int (*)(ScriptArg *, int, int))Script_WriteSharedVar)(&operand, code, offset); // match: called as returning int
    default:
        D_00432920[SCRIPT_ERROR_BAD_INT_TARGET] = 1;
    }
}

/* The float form of Script_StoreResult: writes the bits of `value` to the variable `target` names. */
int Script_StoreFloatResult(ScriptArg *target, f32 value) {
    ScriptArg operand;
    FloatBits bits;
    int kind;
    int code;
    int offset;
    int result;

    bits.as_float = value;
    kind = target->opcode;
    offset = target->value;
    operand.value = bits.as_int;
    code = kind >> 8;
    switch (kind & 0xFF) {
    case SCRIPT_ARG_OUT_GAME_VAR:
        return ((int (*)(ScriptArg *, int, int))Script_WriteGameVar)(&operand, code, offset); // match: called as returning int
    case SCRIPT_ARG_OUT_LOCAL_VAR:
        return ((int (*)(ScriptArg *, int, int))Script_WriteCurrentLocalVar)(&operand, code, offset); // match: called as returning int
    case SCRIPT_ARG_OUT_SHARED_VAR:
        return ((int (*)(ScriptArg *, int, int))Script_WriteSharedVar)(&operand, code, offset); // match: called as returning int
    }
    D_00432920[SCRIPT_ERROR_BAD_FLOAT_TARGET] = result = TRUE;
    return result;
}

/* Command 0, and every unused number below 100: does nothing and never finishes, so the thread waits
   until something stops it. */
void ScriptCmd_Stall(void) {
}

/* Command 1: waits `value` frames. */
void ScriptCmd_Wait(ScriptValueCommand *command) {
    command->value.value--;
    if (command->value.value <= 0) {
        command->active = 0;
    }
}

/* Command 2: starts the script `script_id` (even if it is running already) and, unless `no_wait` is 1,
   waits until its thread is free again. */
void ScriptCmd_RunScript(ScriptRunScriptCommand *command) {
    if (command->started == 0) {
        command->started = 1;
        command->thread_index = Script_StartAlways(command->script_id.value, 0);
        if (command->no_wait.value == 1) {
            command->active = 0;
        }
    } else if (Script_IsThreadBusy(command->thread_index) == 0) {
        command->active = 0;
    }
}

/* Command 3: finishes at once. */
void ScriptCmd_Nop3(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 4, and what word 0x19D starts: never finishes, so the thread waits until something stops it. */
void ScriptCmd_WaitForever4(void) {
}

/* Command 5: stops every script: frees all 24 threads and their commands, this one too. */
void ScriptCmd_StopAllScripts(ScriptThreadCommand *command) {
    int thread;
    int next;
    int index;

    for (thread = 0; thread < 24; thread = next) {
        next = thread + 1; // match: retail keeps thread + 1 in a register of its own across the inner loop
        threads[thread].script_id = 0;
        for (index = 15; index >= 0; index--) {
            threads[thread].commands[index].active = 0;
        }
    }
    command->active = 0;
}

/* Command 8: finishes at once. */
void ScriptCmd_Nop8(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 6: finishes at once. */
void ScriptCmd_Nop6(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 7: finishes at once. */
void ScriptCmd_Nop7(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 9: hands the script, through its eighth argument, a value that is never set, then finishes. */
void ScriptCmd_StoreUninitializedResult9(ScriptLateResultCommand *command) {
    int unused[4];
    int value;

    Script_StoreResult(&command->result, value); // match: retail passes whatever is in the register
    command->active = 0;
}

/* Command 10: the same as command 9. */
void ScriptCmd_StoreUninitializedResult10(ScriptLateResultCommand *command) {
    int unused[4];
    int value;

    Script_StoreResult(&command->result, value); // match: retail passes whatever is in the register
    command->active = 0;
}

/* Command 15: finishes at once. */
void ScriptCmd_Nop15(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 16: finishes at once. */
void ScriptCmd_Nop16(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 17: finishes at once. */
void ScriptCmd_Nop17(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 18: finishes at once. */
void ScriptCmd_Nop18(ScriptThreadCommand *command) {
    command->active = 0;
}

/* Command 19: never finishes, so the thread waits until something stops it. */
void ScriptCmd_WaitForever19(void) {
}

/* Command 20: never finishes, so the thread waits until something stops it. */
void ScriptCmd_WaitForever20(void) {
}

/* Command 29: sets the camera's word at 0x48 (D_004325E8, which func_001127F8 starts at 512.0f) to the
   float argument, then finishes. No reader of the word is known. */
void ScriptCmd_SetCameraUnk48(ScriptFloatValueCommand *command) {
    D_004325E8[0] = command->value.value;
    command->active = 0;
}

/* Command 30: Recollection page `value` shows its next description (the third at most), then finishes. */
void ScriptCmd_AdvanceRecollectionPage(ScriptValueCommand *command) {
    u32 page;
    u8 *description;

    page = command->value.value;
    if (page < 50U) {
        description = recollection_pages + page;
        if (*description < 3U) {
            *description += 1;
        }
    }
    command->active = 0;
}

/* Command 31: hands the script the description Recollection page `input` shows (-1 when there is no such
   page), then finishes. */
void ScriptCmd_GetRecollectionPage(ScriptQueryCommand *command) {
    u32 index;

    index = command->input.value;
    if (index < 50) {
        Script_StoreResult(&command->result, recollection_pages[index]);
    } else {
        Script_StoreResult(&command->result, -1);
    }
    command->active = 0;
}

/* Command 32: sets which description Recollection page `first` shows to `second` (0 to 3), then
   finishes. */
void ScriptCmd_SetRecollectionPage(ScriptPairCommand *command) {
    u32 index;

    index = command->first.value;
    if (index < 50 && command->second.value >= 0 && command->second.value < 4) {
        recollection_pages[index] = command->second.value;
    }
    command->active = 0;
}

/* Command 33: shows the splash image `object_id` and waits until it is gone. */
void ScriptCmd_ShowSplash(ScriptWaitCommand *command) {
    if (command->started == 0) {
        if (Splash_Start(command->object_id.value) != 0) {
            command->started = 1;
        }
    } else if (Splash_GetState() == 0) {
        command->active = 0;
    }
}

/* Command 34: sets both volumes (options bytes 1 and 2 of D_00349680, through Sound_SetBgmVolume and
   Sound_SetBankVolumes) to `value` 64ths (0 to 64) of what the options say, then finishes. */
void ScriptCmd_SetVolumes(ScriptValueCommand *command) {
    if (command->value.value >= 0 && command->value.value <= 64) {
        Sound_SetBgmVolume(D_00349680[1] * command->value.value / 64);
        Sound_SetBankVolumes(D_00349680[2] * command->value.value / 64);
    }
    command->active = 0;
}

/* Command 35: sets the sound effect volume (the nine voices Sound_SetBankVolumes sets; the options' byte 2) to
   `value` 64ths of it, then finishes. */
void ScriptCmd_SetSeVolume(ScriptValueCommand *command) {
    if (command->value.value >= 0 && command->value.value <= 64) {
        Sound_SetBankVolumes(D_00349680[2] * command->value.value / 64);
    }
    command->active = 0;
}

/* Command 38: moves what Recollection page `first` shows to page `second`, then finishes. */
void ScriptCmd_MoveRecollectionPage(ScriptPairCommand *command) {
    Game_MoveRecollectionPage(command->first.value, command->second.value);
    command->active = 0;
}

/* Command 39: marks the message `value` as heard (Game_MarkMessageHeard), then finishes. */
void ScriptCmd_MarkMessageHeard(ScriptValueCommand *command) {
    Game_MarkMessageHeard(command->value.value);
    command->active = 0;
}

/* Command 40: asks for the step 3 sound load of `value` unless it is the one asked for already, then
   finishes. */
void ScriptCmd_RequestSoundLoad(ScriptValueCommand *command) {
    if (D_0038A130.unk00 != command->value.value) {
        D_0038A130.unk00 = command->value.value;
        D_0038A130.unk04 = 1;
    }
    command->active = 0;
}

/* Command 36: sets the BGM volume (the one voice Sound_SetBgmVolume sets; the options' byte 1) to `value`
   64ths of it, then finishes. */
void ScriptCmd_SetBgmVolume(ScriptValueCommand *command) {
    u32 value;

    value = command->value.value;
    if (value < 65) {
        Sound_SetBgmVolume((int)(D_00349680[1] * value) / 64);
    }
    command->active = 0;
}

/* Command 37: sets the player's HP to 0 (func_001BB5C0; not while PlayerState.unk1A8 is set), then
   finishes. */
void ScriptCmd_KillPlayer(ScriptThreadCommand *command) {
    func_001BB5C0();
    command->active = 0;
}

/* Command 41: shows the splash image `splash_id` with the given timing and waits until it is gone. */
void ScriptCmd_ShowSplashTimed(ScriptSplashCommand *command) {
    if (command->started == 0) {
        if (Splash_Begin(command->splash_id.value, command->hold_frames.value, command->skippable.value,
                         command->fade_in_frames.value, command->fade_out_frames.value) != 0) {
            command->started = 1;
        }
    } else if (Splash_GetState() == 0) {
        command->active = 0;
    }
}

/* Command 42: hands the script, through `first`, the 4-bit counter its map keeps for the entity
   `target_id` (func_00103548), -1 when there is no such entity, then finishes. */
void ScriptCmd_GetEntityCounter(ScriptCallCommand *command) {
    Entity *entity;
    int map_slot;
    int entry_index;

    entity = Entity_GetBySpawnId(command->target_id.value);
    if (entity != NULL) {
        map_slot = entity->group_id;
        entry_index = func_00103548(
            D_0038CD20[map_slot].tag,
            (s32)(((u32)((char *)entity - (char *)D_003BEDF0[map_slot])) / sizeof(Entity))); // match: divu
        Script_StoreResult(&command->first, entry_index);
    } else {
        Script_StoreResult(&command->first, -1);
    }
    command->active = 0;
}

/* Command 43: empties the player's two gauges (the weapon's, unkBC, and the magic one, unkC0), then
   finishes. */
void ScriptCmd_EmptyPlayerGauges(ScriptThreadCommand *command) {
    D_00413F80.unkC0 = D_00413F80.unkBC = 0;
    command->active = 0;
}

/* Returns 0. Nothing calls it. */
static int Script_UnusedStub(void) {
    return 0;
}
