#ifndef LADX_HOME_AUDIO_H
#define LADX_HOME_AUDIO_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*AudioStepCallback)(GBState *gb);

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

/**
 * Executes an audio step: calls PlaySfx in bank $1F, then if no wave SFX is playing,
 * executes music tracks from bank $1B and $1E depending on wMusicTrackTiming and frame counter.
 * Corresponds to PlayAudioStep (00:08A4) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PlayAudioStep(GBState *gb);

/**
 * Hooked version of PlayAudioStep allowing custom callbacks for the sub-bank audio routines.
 */
void PlayAudioStepWithHooks(GBState *gb,
                            AudioStepCallback play_sfx,
                            AudioStepCallback play_music_1b,
                            AudioStepCallback play_music_1e);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_AUDIO_H */
