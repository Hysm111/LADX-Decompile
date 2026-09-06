#include "home/audio.h"
#include "home/bank.h"
#include "constants/memory.h"
#include "constants/sfx.h"

void PlayWrongAnswerJingle(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hJingle, JINGLE_WRONG_ANSWER);
}

void AlertSwordMoblins(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wSwordMoblinAlertingSoundCounter, 0x04);
}

void PlayBombExplosionSfx(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hNoiseSfx, NOISE_SFX_EXPLOSION);
    /* In original assembly, falls through into AlertSwordMoblins */
    AlertSwordMoblins(gb);
}

static void doAudioStep(GBState *gb, AudioStepCallback play_music_1b, AudioStepCallback play_music_1e) {
    SwitchBank(gb, 0x1B);
    if (play_music_1b) {
        play_music_1b(gb);
    }
    SwitchBank(gb, 0x1E);
    if (play_music_1e) {
        play_music_1e(gb);
    }
}

void PlayAudioStepWithHooks(GBState *gb,
                            AudioStepCallback play_sfx,
                            AudioStepCallback play_music_1b,
                            AudioStepCallback play_music_1e) {
    if (!gb) return;

    /* callsw PlaySfx -> switches to bank 0x1F and executes PlaySfx */
    SwitchBank(gb, 0x1F);
    if (play_sfx) {
        play_sfx(gb);
    }

    /* If a wave SFX is playing, return early */
    if (gb_read(gb, hWaveSfx) != 0) {
        return;
    }

    uint8_t timing = gb_read(gb, wMusicTrackTiming);
    if (timing == 0) {
        doAudioStep(gb, play_music_1b, play_music_1e);
        return;
    }

    if (timing == 2) {
        uint8_t frame = gb_read(gb, hFrameCounter);
        if ((frame & 1) != 0) {
            return;
        }
        doAudioStep(gb, play_music_1b, play_music_1e);
        return;
    }

    /* Otherwise (e.g. timing == 1), play two audio steps (double speed) */
    doAudioStep(gb, play_music_1b, play_music_1e);
    doAudioStep(gb, play_music_1b, play_music_1e);
}

void PlayAudioStep(GBState *gb) {
    PlayAudioStepWithHooks(gb, NULL, NULL, NULL);
}
