#ifndef LADX_BANK2_BANK2_H
#define LADX_BANK2_BANK2_H

#include "gb.h"
#include <stdbool.h>

extern const int8_t HookshotChainSpeedX[4];
extern const int8_t HookshotChainSpeedY[4];
extern const uint8_t DirectionToLinkAnimationState[4];
extern const int8_t HorizontalIncrementForLinkPosition[32];
extern const int8_t VerticalIncrementForLinkPosition[32];
extern const uint8_t JoypadToLinkDirection[11];

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

/**
 * If Link is carrying a lifted object (wIsCarryingLiftedObject >= 2), sets his animation state
 * and blocks interactive motion. (02:4338)
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4338(GBState *gb);

/**
 * Decrements attack step animation countdown and updates Link animation state from facing direction. (02:434A)
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_434A(GBState *gb);

/**
 * Sets Link's horizontal and vertical speeds directly from joypad directional button mask. (02:437A)
 *
 * @param gb Pointer to Game Boy system state.
 * @param offset Table offset: 0 for normal, 0x10 for Piece of Power.
 */
void MoveLinkToPressedButtonDirection(GBState *gb, uint8_t offset);

/**
 * Smoothly nudges/accelerates Link's speed towards target joypad directional speed. (02:438F)
 *
 * @param gb Pointer to Game Boy system state.
 * @param offset Table offset: 0 for normal, 0x10 for Piece of Power.
 */
void func_002_438F(GBState *gb, uint8_t offset);

#endif /* LADX_BANK2_BANK2_H */
