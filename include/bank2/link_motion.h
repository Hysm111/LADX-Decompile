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

/**
 * Sets up Link's OAM buffer when airborne (02:7587-02:75B1).
 * Writes sprite entry for Link at adjusted position when Z position > 0.
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_7587(GBState *gb);

/**
 * Clears wD475 and checks motion state (02:75B2-02:75BC).
 * Returns early if Link is in unsticking motion state.
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_75B2(GBState *gb);

/**
 * Main ground physics handler for Link (02:75BD-02:77E8).
 * Handles collision with various object types: wells, spikes, pits, water, lava, etc.
 *
 * @param gb Pointer to Game Boy system state.
 */
void ApplyLinkGroundPhysics(GBState *gb);

/**
 * Hurts Link when bumping into spikes (02:75F5-02:7634).
 * Makes Link recoil, lose a heart, and become temporarily invincible.
 *
 * @param gb Pointer to Game Boy system state.
 */
void HurtBySpikes(GBState *gb);

/**
 * Second part of ground physics handling (02:7635-02:76BF).
 * Handles tractor device, conveyor, pit, and pit warp physics.
 *
 * @param gb Pointer to Game Boy system state.
 */
void ApplyLinkGroundPhysics_part2(GBState *gb);

/**
 * Ground physics handler during dialog or room transitions (02:76C0-02:786E).
 * Handles raised/lowered objects, lava, deep water, grass, shallow water, switch buttons.
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_76C0(GBState *gb);

/**
 * Default ground physics for solid ground (02:77A2-02:78D7).
 * Resets pit slipping, handles ocean switch blocks, switch buttons, and grass VFX.
 *
 * @param gb Pointer to Game Boy system state.
 */
void ApplyLinkGroundPhysics_Default(GBState *gb);

/**
 * Grass VFX handler (02:787D-02:78D7).
 * Emits grass cutting transient VFX sprites into Link's OAM buffer.
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_787D(GBState *gb);

/**
 * Handles Pegasus boots collision behavior (02:74AD-02:74FB).
 * Called when Link collides with a wall while running with Pegasus boots.
 * Reverses speed, sets airborne state, and triggers screen shake.
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_74AD(GBState *gb);

/**
 * Handles special object interactions: revolving door ($B1, $B2),
 * and objects $C1, $C2, $BB, $BC.
 * Called from collision handling when specific objects are detected (02:7468-02:74AC).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_7468(GBState *gb);

/* Data tables for swimming physics (02:750A-02:750D) */
extern const int8_t Data_002_750A[4];
extern const int8_t Data_002_750E[4];

/* Dialog wrapper functions (02:74FE-02:7507) */
void OpenDialogInTable0AndClearIncrement(GBState *gb, uint8_t dialog_index);
void OpenDialogInTable2AndClearIncrement(GBState *gb, uint8_t dialog_index);

#endif /* LADX_BANK2_LINK_MOTION_H */
