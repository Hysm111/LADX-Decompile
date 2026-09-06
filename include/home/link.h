#ifndef LADX_HOME_LINK_H
#define LADX_HOME_LINK_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Plays stairs noise SFX (NOISE_SFX_STAIRS) and disables Link's movement.
 * Corresponds to playNoiseStairs (00:0C9A) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void playNoiseStairs(GBState *gb);

/**
 * Disables Link's movement (sets wLinkMotionState = LINK_MOTION_MAP_FADE_OUT)
 * and resets transition sequence variables (wTransitionSequenceCounter, wC16C, wD478).
 * Corresponds to disableMovementInTransition (00:0C9E) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void disableMovementInTransition(GBState *gb);

/**
 * Apply map fade-out transition with stairs sound effect:
 * Sets hMusicFadeOutTimer to $30, plays NOISE_SFX_STAIRS, and disables Link's movement.
 * Corresponds to ApplyMapFadeOutTransitionWithNoise (00:0C7D) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ApplyMapFadeOutTransitionWithNoise(GBState *gb);

/**
 * Apply map fade-out transition without sound:
 * Sets hMusicFadeOutTimer to $30 and disables Link's movement.
 * Corresponds to ApplyMapFadeOutTransition (00:0C83) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ApplyMapFadeOutTransition(GBState *gb);

/**
 * Apply map fade-out transition with sound logic:
 * Checks wWarp0MapCategory == 1 and wIsIndoor != 0; if so, sets
 * hContinueMusicAfterWarp to 1 and plays stairs sound effect without fading music,
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

/**
 * Trampoline to UpdateLinkWalkingAnimation:
 * Selects BANK(LinkAnimationsLists) (bank 2), executes the animation update callback,
 * and calls ReloadSavedBank to restore wCurrentBank into rSelectROMBank.
 * Corresponds to UpdateLinkWalkingAnimation_trampoline (00:0BF0) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param update_func Target callback to execute (can be NULL)
 */
void UpdateLinkWalkingAnimation_trampoline(GBState *gb, void (*update_func)(GBState *));


/**
 * Check sword collision with static elements and objects, then return to bank 2 (00:15A7).
 */
void CheckStaticSwordCollision_trampoline(GBState *gb,
                                          uint8_t (*get_object_physics)(GBState *, uint8_t, uint8_t),
                                          void (*reveal_object)(GBState *),
                                          void (*start_smashing_rock)(GBState *, uint8_t),
                                          void (*alert_sword_moblins)(GBState *));

/**
 * Check sword collision with static elements (bushes, grasses) and floor objects (00:15AF).
 */
void CheckStaticSwordCollision(GBState *gb,
                               uint8_t (*get_object_physics)(GBState *, uint8_t, uint8_t),
                               void (*reveal_object)(GBState *),
                               void (*start_smashing_rock)(GBState *, uint8_t),
                               void (*alert_sword_moblins)(GBState *));

/**
 * Check sword collision with items lying on the ground (00:16C2).
 */
void CheckItemsSwordCollision(GBState *gb, uint8_t physics_flags, void (*alert_sword_moblins)(GBState *));

/**
 * Pegasus boots running action handler (00:1705).
 */
void UsePegasusBoots(GBState *gb);

/**
 * Display transient VFX (dust or water splash) when running with Pegasus boots (00:1756).
 */
void DisplayTransientVfxForLinkRunning(GBState *gb);

/**
 * Reset Link's speed increment (hLinkSpeedX = 0, hLinkSpeedY = 0) (00:178E).
 */
void ClearLinkPositionIncrement(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_LINK_H */
