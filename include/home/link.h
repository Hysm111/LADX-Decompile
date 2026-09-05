#ifndef LADX_HOME_LINK_H
#define LADX_HOME_LINK_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Apply map fade-out transition with stairs sound effect:
 * Sets hMusicFadeOutTimer to $30, plays NOISE_SFX_STAIRS, and disables Link's movement.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ApplyMapFadeOutTransitionWithNoise(GBState *gb);

/**
 * Apply map fade-out transition without sound effect:
 * Sets hMusicFadeOutTimer to $30 and disables Link's movement.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ApplyMapFadeOutTransition(GBState *gb);

/**
 * Apply map fade-out transition, preserving music if warping indoors with category 1,
 * or fading out with noise otherwise.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ApplyMapFadeOutTransitionWithSound(GBState *gb);

/**
 * Reset spin attack state (wIsUsingSpinAttack = 0, wSwordCharge = 0),
 * then resets Pegasus boots state.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ResetSpinAttack(GBState *gb);

/**
 * Reset Pegasus boots state (wPegasusBootsChargeMeter = 0, wIsRunningWithPegasusBoots = 0).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ResetPegasusBoots(GBState *gb);

/**
 * Copy Link's final position (hLinkFinalPositionX, hLinkFinalPositionY)
 * to current position (hLinkPositionX, hLinkPositionY).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void CopyLinkFinalPositionToPosition(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_LINK_H */
