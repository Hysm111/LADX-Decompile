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

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_VFX_H */
