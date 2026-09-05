#ifndef LADX_HOME_AUDIO_H
#define LADX_HOME_AUDIO_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PlayWrongAnswerJingle (00:0C20)
 * Plays the wrong answer audio jingle by writing JINGLE_WRONG_ANSWER to hJingle.
 */
void PlayWrongAnswerJingle(GBState *gb);

/**
 * AlertSwordMoblins (00:0C50)
 * Sets wSwordMoblinAlertingSoundCounter to 4 when an alerting noise occurs.
 */
void AlertSwordMoblins(GBState *gb);

/**
 * PlayBombExplosionSfx (00:0C4B)
 * Triggers bomb explosion noise sfx by writing NOISE_SFX_EXPLOSION to hNoiseSfx,
 * then falls through to AlertSwordMoblins.
 */
void PlayBombExplosionSfx(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_AUDIO_H */
