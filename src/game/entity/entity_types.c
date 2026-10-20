/* Per-entity-type handlers and the Fire Lord's model templates. */
#include "entity/entity_ai_handlers.h"
#include "model/model.h"
#include "enemy/enemy.h"
#include "entity/entity.h"
#include "entity/entity_world.h"
#include "entity/npc.h"

/* 0x3E6E40 [type][0] set-up, [type][1] update; types without a row act as type 0 */
EntityHandler entity_type_handlers[0x60][2] = {
    EntityAi_InitDefaults, EntityAi_Update, Slime_Init, EntityAi_Update,
    Reaper_Init, EntityAi_Update, Spider_Init, EntityAi_Update,
    GiantSpider_Init, EntityAi_Update, Beetle_Init, EntityAi_Update,
    SkeletonSoldier_Init, EntityAi_Update, ExpeditionSoldier_Init, EntityAi_Update,
    Stool_Init, EntityAi_Update, SkeletonArcher_Init, EntityAi_Update,
    BloodBat_Init, EntityAi_Update, LivingDead_Init, EntityAi_Update,
    Crow_Init, EntityAi_Update, ExpeditionArcher_Init, EntityAi_Update,
    Phagos_Init, EntityAi_Update, Npc_Init, EntityAi_Update,
    Scorpion_Init, EntityAi_Update, DragonFly_Init, EntityAi_Update,
    AcidCrawler_Init, EntityAi_Update, Widda_Init, EntityAi_Update,
    WiddaArcher_Init, EntityAi_Update, Snake_Init, EntityAi_Update,
    Salamander_Init, EntityAi_Update, StoneGuardian_Init, EntityAi_Update,
    FireLord_Init, EntityAi_Update, Critter_Init, EntityAi_Update,
    Slasher_Init, EntityAi_Update, Entity_InitInert, Entity_UpdateInert,
    Seafang_Init, EntityAi_Update, GoldenStatue_Init, EntityAi_Update,
    Widdacona_Init, EntityAi_Update, EyePedestal_Init, EntityAi_Update,
    LavaFace_Init, EntityAi_Update, Sentinel_Init, EntityAi_Update,
    LavaLizard_Init, EntityAi_Update, Giant_Init, EntityAi_Update,
    LivingArmor_Init, EntityAi_Update, HammerSkeleton_Init, EntityAi_Update,
    Bell_Init, EntityAi_Update, Mummy_Init, EntityAi_Update,
    Zombie_Init, EntityAi_Update, MalformedGiant_Init, EntityAi_Update,
    AncientMummy_Init, EntityAi_Update, ParaStool_Init, EntityAi_Update,
    DarkSoldier_Init, EntityAi_Update, EarthKing_Init, EntityAi_Update,
    WiddaQueen_Init, EntityAi_Update, IceGolem_Init, EntityAi_Update,
    FireElemental_Init, EntityAi_Update, EarthElemental_Init, EntityAi_Update,
    WindElemental_Init, EntityAi_Update, WaterElemental_Init, EntityAi_Update,
    DarkPuppet_Init, EntityAi_Update, DarkSorcerer_Init, EntityAi_Update,
    DarkSuccubus_Init, EntityAi_Update, DarkArcher_Init, EntityAi_Update,
    GreaterDemon_Init, EntityAi_Update, Aqualizard_Init, EntityAi_Update,
    Malice_Init, EntityAi_Update, DarkBird_Init, EntityAi_Update,
    Minion_Init, EntityAi_Update, AncientKing_Init, EntityAi_Update,
    HighSkeleton_Init, EntityAi_Update, GiantLeader_Init, EntityAi_Update,
    DarkKnight_Init, EntityAi_Update, Skull_Init, EntityAi_Update,
    FireGenie_Init, EntityAi_Update, Larva_Init, EntityAi_Update,
    LavaBird_Init, EntityAi_Update, MutantCeiling_Init, EntityAi_Update,
    MutantWall_Init, EntityAi_Update, DarkFlower_Init, EntityAi_Update,
    EntityType72_Init, EntityAi_Update, Watcher_Init, EntityAi_Update,
    Onyx_Init, EntityAi_Update, DarkOne_Init, EntityAi_Update,
    GiantBeetle_Init, EntityAi_Update, ForestGolem_Init, EntityAi_Update,
    Makara_Init, EntityAi_Update, MasterFadin_Init, EntityAi_Update,
    Fadin_Init, EntityAi_Update, Npc_InitType81, EntityAi_Update,
    LivingStatue_Init, EntityAi_Update, Entity_InitInert, Entity_UpdateInert,
    Entity_InitInert, Entity_UpdateInert, Entity_InitInert, Entity_UpdateInert,
    Entity_InitInert, Entity_UpdateInert, Entity_InitInert, Entity_UpdateInert,
    Entity_InitInert, Entity_UpdateInert, Entity_InitInert, Entity_UpdateInert,
    Entity_InitInert, Entity_UpdateInert, Entity_InitInert, Entity_UpdateInert,
    Entity_InitInert, Entity_UpdateInert, Entity_InitInert, Entity_UpdateInert,
    Entity_InitInert, Entity_UpdateInert, Entity_InitInert, Entity_UpdateInert,
};

ModelInstanceBlock fire_lord_model[2] = { 0 }; /* 0x3E7140 the Fire Lord's model, copied for its two draws */
/* the second of the two, which other files name on its own */
asm(".globl fire_lord_older_afterimage\nfire_lord_older_afterimage = fire_lord_model + 0x1C0");  /* 0x3E7300 the older afterimage, the model two frames back */

/* 0x3E74C0 the Fire Lord's model matrix (second row zero in retail) */
ModelMatrix fire_lord_model_matrix = {
    { { 1.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 0.0f },
      { 0.0f, -5.0f, 0.0f, 1.0f } },
};
