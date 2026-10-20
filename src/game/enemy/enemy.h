/* Enemy AI entry points, one per enemy file (enemy/*.c). */
#ifndef KF_ENEMY_H
#define KF_ENEMY_H

#include "common.h"
#include "entity/entity.h"
#include "model/model.h"

/* enemy/acid_crawler.c */
void AcidCrawler_Init(Entity *entity);

/* enemy/ancient_king.c */
void AncientKing_Init(Entity *entity);

/* enemy/ancient_mummy.c */
void AncientMummy_Init(Entity *entity);

/* enemy/aqualizard.c */
void Aqualizard_Init(Entity *entity);

/* enemy/beetle.c */
void Beetle_Init(Entity *entity);

/* enemy/bell.c */
void Bell_Init(Entity *entity);

/* enemy/blood_bat.c */
void BloodBat_Init(Entity *entity);

/* enemy/critter.c */
void Critter_Init(Entity *entity);

/* enemy/crow.c */
void Crow_Init(Entity *entity);

/* enemy/dark_archer.c */
void DarkArcher_Init(Entity *entity);

/* enemy/dark_bird.c */
void DarkBird_Init(Entity *entity);

/* enemy/dark_flower.c */
void DarkFlower_Init(Entity *entity);

/* enemy/dark_knight.c */
void DarkKnight_Init(Entity *entity);

/* enemy/dark_one.c */
void DarkOne_Init(Entity *entity);

/* enemy/dark_puppet.c */
void DarkPuppet_Init(Entity *entity);

/* enemy/dark_soldier.c */
void DarkSoldier_Init(Entity *entity);

/* enemy/dark_sorcerer.c */
void DarkSorcerer_Init(Entity *entity);

/* enemy/dark_succubus.c */
void DarkSuccubus_Init(Entity *entity);

/* enemy/dragon_fly.c */
void DragonFly_Init(Entity *entity);

/* enemy/earth_elemental.c */
void EarthElemental_Init(Entity *entity);

/* enemy/earth_king.c */
void EarthKing_Init(Entity *entity);

/* enemy/expedition_archer.c */
void ExpeditionArcher_Init(Entity *entity);

/* enemy/expedition_soldier.c */
void ExpeditionSoldier_Init(Entity *entity);

/* enemy/eye_pedestal.c */
void EyePedestal_Init(Entity *entity);

/* enemy/fadin.c */
void Fadin_Init(Entity *entity);

/* enemy/fire_elemental.c */
void FireElemental_Init(Entity *entity);

/* enemy/fire_genie.c */
void FireGenie_Init(Entity *entity);

/* enemy/fire_lord.c */
void FireLord_DrawAfterimages(Entity *entity, s32 list);
void FireLord_Init(Entity *entity);

/* enemy/forest_golem.c */
void ForestGolem_Init(Entity *entity);

/* enemy/giant.c */
void Giant_Init(Entity *entity);

/* enemy/giant_beetle.c */
void GiantBeetle_Init(Entity *entity);

/* enemy/giant_leader.c */
void GiantLeader_Init(Entity *entity);

/* enemy/giant_spider.c */
void GiantSpider_Init(Entity *entity);

/* enemy/golden_statue.c */
void GoldenStatue_Init(Entity *entity);

/* enemy/greater_demon.c */
void GreaterDemon_Init(Entity *entity);

/* enemy/hammer_skeleton.c */
void HammerSkeleton_Init(Entity *entity);

/* enemy/high_skeleton.c */
void HighSkeleton_Init(Entity *entity);

/* enemy/ice_golem.c */
void IceGolem_Init(Entity *entity);

/* enemy/larva.c */
void Larva_Init(Entity *entity);

/* enemy/lava_bird.c */
void LavaBird_Init(Entity *entity);

/* enemy/lava_face.c */
void LavaFace_Init(Entity *entity);

/* enemy/lava_lizard.c */
void LavaLizard_Init(Entity *entity);

/* enemy/living_armor.c */
void LivingArmor_Init(Entity *entity);

/* enemy/living_dead.c */
void LivingDead_Init(Entity *entity);

/* enemy/living_statue.c */
void LivingStatue_Init(Entity *entity);

/* enemy/makara.c */
void Makara_Init(Entity *entity);

/* enemy/malformed_giant.c */
void MalformedGiant_Init(Entity *entity);

/* enemy/malice.c */
void Malice_Init(Entity *entity);

/* enemy/master_fadin.c */
void MasterFadin_Init(Entity *entity);

/* enemy/minion.c */
void Minion_Init(Entity *entity);

/* enemy/mummy.c */
void Mummy_Init(Entity *entity);

/* enemy/mutant_ceiling.c */
void MutantCeiling_Init(Entity *entity);

/* enemy/mutant_wall.c */
void MutantWall_Init(Entity *entity);

/* enemy/onyx.c */
void Onyx_Init(Entity *entity);

/* enemy/para_stool.c */
void ParaStool_Init(Entity *entity);

/* enemy/phagos.c */
void Phagos_Init(Entity *entity);

/* enemy/reaper.c */
void Reaper_Init(Entity *entity);

/* enemy/salamander.c */
void Salamander_Init(Entity *entity);

/* enemy/scorpion.c */
void Scorpion_Init(Entity *entity);

/* enemy/seafang.c */
void Seafang_Init(Entity *entity);

/* enemy/sentinel.c */
void Sentinel_Init(Entity *entity);

/* enemy/skeleton_archer.c */
void SkeletonArcher_Init(Entity *entity);

/* enemy/skeleton_soldier.c */
void SkeletonSoldier_Init(Entity *entity);

/* enemy/skull.c */
void Skull_Init(Entity *entity);

/* enemy/slasher.c */
void Slasher_Init(Entity *entity);

/* enemy/slime.c */
void Slime_Init(Entity *entity);

/* enemy/snake.c */
void Snake_Init(Entity *entity);

/* enemy/spider.c */
void Spider_Init(Entity *entity);

/* enemy/stone_guardian.c */
void StoneGuardian_Init(Entity *entity);

/* enemy/stool.c */
void Stool_Init(Entity *entity);

/* enemy/watcher.c */
void Watcher_Init(Entity *entity);

/* enemy/water_elemental.c */
void WaterElemental_Init(Entity *entity);

/* enemy/widda.c */
void Widda_Init(Entity *entity);

/* enemy/widda_archer.c */
void WiddaArcher_Init(Entity *entity);

/* enemy/widda_queen.c */
void WiddaQueen_Init(Entity *entity);

/* enemy/widdacona.c */
void Widdacona_Init(Entity *entity);

/* enemy/wind_elemental.c */
void WindElemental_Init(Entity *entity);

/* enemy/zombie.c */
void Zombie_Init(Entity *entity);

/* data still in asm */
extern EntityIdTable stone_guardian_spawn_ids;
extern Qword living_armor_look_limits;
extern char golden_statue_arrived_message[];
extern EntityFrameTable5 ancient_king_near_shot_frames;
extern EntityFrameTable ancient_king_bone_shot_frames;
extern char earth_elemental_unexpected_message[];
extern EntityActionWeights3 dark_sorcerer_action_weights;
extern EntityFrameTable forest_golem_rumble_frames;
extern int current_map_slot;
extern EntitySpawnInfo *map_spawn_infos[2];
extern f32 critter_no_ground_height[];
extern f32 makara_no_floor_height[];
extern const f32 seafang_no_floor_height_think[];
extern const f32 seafang_no_floor_height_lower[];
extern const f32 seafang_no_floor_height_death[];
extern f32 ancient_mummy_no_floor_height[];
extern f32 malice_no_floor_height_think[];
extern f32 malice_no_floor_height_relocate[];
extern f32 dark_succubus_no_floor_height_think[];
extern const f32 dark_succubus_no_floor_height_arc[];
extern f32 greater_demon_no_floor_height_think[];
extern f32 greater_demon_no_floor_height_attack[];
extern const f32 dark_archer_no_floor_height[];
extern f32 dark_sorcerer_no_floor_height[];
extern f32 dark_puppet_no_floor_height[];
extern EntityModel *held_item_model_ptr;
extern u32 frame_counter;  /* the frame counter */

#endif
