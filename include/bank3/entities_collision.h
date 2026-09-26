#ifndef LADX_BANK3_ENTITIES_COLLISION_H
#define LADX_BANK3_ENTITIES_COLLISION_H

#include "gb.h"

/* Collision and Damage Handlers (03:6C72-03:7267) */
bool CheckLinkCollisionWithEnemy(GBState *gb, uint16_t bc);
void ApplyLinkCollisionWithEnemy(GBState *gb, uint16_t bc);
void DefaultEnemyDamageCollisionHandler(GBState *gb, uint16_t bc);
void ApplySwordDamagesToEnemy(GBState *gb, uint16_t bc);

/* func_003_6B7B is declared in entities_physics.h */
void func_003_6B7B(GBState *gb, uint16_t bc);

#endif /* LADX_BANK3_ENTITIES_COLLISION_H */