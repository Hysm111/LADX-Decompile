#ifndef LADX_BANK3_ENTITIES_MOBLIN_H
#define LADX_BANK3_ENTITIES_MOBLIN_H

#include "gb.h"

/* Moblin/Roaming Enemy Handlers (03:5827-05:59D6) */
void MoblinEntityHandler(GBState *gb, uint16_t bc);
void AnimateRoamingEnemy(GBState *gb, uint16_t bc);
void RoamingEnemyState0Handler(GBState *gb, uint16_t bc);
void SetEntityVariantForDirection_03(GBState *gb, uint16_t bc);
void SpawnMoblinArrow(GBState *gb, uint16_t bc);
void SpawnOctorokRock(GBState *gb, uint16_t bc);

#endif /* LADX_BANK3_ENTITIES_MOBLIN_H */