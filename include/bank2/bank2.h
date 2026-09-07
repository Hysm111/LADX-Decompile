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

/**
 * Executes ground motion and gravity integration without air/side-scrolling guards (02:44FA).
 *
 * @param gb Pointer to Game Boy system state.
 * @param apply_ground_physics Optional callback for ground physics handling.
 */
void ApplyLinkGroundMotion_noChecks(GBState *gb, void (*apply_ground_physics)(GBState *));

/* Bank 2 Walking & Movement Tables */
extern const int8_t HorizontalIncrementForLinkPosition[32];
extern const int8_t VerticalIncrementForLinkPosition[32];
extern const uint8_t LinkAnimationsList_WalkingNoShield[8];
extern const uint8_t LinkAnimationsList_WalkCarryingDefaultShield[8];
extern const uint8_t LinkAnimationsList_WalkUsingDefaultShield[8];
extern const uint8_t LinkAnimationsList_WalkCarryingMirrorShield[8];
extern const uint8_t LinkAnimationsList_WalkUsingMirrorShield[8];
extern const uint8_t LinkAnimationsList_PushingObject[8];
extern const uint8_t LinkAnimationsList_LiftingObject[8];
extern const uint8_t Data_002_4948[8];
extern const uint8_t Data_002_4950[8];
extern const uint8_t LinkAnimationsList_WalkSideScrolling[8];
extern const uint8_t Data_002_49CA[72];
extern const int8_t Data_002_4A12[2];
extern const int8_t Data_002_4A14[2];

/**
 * Handles Link getting unstuck from solid room collision geometry (02:4960).
 *
 * @param gb Pointer to Game Boy system state.
 * @param bg_collision_handler Optional callback for background collision handler (02:6E45).
 */
void LinkMotionUnstuckingHandler(GBState *gb, void (*bg_collision_handler)(GBState *));

/**
 * Link playing ocarina state handler: progresses countdown, note VFX entities, songs, warp (02:4A16).
 *
 * @param gb Pointer to Game Boy system state.
 * @param select_music_track Optional callback to reload soundtrack (00:27DD / 02:4146).
 * @param spawn_new_entity Optional callback to spawn entity (00:3B86).
 */
void LinkPlayingOcarinaHandler(GBState *gb,
                               void (*select_music_track)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t));

/* Bank 2 Shovel Digging & Animation Tables */
extern const uint8_t LinkDirectionToLinkAnimationState2[8];
extern const int8_t LinkDirectionToAdjacentTileIndexX[4];
extern const int8_t LinkDirectionToAdjacentTileIndexY[4];

/**
 * Prepares DMG draw commands for a dug shovel hole tile at the intersected object address (02:4BD4).
 *
 * @param gb Pointer to Game Boy system state.
 * @param hl Destination pointer in wDrawCommand.
 */
void func_002_4BD4(GBState *gb, uint16_t hl);

/**
 * Prepares CGB draw commands for a dug shovel hole tile in both VRAM0 and VRAM1 (02:4C14).
 *
 * @param gb Pointer to Game Boy system state.
 * @param hl Destination pointer in wDrawCommand.
 */
void func_002_4C14(GBState *gb, uint16_t hl);

/**
 * Places a shovel hole in wRoomObjects, backups to RAM2, issues draw command, and rolls random drop (02:4C92).
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn drop entity.
 * @param apply_vector Optional callback to apply velocity vector towards Link.
 */
void label_002_4C92(GBState *gb,
                    uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                    void (*apply_vector)(GBState *));

/**
 * Validates whether the tile in front of Link can be dug with the shovel (02:4D20).
 *
 * @param gb Pointer to Game Boy system state.
 * @return true if tile can be dug, false if blocked or non-diggable.
 */
bool func_002_4D20(GBState *gb);

/**
 * Attempts to dig with the shovel: validates facing tile and places hole / rolls drop (02:4BC8).
 *
 * @param gb Pointer to Game Boy system state.
 * @param spawn_new_entity Optional callback to spawn drop entity.
 * @param apply_vector Optional callback to apply velocity vector towards Link.
 * @return true if digging succeeded, false otherwise.
 */
bool func_002_4BC8(GBState *gb,
                   uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                   void (*apply_vector)(GBState *));

/**
 * Link shovel usage state handler: advances digging animation, triggers hole placement, Marin scolding (02:4B49).
 *
 * @param gb Pointer to Game Boy system state.
 * @param func_020_4b4a Optional callback for Wind Fish Egg Nightmare shovel interaction.
 * @param shovel_dig_action Optional callback for shovel digging action.
 */
void func_002_4B49(GBState *gb,
                   void (*func_020_4b4a)(GBState *),
                   bool (*shovel_dig_action)(GBState *));

/* Bank 2 Revolving Door & Tile/Palette Data Tables */
extern const uint8_t Data_002_4E1C[16];
extern const uint8_t LinkRevolvingDoorAnimation[7];

/**
 * Replaces a room object tile with 0xAE, writes GBC attributes, and emits 10-byte draw command (02:4D97).
 *
 * @param gb Pointer to Game Boy system state.
 * @param get_bg_attr_addr Optional callback for GBC attribute lookup.
 */
void label_002_4D97(GBState *gb, void (*get_bg_attr_addr)(GBState *));

/**
 * Copies 8 bytes of object palette 1 from WRAM bank 1 to WRAM bank 2 (02:4DFC).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4DFC(GBState *gb);

/**
 * Copies 8 bytes from Data_002_4E1C + de into wObjPal8 and flags palette update (02:4E2C).
 *
 * @param gb Pointer to Game Boy system state.
 * @param de Byte offset into Data_002_4E1C (0 or 8).
 */
void func_002_4E2C(GBState *gb, uint16_t de);

/**
 * Restores 8 bytes of wObjPal8 from WRAM bank 2 to WRAM bank 1 and flags palette update (02:4E48).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4E48(GBState *gb);

/**
 * Resets revolving door animation and state, returning Link to default motion (02:4EDD).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4EDD(GBState *gb);

/**
 * Handles Link moving through the revolving door in Eagle's Tower (02:4E6D).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LinkMotionRevolvingDoorHandler(GBState *gb);

/* Bank 2 Swimming Velocity Tables */
extern const int8_t Data_002_4EF0[16];
extern const int8_t Data_002_4F00[16];
extern const int8_t Data_002_4F10[16];
extern const int8_t Data_002_4F20[16];

/**
 * Creates water splash transient VFX and plays water splash jingle (02:5928).
 *
 * @param gb Pointer to Game Boy system state.
 * @param y Y coordinate for water splash VFX.
 */
void func_002_5928(GBState *gb, uint8_t y);

/**
 * Handles Link's swimming and diving physics, movement, B diving toggle, and sunken items (02:4F30).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_map_transition Optional callback for map transition check.
 * @param func_753a Optional callback for surface offset and shadow physics (02:753A).
 * @param spawn_new_entity Optional callback to spawn entity (00:3B86).
 * @param splash_vfx Optional callback to create splash VFX (02:5928).
 */
void LinkMotionSwimmingHandler(GBState *gb,
                               void (*check_map_transition)(GBState *),
                               void (*func_753a)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t),
                               void (*splash_vfx)(GBState *, uint8_t));

/**
 * Handles Link's unknown / hole falling motion state 0x0F (02:50A3).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LinkMotionUnknownHandler(GBState *gb);

#endif /* LADX_BANK2_BANK2_H */
