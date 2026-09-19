#ifndef LADX_BANK2_ROOM_TRANSITION_H
#define LADX_BANK2_ROOM_TRANSITION_H

#include "gb.h"

/* Bank 2 Debug Warp Tables (02:5471+) */
extern const uint8_t DebugWarpRooms[11];
extern const uint8_t DebugWarpMaps[11];

/**
 * Triggers debug warp sequence by incrementing debug warp index and fading out (02:54AE).
 *
 * @param gb Pointer to Game Boy system state.
 */
void ExecuteDebugWarp(GBState *gb);

/**
 * Checks Link proximity, jump state, lifted object, and color dungeon conditions to trigger staircase warp (02:552A).
 *
 * @param gb Pointer to Game Boy system state.
 */
void staircaseIsActive(GBState *gb);

/**
 * Iterates through transient VFX slots (15 down to 0) and processes active/inactive staircase state (02:54E4).
 *
 * @param gb Pointer to Game Boy system state.
 * @param render_transient_vfx Optional callback to render transient VFX in slot.
 */
void renderTranscientVFXs(GBState *gb, void (*render_transient_vfx)(GBState *, uint8_t));

/**
 * Clears indoor room status, decrements dialog/photo album cooldowns, and updates VFX/staircase (02:5487).
 *
 * @param gb Pointer to Game Boy system state.
 * @param render_transient_vfx Optional callback to render transient VFX in slot.
 */
void label_002_5487(GBState *gb, void (*render_transient_vfx)(GBState *, uint8_t));

/**
 * Loads the minimap for the current dungeon (02:6709).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LoadMinimap(GBState *gb);

#endif /* LADX_BANK2_ROOM_TRANSITION_H */
