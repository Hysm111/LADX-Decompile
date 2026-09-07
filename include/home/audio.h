#ifndef LADX_HOME_AUDIO_H
#define LADX_HOME_AUDIO_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PlayWrongAnswerJingle (00:0C20)
 * Plays the wrong answer error sound effect by writing JINGLE_WRONG_ANSWER to hJingle.
 */
void PlayWrongAnswerJingle(GBState *gb);

/**
 * AlertSwordMoblins (00:0C50)
 * Sets wSwordMoblinAlertingSoundCounter to 0x04.
 */
void AlertSwordMoblins(GBState *gb);

/**
 * PlayBombExplosionSfx (00:0C4B)
 * Triggers bomb explosion noise sfx by writing NOISE_SFX_EXPLOSION to hNoiseSfx,
 * then falls through to AlertSwordMoblins.
 */
void PlayBombExplosionSfx(GBState *gb);

/**
 * PlayAudioStep (00:08A4)
 * Audio step dispatcher executed once per frame.
 * Calls PlaySfx in bank $1F, checks if hWaveSfx is active,
 * evaluates wMusicTrackTiming, and steps music handlers in banks $1B and $1E.
 */
void PlayAudioStep(GBState *gb);

typedef void (*AudioStepCallback)(GBState *gb);

void PlayAudioStepWithHooks(GBState *gb,
                            AudioStepCallback play_sfx,
                            AudioStepCallback play_music_1b,
                            AudioStepCallback play_music_1e);

/**
 * SetWorldMusicTrack (00:27C3)
 * Sets the music track to play on the overworld / map.
 * Stores track to wMusicTrackToPlay and hNextDefaultMusicTrack,
 * sets hMusicFadeInTimer to 0x38, and zeroes hMusicFadeOutTimer.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param track_id Soundtrack ID to play
 */
void SetWorldMusicTrack(GBState *gb, uint8_t track_id);

/**
 * SelectMusicTrackAfterTransition_trampoline (00:27DD)
 * Switches to BANK(SelectMusicTrackAfterTransition) ($02), calls
 * SelectMusicTrackAfterTransition callback, and restores saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param select_music Callback to SelectMusicTrackAfterTransition
 */
void SelectMusicTrackAfterTransition_trampoline(GBState *gb, void (*select_music)(GBState *));

/**
 * ResetMusicFadeTimer (00:27EA)
 * Sets fade-out timer to max (MUSIC_FADE_OUT_TIMER_MAX, 0x38)
 * and clears fade-in timer (hMusicFadeInTimer = 0).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ResetMusicFadeTimer(GBState *gb);

/**
 * func_27F2 (00:27F2)
 * If hContinueMusicAfterWarp is 0, switches to bank $1F and calls func_01F_4003.
 * In all cases, reloads saved bank via ReloadSavedBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param func_01F_4003 Callback to routine at 01F:4003
 */
void func_27F2(GBState *gb, void (*func_01F_4003)(GBState *));

/**
 * PlayBoomerangSfx_trampoline (00:29F8)
 * Switches to bank $20 (BANK(PlayBoomerangSfx)), calls PlayBoomerangSfx,
 * and restores rSelectROMBank from wCurrentBank.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param play_sfx Callback to PlayBoomerangSfx
 */
void PlayBoomerangSfx_trampoline(GBState *gb, void (*play_sfx)(GBState *));

#ifdef __cplusplus
}
#endif

void label_27F2(GBState *gb);

#endif /* LADX_HOME_AUDIO_H */
