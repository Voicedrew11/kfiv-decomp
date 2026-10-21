/* The game's sound layer over the driver (sound/sound.c): voices, volumes, sounds played at a
   point and panned by where it lies from the player, tagged (looping) sounds, BGM, bank loading and the
   reverb settings of rev_para.dat. */
#ifndef KF_SOUND_GAME_H
#define KF_SOUND_GAME_H

#include "common.h"
#include "sound/sound.h"
#include "core/load.h"
#include "map/map.h"
#include "model/model.h"

/* Starts the sound driver and resets the sound state: loads the se_com and sfx_com banks into the heap,
   reads rev_para.dat, and frees every channel, queued request and claimed voice */
void Sound_Init(void);
/* Applies the stereo option (game_options[0]) to the driver: SoundDriver_SetStereo when set, else SoundDriver_SetMono */
void Sound_ApplyStereoOption(void);
/* Once a frame: starts the queued tagged sounds; then, unless a reverb change is pending (reverb_change_pending),
   updates the tagged channels and the map's reverb volume and sends the frame's note commands */
void Sound_Update(void);
/* Sets the BGM voice's volume (SoundDriver_SetVoiceVolume) to level, kept within 0..0x7F: the music volume option */
void Sound_SetBgmVolume(s32 level);
/* Sets one volume (SoundDriver_SetVoiceVolume) on the voices of the five sound banks and on voices 6 to 9 */
void Sound_SetBankVolumes(int level);
/* Sets the BGM fade level (bgm_fade_level) to the music volume option (music_volume) */
void Sound_SyncBgmLevel(void);
/* Starts a tracked sound with the player of `kind` in tracked_sound_players (0 Sound_PlayTracked, 1
   Sound_PlayTrackedAt) */
char Sound_PlayTrackedKind(int index, int first, int second, int third, int fourth);
/* Starts a sound on the next tracked voice (0x64..0x7D) whose tracked_voices slot is free, from the mg bank
   (sound below 8) or the sfx_com bank, at the sound volume option; returns the voice, -1 if 11 tries find
   none */
/* the two ways of starting a tracked sound (sound/sound_game.c) */
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
/* Plays a map bank sound at a point (while map_sounds_enabled is set): the volume falls off with the distance from
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
/* Plays a map bank sound at volume (scaled by the sound option) with a random bend; -1 while map_sounds_enabled is
   clear */
int Sound_PlayMapVolume(u8 sound, int volume);
/* Plays a map bank sound at a point within range, the volume falling off by mode (0 and 6 linear, 1 and 7
   steep, 2 and 8 round), panned; a random bend for modes below 3 */
int Sound_PlayMapAtRange(u8 sound, u8 mode, float *position, float range);
/* Starts a tagged sound (channel kind 0x63, no position) of the map bank at volume; queued in tagged_requests
   while map_sounds_enabled is clear */
int Sound_StartTagged(s8 sound, u8 voice, int volume);
/* Starts a tagged sound at a point, heard within range (SoundChannel.volume); queued in tagged_requests while
   map_sounds_enabled is clear */
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
/* BGM load step: once the driver is free, fades the playing BGM down by 3 a frame; done at 0 */
int Sound_BgmFadeOut(IopLoadJob *job);
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
/* BGM load step: loads the sequence (SoundDriver_LoadSequenceStep) and starts it playing; -1 until it is loaded */
int Sound_BgmStartSequence(IopLoadJob *job);
/* BGM load step: raises the BGM fade level by 3 a frame (applied unless the bgm is 0x12) until it passes the
   music volume option */
int Sound_BgmFadeIn(int unused, int mode);
/* BGM load step: frees the last BGM's reverb slot and stores the reverb settings of BGM cue_index (none for 0x12) */
int Sound_BgmStartCue(void *context, int cue_index);
/* BGM load step: unless the bgm is 0x12, waits for the driver and sends the BGM's reverb settings, flagging
   the change (reverb_change_pending) */
int Sound_BgmApplyReverb(void *actor, int mode);
/* Reads the file request->name formats with the current map's file_index into a new buffer of the sound heap, a
   step a call; 1 when done. Nothing calls it */
s32 Sound_ReadMapBankFileStep(LoadRequest *request);
/* BGM load step: loads the BGM's voice (SoundDriver_LoadVoiceStep) and keeps it in the job */
int Sound_BgmLoadVoice(IopLoadJob *job);
/* BGM load step: once the transfer to sound memory is done, frees the IOP copy of the voice's samples */
int Sound_BgmFreeSampleBuffer(IopLoadJob *job);
/* Loads BGM bgm a step a call while pending; returns 0 once it is the loaded one */
int Sound_LoadBgm(int bgm, int pending);
/* Whether the BGM load may be set aside for another sound load (bgm_load_can_yield) */
int Sound_BgmLoadCanYield(void);
/* Load step (Sound_RunFileLoad): reads the file job->name into the job's first buffer, waiting for it */
int Sound_ReadJobFile(LoadJob *job);
/* Load step (Sound_RunFileLoad): reads the file request->name into the request's buffer, waiting for it */
s32 Sound_ReadRequestFile(LoadRequest *request);
/* Load step (Sound_RunFileLoad): as Sound_BgmLoadVoice */
int Sound_FileLoadVoice(IopLoadJob *job);
/* Load step (Sound_RunFileLoad): as Sound_BgmFreeSampleBuffer */
int Sound_FileLoadFreeSampleBuffer(IopLoadJob *job);
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
/* Entity bank load step: loads the voice from the job's spec (SoundDriver_LoadVoiceStep) */
int Sound_EntityBankLoadVoice(SoundJob *job);
/* Entity bank load step: once the transfer to sound memory is done, frees the IOP copy of the samples */
int Sound_EntityBankFreeSampleBuffer(SoundJob *job);
/* Loads the sound bank of the entity type file just read into voice index + 6, a step a call; 0 and the
   voice in *voice_out once done, else 1 */
int Sound_LoadEntityBankStep(int args_addr, int index, int voice_out);
/* Plays sound from bank with the falloff player kind of falloff_sound_players */
#ifndef SOUND_PLAY_FALLOFF_DEFINITION
// match: the definition names only the three integer arguments, which keeps retail's registers
void Sound_PlayFalloff(int sound, int kind, int bank, f32 range, f32 distance, f32 angle);
#endif
/* Map bank load step: does nothing but move on */
s32 Sound_MapBankNoOp(SoundJob *job);
/* Map bank load step: waits for a pending reverb change, then loads the map's voice (SoundDriver_LoadVoiceStep) */
int Sound_MapBankLoadVoice(SoundJob *job);
/* Map bank load step: once the transfer to sound memory is done, frees the IOP copy of the samples */
int Sound_MapBankFreeSampleBuffer(SoundJob *job);
/* Map bank load step: frees the last map's reverb slot and stores the map's reverb settings (map_reverb_cue_indices) */
int Sound_MapBankAddReverb(SoundJob *job);
/* Map bank load step: once the upload is done, sends the map's reverb settings and sets map_sounds_enabled (map
   sounds may start) */
int Sound_MapBankApplyReverb(SoundJob *job);
/* Map bank load step: the first time after Sound_Init moves on to the reverb steps; after that lets map
   sounds start (map_sounds_enabled) and ends the load */
int Sound_MapBankFinishUnlessFirst(SoundJob *job);
/* Loads the sound bank of a map slot (MapSlot.sound_bank) a step a call while pending; returns 0 once it is done */
int Sound_LoadMapBankStep(int slot_addr, int pending);
/* Loads the sound bank of a map slot (MapSlot.sound_bank), waiting until it is done */
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
/* Frees the IOP copy of a voice's samples; returns 0 */
int Sound_FreeSampleBuffer(int voice_index);

/* the bank buffers and load jobs (sound_tables.c) */
extern SoundBankSlot se_com_bank;
extern SoundBankSlot sfx_com_bank;
extern SoundBankSlot sw_bank;
extern SoundBankSlot mg_bank;
extern LoadJobSlot bgm_load_job;
extern SoundJobSlot se_com_job;
extern SoundJobSlot sfx_com_job;
extern SoundJobSlot map_bank_job;
extern SoundJobSlot sw_bank_job;
extern SoundJobSlot mg_bank_job;

/* data still in asm */
extern char rev_para_path[];
extern SoundScriptState map_reverb_cue_indices;
extern int bgm_load_step;
extern int bgm_load_can_yield;
extern int bgm_fade_level;
extern int entity_bank_load_step;
extern int map_bank_load_step;
extern u8 voice_range_cursors[8];
extern int tagged_bend;
extern int tagged_bend_high;
extern int tagged_bend_low;
extern char tagged_bend_falling;
extern u8 map_reverb_slot;
extern u8 bgm_reverb_slot;
extern int heap_file_read_step;
extern s32 map_bank_file_read_step;
extern s32 sound_file_load_step;
extern int map_bank_unread_flag;  /* cleared by Sound_MapBankFinishUnlessFirst; nothing reads it */
extern int sound_unread_limit;  /* Sound_Init sets it to 0x40; nothing reads it */
extern float sound_unread_scale;  /* Sound_Init sets it to 0.013; nothing reads it */
extern int sound_unread_switch;  /* Sound_Init sets it to 1; nothing reads it */
extern int heap_file_handle;
extern s32 map_bank_file_handle;
extern s8 map_reverb_set;
extern char map_sounds_enabled;
extern char reverb_change_pending;
extern s8 map_reverb_volume;
extern u8 loaded_bgm;
extern u8 bgm_load_running;
extern s8 positional_sounds_muted;
extern u8 loading_bgm;
extern int bgm_bd_size;
extern int bgm_hd_size;
extern int bgm_sq_size;
extern int sound_heap_next;

#endif
