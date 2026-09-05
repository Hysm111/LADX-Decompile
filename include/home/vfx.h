#ifndef LADX_HOME_VFX_H
#define LADX_HOME_VFX_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Display a temporary visual effect as a non-interacting sprite.
 * Allocates an available slot from wTranscientVfxTypeTable (searching backwards from 15 to 0),
 * or recycles a slot using round-robin pointer wC5C0 if all slots are active.
 * Corresponds to AddTranscientVfx (00:0CC7) in disassembly.
 *
 * Inputs in GBState:
 *   - hMultiPurpose0: effect X position
 *   - hMultiPurpose1: effect Y position
 *
 * @param gb Pointer to Game Boy hardware state
 * @param vfx_type Visual effect type identifier
 * @return Index of the allocated VFX slot (0 to 15)
 */
uint8_t AddTranscientVfx(GBState *gb, uint8_t vfx_type);

/**
 * Sets up sword poke VFX coordinates by subtracting 8 from wC140 and wC142,
 * storing them in hMultiPurpose0 and hMultiPurpose1, then plays sword poking sound
 * and allocates a TRANSCIENT_VFX_SWORD_POKE transient VFX sprite.
 * Corresponds to label_D07 (00:0D07) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @return Index of the allocated VFX slot (0 to 15)
 */
uint8_t label_D07(GBState *gb);

/**
 * Plays JINGLE_SWORD_POKING ($07) into hJingle and allocates a TRANSCIENT_VFX_SWORD_POKE
 * transient VFX sprite at coordinates (hMultiPurpose0, hMultiPurpose1).
 * Corresponds to label_D15 (00:0D15) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @return Index of the allocated VFX slot (0 to 15)
 */
uint8_t label_D15(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_VFX_H */
