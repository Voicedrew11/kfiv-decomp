/* The game's sound layer over the driver (kf_sound.c): voices, volumes, sounds played at a
   point and panned by where it lies from the player, tagged (looping) sounds, BGM and bank loading. */
#ifndef KF_SOUND_GAME_H
#define KF_SOUND_GAME_H

#include "common.h"
#include "kf_sound.h"
#include "kf_load.h"
#include "kf_map.h"
#include "kf_model.h"

/* Starts the sound driver and resets the sound state: loads the se_com and sfx_com banks into the heap,
   reads rev_para.dat, and frees every channel, queued request and claimed voice */
void Sound_Init(void);
/* Applies option 0 (D_00349680[0]) to the driver: func_001CC018 when it is set, func_001CBED8 when not */
void func_0019C520(void);
/* Once a frame: starts the queued tagged sounds; then, unless a cue change is pending (D_004470E2), updates
   the tagged channels, the map's effect level and the driver; otherwise waits for the driver to finish it */
void Sound_Update(void);
/* Sets the BGM voice's volume (func_001CDBC0) to level, kept within 0..0x7F: the music volume option */
void Sound_SetBgmVolume(s32 level);
/* Sets one volume (func_001CDBC0) on the voices of the five sound banks and on voices 6 to 9 */
void Sound_SetBankVolumes(int level);
/* Sets the BGM fade level (D_00446860) to the music volume option (D_00349681) */
void Sound_SyncBgmLevel(void);
/* Starts a tracked sound with the player of `kind` in D_003EF700 (0 Sound_PlayTracked, 1
   Sound_PlayTrackedAt) */
char Sound_PlayTrackedKind(int index, int first, int second, int third, int fourth);
/* Starts a sound on the next tracked voice (0x64..0x7D) whose D_00502170 slot is free, from the mg bank
   (sound below 8) or the sfx_com bank, at the sound volume option; returns the voice, -1 if 11 tries find
   none */
int Sound_PlayTracked(int sound, int unused, float *position);
/* As Sound_PlayTracked from the sfx_com bank, at a volume that falls off with the distance from the player
   (silent at 2500) and panned by the side the point lies on */
int Sound_PlayTrackedAt(int sound, int unused, float *position);
/* Turns the volume of every playing tracked voice to 0 */
void Sound_MuteTracked(void);
/* Gives every playing tracked voice its volume back */
void Sound_UnmuteTracked(void);
/* Stops a tracked voice and frees its slot */
void Sound_StopTracked(s8 slot);
/* Stops a voice (nothing for 0 or less) */
void Sound_StopVoice(s8 slot);
/* Plays a sound of the se_com bank on voice 1 unless it is 0xFF (none): the menu sounds */
s32 Sound_PlayMenu(s32 sound);
/* Plays a sound of the sw bank on voice 2 */
void Sound_PlaySw(int mode);
/* Plays a map bank sound at a point (while D_004470E1 is set): the volume falls off with the distance from
   the player (silent at 2500), panned by side, with a random bend */
int Sound_PlayMapAt(u8 sound, void *position);
/* Plays a footstep sound (0x20..0x27) from the se_com bank (below 0x22, 0x24, 0x25) or the map bank, with a
   random bend; returns the voice, -1 for other sounds */
int Sound_PlayFootstep(int sound);
/* Plays a sound of the se_com bank on the next voice of group 1; returns the voice */
s32 Sound_PlayCommon(s32 value);
/* Plays a sound of the map bank on the next voice of group 1; returns the voice */
int Sound_PlayMap(int value);
/* Plays a se_com sound at a point within range: the volume falls off linearly, panned by where the point
   lies from the player's facing; returns the voice */
int Sound_PlayCommonAt(float range, int sound, float *position);
/* Plays a map bank sound at volume (scaled by the sound option) with a random bend; -1 while D_004470E1 is
   clear */
int Sound_PlayMapVolume(u8 sound, int volume);
/* Plays a map bank sound at a point within range, the volume falling off by mode (0 and 6 linear, 1 and 7
   steep, 2 and 8 round), panned; a random bend for modes below 3 */
int Sound_PlayMapAtRange(u8 sound, u8 mode, float *position, float range);
/* Starts a tagged sound (channel kind 0x63, no position) of the map bank at volume; queued in D_00500170
   while D_004470E1 is clear */
int Sound_StartTagged(s8 sound, u8 voice, int volume);
/* Starts a tagged sound at a point, heard within range (SoundChannel.volume); queued in D_00500170 while
   D_004470E1 is clear */
s32 Sound_StartTaggedAt(s8 sound_id, u8 voice, u8 mode, f32 volume, Vector4Arg *position);
/* As Sound_StartTaggedAt for a sound along the segment origin..target */
s32 Sound_StartTaggedAlong(s8 sound_id, u8 voice, u8 mode, Vector4Arg *origin, Vector4Arg *target, f32 volume);
/* Each frame, updates the voice of every positioned tagged sound: stops it out of range, else sets its
   volume (by its falloff mode) and pan, with a wandering bend, and gives it a voice when it has none */
void Sound_UpdateTagged(void);
/* Turns the volume of every playing unpositioned tagged sound (kind 0x63) to 0 */
void Sound_MuteTagged(void);
/* Gives every playing unpositioned tagged sound its volume back */
void Sound_UnmuteTagged(void);
/* Stops the tagged sound with tag `tag` and frees its channel; -1 if there is none */
int Sound_StopTagged(char sound);
/* Silences every sound voice and marks all channels (and optionally all queued requests) free */
void Sound_StopAll(int clear_queue);
/* BGM load step: opens the job's .bd file for the bgm number at a 64-byte aligned address and starts reading
   it; -1 while the file layer or the driver is busy */
int Sound_BgmReadBody(LoadJob *job, int index);
/* BGM load step: once the driver is free, gets the voice (func_001CD020) and the bank (func_001CDF90) of the
   BGM if they are missing, then fades the BGM down by 3 a frame; done at 0 */
int func_0019EC90(IopLoadJob *job);
/* BGM load step: once the read has finished, closes the file and frees the old BGM's bank and voice */
int Sound_BgmCloseBody(LoadJob *job);
/* BGM load step: opens the job's .hd file for the bgm number and starts reading it into the spec's second
   buffer; -1 while busy */
int Sound_BgmReadHeader(LoadJob *job, int index);
/* BGM load step: once the read has finished, closes the file */
int Sound_BgmCloseHeader(LoadJob *job);
/* BGM load step: opens the job's .sq file for the bgm number and starts reading it into the third buffer; -1
   while busy */
int Sound_BgmReadSequence(LoadJob *job, int index);
/* BGM load step: once the read has finished, closes the file */
int Sound_BgmCloseSequence(LoadJob *job);
/* BGM load step: hands the sequence to the driver (func_001CDF90) and calls func_001CE468 on the bank it
   gives; -1 until it does */
int func_0019F090(IopLoadJob *job);
/* BGM load step: raises the BGM fade level by 3 a frame (applied unless the bgm is 0x12) until it passes the
   music volume option */
int Sound_BgmFadeIn(int unused, int mode);
/* BGM load step: ends the cue of the last BGM and starts cue `bgm` of the rev_para.dat table (none for 0x12) */
int Sound_BgmStartCue(void *context, int cue_index);
/* BGM load step: unless the bgm is 0x12, waits for the driver and calls func_001CEFA0 on the cue, flagging
   the change (D_004470E2) */
int func_0019F220(void *actor, int mode);
/* Reads the file request->name formats with the current map's unk70 into a new buffer of the sound heap, a
   step a call; 1 when done */
s32 func_0019F3A0(LoadRequest *request);
/* Load step: hands the job's spec to the driver (func_001CD020) and keeps the result */
int func_0019F498(IopLoadJob *job);
/* Load step: once the driver is free, calls func_001CD7E0 on the job's result */
int func_0019F4F8(IopLoadJob *job);
/* Loads BGM bgm a step a call while pending; returns 0 once it is the loaded one */
int Sound_LoadBgm(int bgm, int pending);
/* Returns D_0044685C, which the BGM load steps clear while they work */
int func_0019F6B0(void);
/* Load step: reads the file job->name into the job's first buffer, waiting for it */
int func_0019F6B8(LoadJob *job);
/* Load step: reads the file request->name into the request's buffer, waiting for it */
s32 func_0019F788(LoadRequest *request);
/* Load step: as func_0019F498 */
int func_0019F7F0(IopLoadJob *job);
/* Load step: as func_0019F4F8 */
int func_0019F850(IopLoadJob *job);
/* Plays a sound of bank while distance is within range: volume falls off linearly, panned by angle, with a
   random bend */
void Sound_PlayLinear(u8 sound, int bank, float range, float distance, float angle);
/* As Sound_PlayLinear with the steep falloff (loudest near, 1.5 times as loud at the point) */
void Sound_PlaySteep(u8 sound, int bank, float range, float distance, float angle);
/* As Sound_PlayLinear with the round falloff (the root of 1 - (distance / range) squared) */
void Sound_PlayRound(u8 sound, int bank, float range, float distance, float angle);
/* As Sound_PlayLinear without the bend */
void Sound_PlayLinearSteady(u8 sound, int bank, float range, float distance, float angle);
/* As Sound_PlaySteep without the bend */
void Sound_PlaySteepSteady(u8 sound, int bank, float range, float distance, float angle);
/* As Sound_PlayRound without the bend */
void Sound_PlayRoundSteady(u8 sound, int bank, float range, float distance, float angle);
/* Sound_PlayLinear from the sfx_com bank */
void Sound_PlaySfxLinear(u8 sound, float range, float distance, float angle);
/* Sound_PlaySteep from the sfx_com bank */
void Sound_PlaySfxSteep(u8 sound, float range, float distance, float angle);
/* Sound_PlayRound from the sfx_com bank */
void Sound_PlaySfxRound(u8 sound, float range, float distance, float angle);
/* Sound_PlayLinear from the map bank */
void Sound_PlayMapLinear(u8 sound, float range, float distance, float angle);
/* Sound_PlaySteep from the map bank */
void Sound_PlayMapSteep(u8 sound, float range, float distance, float angle);
/* Sound_PlayRound from the map bank */
void Sound_PlayMapRound(u8 sound, float range, float distance, float angle);
/* Load step: hands the job's spec to the driver (func_001CD020) and keeps the result */
int func_001A0B10(SoundJob *job);
/* Load step: once the driver is free, calls func_001CD7E0 on the job's result */
int func_001A0B70(SoundJob *job);
/* Loads the bank of args into driver slot index + 6, a step a call; 0 and the slot in *voice_out once done,
   else 1 */
int func_001A0C18(int args_addr, int index, int voice_out);
/* Plays sound from bank with the falloff player kind of D_0043EE30 */
void Sound_PlayFalloff(int sound, int kind, int bank);
/* Map bank load step: moves on */
s32 func_001A0D08(void);
/* Map bank load step: waits for a pending cue change, then hands the job's spec to the driver
   (func_001CD020) */
int func_001A0D20(SoundJob *job);
/* Map bank load step: once the driver is free, calls func_001CD7E0 on the job's result */
int func_001A0D98(SoundJob *job);
/* Map bank load step: ends the last map cue and starts the one D_0043EE78 names from the second cue table */
int func_001A0DF8(void);
/* Map bank load step: once the driver is free, calls func_001CEFA0 on the cue and sets D_004470E1 (sounds
   may start) */
int func_001A0F60(void);
/* Map bank load step: moves on; the first time through sets D_004470E0 and returns 0, after that sets
   D_004470E1 and returns 1 */
int func_001A0FC8(void);
/* Loads the sound bank of a map slot (MapSlot.unk34) a step a call while pending; returns 0 once it is done */
int Sound_LoadMapBankStep(int slot_addr, int pending);
/* Loads the sound bank of a map slot (MapSlot.unk34), waiting until it is done */
int Sound_LoadMapBank(MapSlot *slot);
/* Loads the sw bank file_id (sw%03d with 0x96 added) into the sound heap and hands it to the driver, waiting
   until it is done */
void Sound_LoadSwBank(int file_id);
/* Loads the mg bank file_number (mg%03d) into the sound heap and hands it to the driver, waiting until it is
   done */
void Sound_LoadMgBank(int file_number);
/* Loads the two files named in a job and hands them to the driver, waiting until it is done */
void Sound_LoadJob(SoundJob *slot);
/* Lets Sound_UpdateTagged give positioned tagged sounds their volume again */
void Sound_UnmutePositional(void);
/* Makes Sound_UpdateTagged keep positioned tagged sounds at volume 0 */
void Sound_MutePositional(void);

#endif
