#ifndef LADX_BANK2_BANK2_H
#define LADX_BANK2_BANK2_H

#include "gb.h"
#include <stdbool.h>

extern const int8_t HookshotChainSpeedX[4];
extern const int8_t HookshotChainSpeedY[4];

/**
 * Spawns a chest containing an item at the coordinates of the intersected object. (02:41D0)
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn an entity; if NULL, uses default allocation.
 * @return true if entity was successfully spawned, false if entity table was full.
 */
bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/**
 * Executes the Ocarina item action when Link is on the ground.
 * Updates song countdown timer and plays the selected song audio wave SFX. (02:41FC)
 *
 * @param gb Pointer to Game Boy system state.
 */
void UseOcarina(GBState *gb);

/**
 * Fires the hookshot chain projectile in Link's facing direction. (02:4254)
 *
 * @param gb Pointer to Game Boy system state.
 * @return true if hookshot projectile was successfully spawned, false otherwise.
 */
bool FireHookshot(GBState *gb);

#endif /* LADX_BANK2_BANK2_H */
