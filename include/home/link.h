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


/**
 * Link motion state application & sword / attack animation handler (00:1794).
 */
void ApplyLinkMotionState(GBState *gb,
                          void (*func_002_753a)(GBState *),
                          void (*label_002_5310)(GBState *),
                          void (*label_002_538b)(GBState *));

/**
 * Bank 20 helper callers (00:1819, 00:1828).
 */
void func_1819(GBState *gb, void (*func_020_4ab3)(GBState *));
void func_1828(GBState *gb, void (*func_020_49ba)(GBState *));

/**
 * Link motion handler for map fade-out transition (00:1837).
 */
void LinkMotionMapFadeOutHandler(GBState *gb,
                                 void (*func_002_754f)(GBState *),
                                 void (*func_020_6c4f)(GBState *),
                                 void (*fade_out_music)(GBState *),
                                 void (*label_004_7a5f)(GBState *),
                                 void (*load_minimap)(GBState *));

/**
 * Saves spawn location and indoor room to wSpawnLocationData (00:19C2).
 */
void SetSpawnLocation(GBState *gb, uint16_t warp_data_addr);

/**
 * Resets Link facing direction to 0 (DIRECTION_RIGHT) (00:19DA).
 */
void label_19DA(GBState *gb);

/**
 * Link motion handler for map fade-in transition (00:19DE).
 */
void LinkMotionMapFadeInHandler(GBState *gb,
                                void (*func_002_754f)(GBState *),
                                void (*func_020_6c7a)(GBState *),
                                void (*func_020_563b)(GBState *));

/**
 * Fade-out and fade-in audio/effects helpers (00:1A22, 00:1A39).
 */
void func_1A22(GBState *gb, void (*func_020_6c4f)(GBState *), void (*fade_out_music)(GBState *));
void func_1A39(GBState *gb, void (*func_020_6c7a)(GBState *), void (*func_020_563b)(GBState *));

/**
 * Computes and updates Link's walking animation state from direction, steps, shield, and motion (00:1A50).
 */
void UpdateLinkWalkingAnimation(GBState *gb);


/**
 * Link interactive motion, lifting, pulling, and object interaction handler (00:1F69).
 */
void label_1F69(GBState *gb,
                uint8_t (*get_physics)(GBState *, uint8_t, uint8_t),
                void (*open_dialog_table0)(GBState *, uint8_t),
                void (*open_dialog_table1)(GBState *, uint8_t),
                void (*open_dialog_table2)(GBState *, uint8_t),
                void (*func_014_5900)(GBState *),
                void (*spawn_chest)(GBState *),
                void (*label_002_48b0)(GBState *),
                void (*func_014_50c3)(GBState *),
                void (*reveal_object)(GBState *),
                uint8_t (*spawn_projectile)(GBState *, uint8_t),
                void (*func_003_5795)(GBState *));

/**
 * Trampoline calling label_1F69 and restoring Bank 2 (00:1F61).
 */
void label_1F69_trampoline(GBState *gb,
                           uint8_t (*get_physics)(GBState *, uint8_t, uint8_t),
                           void (*open_dialog_table0)(GBState *, uint8_t),
                           void (*open_dialog_table1)(GBState *, uint8_t),
                           void (*open_dialog_table2)(GBState *, uint8_t),
                           void (*func_014_5900)(GBState *),
                           void (*spawn_chest)(GBState *),
                           void (*label_002_48b0)(GBState *),
                           void (*func_014_50c3)(GBState *),
                           void (*reveal_object)(GBState *),
                           uint8_t (*spawn_projectile)(GBState *, uint8_t),
                           void (*func_003_5795)(GBState *));

/**
 * Spawns lifted object projectile and marks object under entity revealed (00:2165).
 */
void func_2165(GBState *gb,
               void (*reveal_object)(GBState *),
               uint8_t (*spawn_projectile)(GBState *, uint8_t),
               void (*func_003_5795)(GBState *));

/**
 * Trampoline calling Bank 14 RevealObjectUnderObject (00:2178).
 */
void RevealObjectUnderObject_trampoline(GBState *gb, void (*reveal_object)(GBState *));

/**
 * Spawns ENTITY_LIFTABLE_ROCK and initializes lifted entity status (00:2183).
 */
void label_2183(GBState *gb,
                uint8_t (*spawn_projectile)(GBState *, uint8_t),
                void (*func_003_5795)(GBState *));

/**
 * Updates Link's final X and Y coordinates by integrating speed (00:21A8).
 */
void UpdateFinalLinkPosition(GBState *gb);

/**
 * Integrates Link's speed along horizontal (c=0) or vertical (c=1) axis (00:21B6).
 */
void ComputeLinkPosition(GBState *gb, uint8_t direction);

/**
 * Integrates Link's Z velocity into Z position using subpixel accumulator (00:21E1).
 */
void func_21E1(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_LINK_H */
