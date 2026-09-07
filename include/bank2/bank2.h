#ifndef LADX_BANK2_BANK2_H
#define LADX_BANK2_BANK2_H

#include "gb.h"

extern const int8_t HookshotChainSpeedX[4];
extern const int8_t HookshotChainSpeedY[4];
extern const uint8_t DirectionToLinkAnimationState[4];
extern const int8_t HorizontalIncrementForLinkPosition[32];
extern const int8_t VerticalIncrementForLinkPosition[32];
extern const uint8_t JoypadToLinkDirection[11];
extern const int8_t Data_002_44E7[6];
extern const int8_t Data_002_68B1[3];

/**
 * Spawns a chest containing an item at the coordinates of the intersected object. (02:41D0)
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn an entity. If NULL, standard slot search is used.
 * @return true if chest was spawned, false if no free entity slot.
 */
bool SpawnChestWithItem(GBState *gb, uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/**
 * Plays the currently selected Ocarina song or off-key notes if no songs learned. (02:41FC)
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

/**
 * Triggers shallow water splash visual effect and plays water splash jingle. (02:45AD)
 *
 * @param gb Pointer to Game Boy system state.
 */
void shallowWaterVfx(GBState *gb);

/**
 * If on the ground, updates Link speed from gravity and joypad input, and handles ground landing. (02:44ED)
 *
 * @param gb Pointer to Game Boy system state.
 * @param apply_ground_physics Optional callback for ground physics handling on landing.
 */
void ApplyLinkGroundMotion(GBState *gb, void (*apply_ground_physics)(GBState *));

/**
 * Copies wLinkGroundStatus to wC130, clears wLinkGroundStatus, and checks map transition. (02:44B5)
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_map_transition Optional callback to check position for map transition.
 */
void label_002_44B5(GBState *gb, void (*check_map_transition)(GBState *));

/**
 * If inventory is not appearing, updates final Link position and checks map transition. (02:44AD)
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_map_transition Optional callback to check position for map transition.
 */
void func_002_44AD(GBState *gb, void (*check_map_transition)(GBState *));

/**
 * Decrements ignore collisions countdown, updates position, checks transition, and handles collision stop. (02:44C2)
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_map_transition Optional callback to check position for map transition.
 * @return true if collision countdown was active (caller should pop return and jump to ApplyLinkMotionState), false otherwise.
 */
bool func_002_44C2(GBState *gb, void (*check_map_transition)(GBState *));

/**
 * Overhead walk physics and Link collision/movement processing. (02:43BA)
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_map_transition Optional callback for map boundary transition checks.
 */
void OverheadWalkPhysics(GBState *gb, void (*check_map_transition)(GBState *));

/**
 * Dispatches between overhead walk physics and side-scrolling physics. (02:436C)
 *
 * @param gb Pointer to Game Boy system state.
 * @param side_scrolling_physics Optional callback for side-scrolling physics.
 * @param check_map_transition Optional callback for map boundary transition checks.
 */
void func_002_436C(GBState *gb,
                   void (*side_scrolling_physics)(GBState *),
                   void (*check_map_transition)(GBState *));

/**
 * Main default Link motion handler (physics, animations, collisions, sword charging). (02:4287)
 *
 * @param gb Pointer to Game Boy system state.
 * @param side_scrolling_physics Optional callback for side-scrolling physics.
 * @param check_map_transition Optional callback for map transition checks.
 * @param ocarina_handler Optional callback for LinkPlayingOcarinaHandler.
 * @param func_002_753a Optional callback for func_002_753a.
 * @param update_link_animation Optional callback for UpdateLinkAnimation.
 * @param func_002_4b49 Optional callback for func_002_4b49.
 * @param apply_ground_physics Optional callback for ground physics handling.
 */
void LinkMotionDefault(GBState *gb,
                       void (*side_scrolling_physics)(GBState *),
                       void (*check_map_transition)(GBState *),
                       void (*ocarina_handler)(GBState *),
                       void (*func_002_753a)(GBState *),
                       void (*update_link_animation)(GBState *),
                       void (*func_002_4b49)(GBState *),
                       void (*apply_ground_physics)(GBState *));


/* Bank 2 Sword & Spin Attack Tables */
extern const uint8_t LinkDirectionToStaticSwordCollitionCheckNeeded[24];
extern const uint8_t LinkDirectionTo_wC141[24];
extern const uint8_t LinkDirectionToOffset[24];
extern const uint8_t LinkDirectionTo_wC143[24];
extern const uint8_t LinkDirectionToSwordDirection[24];
extern const uint8_t LinkDirectionToLinkAnimationState1[24];
extern const int8_t LinkDirectionTo_wC13A[24];
extern const int8_t LinkDirectionTo_wC139[24];
extern const int8_t LinkDirectionTo_wC13C[24];
extern const int8_t LinkDirectionTo_wC13B[24];
extern const uint8_t SwordAnimationStateToUnknow[8];
extern const uint8_t UnkownToLinkStateTable[16];
extern const uint8_t FrameCounterToLinkDirection[4];
extern const uint8_t LinkDirectionToSwordAnimationState[32];
extern const uint8_t LinkDirectionToAbsolute[32];

/**
 * Resets sword animation state and spin attack variables (02:48B0).
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_48B0(GBState *gb);

/**
 * Updates sword direction, link animation state, and computes sword collision box (02:4827).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void label_002_4827(GBState *gb, void (*check_collision)(GBState *));

/**
 * Progresses sword swing animation from wC16D timer (02:476B).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void label_002_476B(GBState *gb, void (*check_collision)(GBState *));

/**
 * Updates spin attack 360-degree rotation animation and collision box (02:4709).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void UpdateSpinAttackAnimation(GBState *gb, void (*check_collision)(GBState *));

/**
 * Updates Link automatic animations: whirlpool rotation, jump frames, sword/spin attack (02:478C).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void UpdateLinkAnimation(GBState *gb, void (*check_collision)(GBState *));

#endif /* LADX_BANK2_BANK2_H */
