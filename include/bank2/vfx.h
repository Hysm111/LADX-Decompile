#ifndef LADX_BANK2_VFX_H
#define LADX_BANK2_VFX_H

#include "gb.h"

/**
 * Triggers shallow water splash visual effect and plays water splash jingle. (02:45AD)
 *
 * @param gb Pointer to Game Boy system state.
 */
void shallowWaterVfx(GBState *gb);

/**
 * Creates water splash transient VFX and plays water splash jingle (02:5928).
 *
 * @param gb Pointer to Game Boy system state.
 * @param y Y coordinate for water splash VFX.
 */
void func_002_5928(GBState *gb, uint8_t y);

/* Transient Visual Effects Tables */
extern const uint8_t Data_002_559C[32];
extern const uint8_t Data_002_55BC[32];
extern const uint8_t Data_002_5642[4];
extern const uint8_t Data_002_5708[16];
extern const uint8_t Data_002_5736[16];
extern const uint8_t Data_002_5756[4];
extern const uint8_t Data_002_575A[4];
extern const uint8_t Data_002_57DD[16];
extern const uint8_t Data_002_57FD[16];
extern const uint8_t Data_002_580D[16];
extern const uint8_t Data_002_5867[16];
extern const uint8_t Data_002_5884[32];
extern const uint8_t Data_002_58ED[8];

/**
 * Clears a transient visual effect from wTranscientVfxTypeTable (02:58E6).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void ClearTranscientVfx(GBState *gb, uint8_t slot);

/**
 * Validates transient VFX coordinates; clears VFX if off screen (02:58D0).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void func_002_58D0(GBState *gb, uint8_t slot);

/**
 * Writes one 4-byte sprite entry to target OAM address using relative offsets (02:5854).
 *
 * @param gb Pointer to Game Boy system state.
 * @param src Pointer to pointer of source sprite definition data (advanced by 4 bytes).
 * @param dst_oam_addr Pointer to destination address in OAM buffer (advanced by 4 bytes).
 */
void label_002_5854(GBState *gb, const uint8_t **src, uint16_t *dst_oam_addr);

/**
 * Advances wOAMNextAvailableSlot and wC3C1 ring buffer pointers (02:58F5).
 *
 * @param gb Pointer to Game Boy system state.
 * @param count Number of sprites written (advances buffer by count * 4 bytes).
 */
void label_002_58F5(GBState *gb, uint8_t count);

/**
 * Emits 2 sprites into dynamic OAM buffer and advances slot pointer (02:583A).
 *
 * @param gb Pointer to Game Boy system state.
 * @param table Pointer to sprite definition table.
 * @param offset Byte offset into sprite table.
 */
void label_002_583A(GBState *gb, const uint8_t *table, uint8_t offset);

/**
 * Emits shallow water splash transient VFX sprites (02:5877).
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_5877(GBState *gb);

/**
 * Renders water splash transient visual effect (02:5825).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientWaterSplash(GBState *gb, uint8_t slot);

/**
 * Renders Pegasus splash transient visual effect (02:581D).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientPegasusSplash(GBState *gb, uint8_t slot);

/**
 * Renders poof transient visual effect, spawning chest or stairs on trigger frame (02:58A4).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 * @param spawn_chest Optional callback to spawn chest (02:5ED3).
 * @param spawn_stairs Optional callback to spawn stairs (02:5F5C).
 */
void RenderTranscientPoof(GBState *gb, uint8_t slot, void (*spawn_chest)(GBState *), void (*spawn_stairs)(GBState *));

/**
 * Renders smoke transient visual effect (02:5746).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientSmoke(GBState *gb, uint8_t slot);

/**
 * Renders sword poke transient visual effect (02:57ED).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientSwordPoke(GBState *gb, uint8_t slot);

/**
 * Renders laser beam transient visual effect (02:57B4).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientLaserBeam(GBState *gb, uint8_t slot);

/**
 * Updates position and renders moving sparkle transient visual effect (02:575E).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientMovingSparkle(GBState *gb, uint8_t slot);

/**
 * Renders lava splash transient visual effect (02:560C).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientLavaSplash(GBState *gb, uint8_t slot);

/**
 * Renders Pegasus dust transient visual effect (02:5718).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientPegasusDust(GBState *gb, uint8_t slot);

/**
 * Handles screen rumble, key cavern / dungeon door triggers, and redraws room tiles (02:5646).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientRumble(GBState *gb, uint8_t slot);

/**
 * Renders sword beam transient visual effect (02:55DC).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientSwordBeam(GBState *gb, uint8_t slot);

/**
 * Main dispatcher for transient visual effects rendering engine (02:5567).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 * @param spawn_chest Optional callback to spawn chest (02:5ED3).
 * @param spawn_stairs Optional callback to spawn stairs (02:5F5C).
 */
void RenderTranscientVfx(GBState *gb, uint8_t slot, void (*spawn_chest)(GBState *), void (*spawn_stairs)(GBState *));

/**
 * Default wrapper for RenderTranscientVfx suitable for function pointer (GBState *, uint8_t).
 *
 * @param gb Pointer to Game Boy system state.
 * @param slot Transient VFX slot index (0..15).
 */
void RenderTranscientVfxDefault(GBState *gb, uint8_t slot);

/**
 * Spawns water splash transient VFX at Link position (02:5926).
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_5926(GBState *gb);

#endif /* LADX_BANK2_VFX_H */
