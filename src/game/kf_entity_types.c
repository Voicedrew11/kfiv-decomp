/* Per-entity-type handlers and the Fire Lord's model templates. */
#include "kf_entity_ai_handlers.h"
#include "kf_model.h"

/* the set-up function of every entity type that has one of its own (kf_enemy_*.c, kf_npc.c) */
void AcidCrawler_Init(Entity *entity);
void AncientKing_Init(Entity *entity);
void AncientMummy_Init(Entity *entity);
void Aqualizard_Init(Entity *entity);
void Beetle_Init(Entity *entity);
void Bell_Init(Entity *entity);
void BloodBat_Init(Entity *entity);
void Critter_Init(Entity *entity);
void Crow_Init(Entity *entity);
void DarkArcher_Init(Entity *entity);
void DarkBird_Init(Entity *entity);
void DarkFlower_Init(Entity *entity);
void DarkKnight_Init(Entity *entity);
void DarkOne_Init(Entity *entity);
void DarkPuppet_Init(Entity *entity);
void DarkSoldier_Init(Entity *entity);
void DarkSorcerer_Init(Entity *entity);
void DarkSuccubus_Init(Entity *entity);
void DragonFly_Init(Entity *entity);
void EarthElemental_Init(Entity *entity);
void EarthKing_Init(Entity *entity);
void EntityType72_Init(Entity *entity);
void ExpeditionArcher_Init(Entity *entity);
void ExpeditionSoldier_Init(Entity *entity);
void EyePedestal_Init(Entity *entity);
void Fadin_Init(Entity *entity);
void FireElemental_Init(Entity *entity);
void FireGenie_Init(Entity *entity);
void FireLord_Init(Entity *entity);
void ForestGolem_Init(Entity *entity);
void GiantBeetle_Init(Entity *entity);
void GiantLeader_Init(Entity *entity);
void GiantSpider_Init(Entity *entity);
void Giant_Init(Entity *entity);
void GoldenStatue_Init(Entity *entity);
void GreaterDemon_Init(Entity *entity);
void HammerSkeleton_Init(Entity *entity);
void HighSkeleton_Init(Entity *entity);
void IceGolem_Init(Entity *entity);
void Larva_Init(Entity *entity);
void LavaBird_Init(Entity *entity);
void LavaFace_Init(Entity *entity);
void LavaLizard_Init(Entity *entity);
void LivingArmor_Init(Entity *entity);
void LivingDead_Init(Entity *entity);
void LivingStatue_Init(Entity *entity);
void Makara_Init(Entity *entity);
void MalformedGiant_Init(Entity *entity);
void Malice_Init(Entity *entity);
void MasterFadin_Init(Entity *entity);
void Minion_Init(Entity *entity);
void Mummy_Init(Entity *entity);
void MutantCeiling_Init(Entity *entity);
void MutantWall_Init(Entity *entity);
void Npc_Init(Entity *entity);
void Npc_InitType81(Entity *entity);
void Onyx_Init(Entity *entity);
void ParaStool_Init(Entity *entity);
void Phagos_Init(Entity *entity);
void Reaper_Init(Entity *entity);
void Salamander_Init(Entity *entity);
void Scorpion_Init(Entity *entity);
void Seafang_Init(Entity *entity);
void Sentinel_Init(Entity *entity);
void SkeletonArcher_Init(Entity *entity);
void SkeletonSoldier_Init(Entity *entity);
void Skull_Init(Entity *entity);
void Slasher_Init(Entity *entity);
void Slime_Init(Entity *entity);
void Snake_Init(Entity *entity);
void Spider_Init(Entity *entity);
void StoneGuardian_Init(Entity *entity);
void Stool_Init(Entity *entity);
void Watcher_Init(Entity *entity);
void WaterElemental_Init(Entity *entity);
void WiddaArcher_Init(Entity *entity);
void WiddaQueen_Init(Entity *entity);
void Widda_Init(Entity *entity);
void Widdacona_Init(Entity *entity);
void WindElemental_Init(Entity *entity);
void Zombie_Init(Entity *entity);
void Entity_InitInert(Entity *entity);
void Entity_UpdateInert();

/* 0x3E6E40 [type][0] set-up, [type][1] update; types without a row act as type 0 */
s32 entity_type_handlers[0x60][2] = {
    (s32)EntityAi_InitDefaults, (s32)EntityAi_Update, (s32)Slime_Init, (s32)EntityAi_Update,
    (s32)Reaper_Init, (s32)EntityAi_Update, (s32)Spider_Init, (s32)EntityAi_Update,
    (s32)GiantSpider_Init, (s32)EntityAi_Update, (s32)Beetle_Init, (s32)EntityAi_Update,
    (s32)SkeletonSoldier_Init, (s32)EntityAi_Update, (s32)ExpeditionSoldier_Init, (s32)EntityAi_Update,
    (s32)Stool_Init, (s32)EntityAi_Update, (s32)SkeletonArcher_Init, (s32)EntityAi_Update,
    (s32)BloodBat_Init, (s32)EntityAi_Update, (s32)LivingDead_Init, (s32)EntityAi_Update,
    (s32)Crow_Init, (s32)EntityAi_Update, (s32)ExpeditionArcher_Init, (s32)EntityAi_Update,
    (s32)Phagos_Init, (s32)EntityAi_Update, (s32)Npc_Init, (s32)EntityAi_Update,
    (s32)Scorpion_Init, (s32)EntityAi_Update, (s32)DragonFly_Init, (s32)EntityAi_Update,
    (s32)AcidCrawler_Init, (s32)EntityAi_Update, (s32)Widda_Init, (s32)EntityAi_Update,
    (s32)WiddaArcher_Init, (s32)EntityAi_Update, (s32)Snake_Init, (s32)EntityAi_Update,
    (s32)Salamander_Init, (s32)EntityAi_Update, (s32)StoneGuardian_Init, (s32)EntityAi_Update,
    (s32)FireLord_Init, (s32)EntityAi_Update, (s32)Critter_Init, (s32)EntityAi_Update,
    (s32)Slasher_Init, (s32)EntityAi_Update, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Seafang_Init, (s32)EntityAi_Update, (s32)GoldenStatue_Init, (s32)EntityAi_Update,
    (s32)Widdacona_Init, (s32)EntityAi_Update, (s32)EyePedestal_Init, (s32)EntityAi_Update,
    (s32)LavaFace_Init, (s32)EntityAi_Update, (s32)Sentinel_Init, (s32)EntityAi_Update,
    (s32)LavaLizard_Init, (s32)EntityAi_Update, (s32)Giant_Init, (s32)EntityAi_Update,
    (s32)LivingArmor_Init, (s32)EntityAi_Update, (s32)HammerSkeleton_Init, (s32)EntityAi_Update,
    (s32)Bell_Init, (s32)EntityAi_Update, (s32)Mummy_Init, (s32)EntityAi_Update,
    (s32)Zombie_Init, (s32)EntityAi_Update, (s32)MalformedGiant_Init, (s32)EntityAi_Update,
    (s32)AncientMummy_Init, (s32)EntityAi_Update, (s32)ParaStool_Init, (s32)EntityAi_Update,
    (s32)DarkSoldier_Init, (s32)EntityAi_Update, (s32)EarthKing_Init, (s32)EntityAi_Update,
    (s32)WiddaQueen_Init, (s32)EntityAi_Update, (s32)IceGolem_Init, (s32)EntityAi_Update,
    (s32)FireElemental_Init, (s32)EntityAi_Update, (s32)EarthElemental_Init, (s32)EntityAi_Update,
    (s32)WindElemental_Init, (s32)EntityAi_Update, (s32)WaterElemental_Init, (s32)EntityAi_Update,
    (s32)DarkPuppet_Init, (s32)EntityAi_Update, (s32)DarkSorcerer_Init, (s32)EntityAi_Update,
    (s32)DarkSuccubus_Init, (s32)EntityAi_Update, (s32)DarkArcher_Init, (s32)EntityAi_Update,
    (s32)GreaterDemon_Init, (s32)EntityAi_Update, (s32)Aqualizard_Init, (s32)EntityAi_Update,
    (s32)Malice_Init, (s32)EntityAi_Update, (s32)DarkBird_Init, (s32)EntityAi_Update,
    (s32)Minion_Init, (s32)EntityAi_Update, (s32)AncientKing_Init, (s32)EntityAi_Update,
    (s32)HighSkeleton_Init, (s32)EntityAi_Update, (s32)GiantLeader_Init, (s32)EntityAi_Update,
    (s32)DarkKnight_Init, (s32)EntityAi_Update, (s32)Skull_Init, (s32)EntityAi_Update,
    (s32)FireGenie_Init, (s32)EntityAi_Update, (s32)Larva_Init, (s32)EntityAi_Update,
    (s32)LavaBird_Init, (s32)EntityAi_Update, (s32)MutantCeiling_Init, (s32)EntityAi_Update,
    (s32)MutantWall_Init, (s32)EntityAi_Update, (s32)DarkFlower_Init, (s32)EntityAi_Update,
    (s32)EntityType72_Init, (s32)EntityAi_Update, (s32)Watcher_Init, (s32)EntityAi_Update,
    (s32)Onyx_Init, (s32)EntityAi_Update, (s32)DarkOne_Init, (s32)EntityAi_Update,
    (s32)GiantBeetle_Init, (s32)EntityAi_Update, (s32)ForestGolem_Init, (s32)EntityAi_Update,
    (s32)Makara_Init, (s32)EntityAi_Update, (s32)MasterFadin_Init, (s32)EntityAi_Update,
    (s32)Fadin_Init, (s32)EntityAi_Update, (s32)Npc_InitType81, (s32)EntityAi_Update,
    (s32)LivingStatue_Init, (s32)EntityAi_Update, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Entity_InitInert, (s32)Entity_UpdateInert, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Entity_InitInert, (s32)Entity_UpdateInert, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Entity_InitInert, (s32)Entity_UpdateInert, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Entity_InitInert, (s32)Entity_UpdateInert, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Entity_InitInert, (s32)Entity_UpdateInert, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
    (s32)Entity_InitInert, (s32)Entity_UpdateInert, (s32)Entity_InitInert, (s32)Entity_UpdateInert,
};

ModelInstanceBlock fire_lord_model[2] = { 0 }; /* 0x3E7140 the Fire Lord's model, copied for its two draws */
/* the second of the two, which other files name on its own */
asm(".globl fire_lord_older_afterimage\nfire_lord_older_afterimage = fire_lord_model + 0x1C0");  /* 0x3E7300 the older afterimage, the model two frames back */

/* 0x3E74C0 the Fire Lord's model matrix (second row zero in retail) */
ModelMatrix fire_lord_model_matrix = {
    { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f },
      { 0.0f, -5.0f, 0.0f, 1.0f } },
};
