#ifndef LADX_BANK2_LINK_MOTION_H
#define LADX_BANK2_LINK_MOTION_H

#include "gb.h"

extern const int8_t HorizontalIncrementForLinkPosition[32];
extern const int8_t VerticalIncrementForLinkPosition[32];
extern const uint8_t JoypadToLinkDirection[11];
extern const int8_t Data_002_44E7[6];
extern const int8_t Data_002_68B1[3];
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


/**
 * Executes ground motion and gravity integration without air/side-scrolling guards (02:44FA).
 *
 * @param gb Pointer to Game Boy system state.
 * @param apply_ground_physics Optional callback for ground physics handling.
 */
void ApplyLinkGroundMotion_noChecks(GBState *gb, void (*apply_ground_physics)(GBState *));

/**
 * Handles Link getting unstuck from solid room collision geometry (02:4960).
 *
 * @param gb Pointer to Game Boy system state.
 * @param bg_collision_handler Optional callback for background collision handler (02:6E45).
 */
void LinkMotionUnstuckingHandler(GBState *gb, void (*bg_collision_handler)(GBState *));

#endif /* LADX_BANK2_LINK_MOTION_H */
