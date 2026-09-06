#include "home/audio.h"
#include "home/bank.h"
#include "constants/audio.h"
#include "constants/hardware.h"
#include "constants/memory.h"

#include "constants/sfx.h"
// #define JINGLE_WRONG_ANSWER  0x03
// #define NOISE_SFX_EXPLOSION  0x04

void PlayWrongAnswerJingle(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hJingle, JINGLE_WRONG_ANSWER);
}

void AlertSwordMoblins(GBState *gb) {
    if (!gb) return;
    gb_write(gb, wSwordMoblinAlertingSoundCounter, 4);
}

void PlayBombExplosionSfx(GBState *gb) {
    if (!gb) return;
    gb_write(gb, hNoiseSfx, NOISE_SFX_EXPLOSION);
    AlertSwordMoblins(gb);
}

static void doAudioStep(GBState *gb, AudioStepCallback play_music_1b, AudioStepCallback play_music_1e) {
    /* 1. BANK($1B) - Music track handler */
    SwitchBank(gb, 0x1B);
    if (play_music_1b) {
        play_music_1b(gb);
    }

    /* 2. BANK($1E) - Secondary music track handler */
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

    /* Always play sound effects step in bank $1F */
    SwitchBank(gb, 0x1F);
    if (play_sfx) {
        play_sfx(gb);
    }

    /* If a wave SFX is playing (hWaveSfx != 0), do not execute music tracks */
    if (gb_read(gb, hWaveSfx) != 0) {
        return;
    }

    uint8_t timing = gb_read(gb, wMusicTrackTiming);

    /* If timing == 0: standard speed, 1 step */
    if (timing == 0) {
        doAudioStep(gb, play_music_1b, play_music_1e);
        return;
    }

    /* If timing == 2: half speed (play only every other frame) */
    if (timing == 2) {
        uint8_t frame = gb_read(gb, hFrameCounter);
        if ((frame & 1) != 0) {
            return;
        }
        doAudioStep(gb, play_music_1b, play_music_1e);
        return;
    }

    /* If timing == 1: double speed, 2 steps */
    doAudioStep(gb, play_music_1b, play_music_1e);
    doAudioStep(gb, play_music_1b, play_music_1e);
}

void PlayAudioStep(GBState *gb) {
    PlayAudioStepWithHooks(gb, NULL, NULL, NULL);
}

void SetWorldMusicTrack(GBState *gb, uint8_t track_id) {
    if (!gb) return;

    gb_write(gb, wMusicTrackToPlay, track_id);
    gb_write(gb, hNextDefaultMusicTrack, track_id);
    gb_write(gb, hMusicFadeInTimer, 0x38);
    gb_write(gb, hMusicFadeOutTimer, 0x00);
}

void SelectMusicTrackAfterTransition_trampoline(GBState *gb, void (*select_music)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x02);
    if (select_music) {
        select_music(gb);
    }
    ReloadSavedBank(gb);
}

void ResetMusicFadeTimer(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hMusicFadeOutTimer, MUSIC_FADE_OUT_TIMER_MAX);
    gb_write(gb, hMusicFadeInTimer, 0x00);
}

void func_27F2(GBState *gb, void (*func_01F_4003)(GBState *)) {
    if (!gb) return;

    if (gb_read(gb, hContinueMusicAfterWarp) == 0) {
        gb_write(gb, rSelectROMBank, 0x1F);
        if (func_01F_4003) {
            func_01F_4003(gb);
        }
    }
    ReloadSavedBank(gb);
}

void PlayBoomerangSfx_trampoline(GBState *gb, void (*play_sfx)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (play_sfx) {
        play_sfx(gb);
    }
    gb_write(gb, rSelectROMBank, gb_read(gb, wCurrentBank));
}
