#include <stdio.h>
#include <assert.h>
#include "gb.h"
#include "home/audio.h"
#include "constants/audio.h"
#include "constants/hardware.h"
#include "constants/memory.h"

void test_play_wrong_answer_jingle(void) {
    printf("[*] Running PlayWrongAnswerJingle tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, hJingle, 0x00);
    PlayWrongAnswerJingle(&gb);
    assert(gb_read(&gb, hJingle) == 0x03);
}

void test_alert_sword_moblins(void) {
    printf("[*] Running AlertSwordMoblins tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wSwordMoblinAlertingSoundCounter, 0x00);
    AlertSwordMoblins(&gb);
    assert(gb_read(&gb, wSwordMoblinAlertingSoundCounter) == 4);
}

void test_play_bomb_explosion_sfx(void) {
    printf("[*] Running PlayBombExplosionSfx tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, hNoiseSfx, 0x00);
    gb_write(&gb, wSwordMoblinAlertingSoundCounter, 0x00);

    PlayBombExplosionSfx(&gb);

    assert(gb_read(&gb, hNoiseSfx) == 0x04);
    assert(gb_read(&gb, wSwordMoblinAlertingSoundCounter) == 4);
}

static int sfx_calls = 0;
static int m1b_calls = 0;
static int m1e_calls = 0;

static void hook_sfx(GBState *gb) {
    sfx_calls++;
    assert(gb->rom_bank == 0x1F);
}

static void hook_m1b(GBState *gb) {
    m1b_calls++;
    assert(gb->rom_bank == 0x1B);
}

static void hook_m1e(GBState *gb) {
    m1e_calls++;
    assert(gb->rom_bank == 0x1E);
}

void test_play_audio_step(void) {
    printf("[*] Running PlayAudioStep tests...\n");

    GBState gb;
    gb_init(&gb);

    /* 1. Wave SFX playing -> early return */
    sfx_calls = m1b_calls = m1e_calls = 0;
    gb_write(&gb, hWaveSfx, 0x01);
    gb_write(&gb, wMusicTrackTiming, 0x00);
    PlayAudioStepWithHooks(&gb, hook_sfx, hook_m1b, hook_m1e);
    assert(sfx_calls == 1);
    assert(m1b_calls == 0);
    assert(m1e_calls == 0);

    /* 2. Wave SFX zero, timing 0 -> normal speed (1 step) */
    sfx_calls = m1b_calls = m1e_calls = 0;
    gb_write(&gb, hWaveSfx, 0x00);
    gb_write(&gb, wMusicTrackTiming, 0x00);
    PlayAudioStepWithHooks(&gb, hook_sfx, hook_m1b, hook_m1e);
    assert(sfx_calls == 1);
    assert(m1b_calls == 1);
    assert(m1e_calls == 1);
    assert(gb.rom_bank == 0x1E);

    /* 3. Wave SFX zero, timing 2, odd frame -> early return */
    sfx_calls = m1b_calls = m1e_calls = 0;
    gb_write(&gb, hWaveSfx, 0x00);
    gb_write(&gb, wMusicTrackTiming, 0x02);
    gb_write(&gb, hFrameCounter, 0x05); /* odd frame */
    PlayAudioStepWithHooks(&gb, hook_sfx, hook_m1b, hook_m1e);
    assert(sfx_calls == 1);
    assert(m1b_calls == 0);
    assert(m1e_calls == 0);

    /* 4. Wave SFX zero, timing 2, even frame -> 1 step */
    sfx_calls = m1b_calls = m1e_calls = 0;
    gb_write(&gb, hWaveSfx, 0x00);
    gb_write(&gb, wMusicTrackTiming, 0x02);
    gb_write(&gb, hFrameCounter, 0x06); /* even frame */
    PlayAudioStepWithHooks(&gb, hook_sfx, hook_m1b, hook_m1e);
    assert(sfx_calls == 1);
    assert(m1b_calls == 1);
    assert(m1e_calls == 1);

    /* 5. Wave SFX zero, timing 1 -> double speed (2 steps) */
    sfx_calls = m1b_calls = m1e_calls = 0;
    gb_write(&gb, hWaveSfx, 0x00);
    gb_write(&gb, wMusicTrackTiming, 0x01);
    PlayAudioStepWithHooks(&gb, hook_sfx, hook_m1b, hook_m1e);
    assert(sfx_calls == 1);
    assert(m1b_calls == 2);
    assert(m1e_calls == 2);
}

static int mock_select_music_calls = 0;
static void mock_select_music(GBState *gb) {
    mock_select_music_calls++;
    assert(gb->rom_bank == 0x02);
}

static int mock_func_1f_calls = 0;
static void mock_func_1f(GBState *gb) {
    mock_func_1f_calls++;
    assert(gb->rom_bank == 0x1F);
}

void test_music_fade_and_track_routines(void) {
    printf("[*] Running SetWorldMusicTrack & fade tests...\n");

    GBState gb;
    gb_init(&gb);

    /* Test SetWorldMusicTrack */
    SetWorldMusicTrack(&gb, 0x2A);
    assert(gb_read(&gb, wMusicTrackToPlay) == 0x2A);
    assert(gb_read(&gb, hNextDefaultMusicTrack) == 0x2A);
    assert(gb_read(&gb, hMusicFadeInTimer) == 0x38);
    assert(gb_read(&gb, hMusicFadeOutTimer) == 0x00);

    /* Test ResetMusicFadeTimer */
    ResetMusicFadeTimer(&gb);
    assert(gb_read(&gb, hMusicFadeOutTimer) == MUSIC_FADE_OUT_TIMER_MAX);
    assert(gb_read(&gb, hMusicFadeInTimer) == 0x00);

    /* Test SelectMusicTrackAfterTransition_trampoline */
    gb_write(&gb, wCurrentBank, 0x05);
    gb.rom_bank = 0x05;
    mock_select_music_calls = 0;
    SelectMusicTrackAfterTransition_trampoline(&gb, mock_select_music);
    assert(mock_select_music_calls == 1);
    assert(gb.rom_bank == 0x05);

    /* Test func_27F2 with hContinueMusicAfterWarp == 0 */
    gb_write(&gb, wCurrentBank, 0x03);
    gb.rom_bank = 0x03;
    gb_write(&gb, hContinueMusicAfterWarp, 0x00);
    mock_func_1f_calls = 0;
    func_27F2(&gb, mock_func_1f);
    assert(mock_func_1f_calls == 1);
    assert(gb.rom_bank == 0x03);

    /* Test func_27F2 with hContinueMusicAfterWarp != 0 */
    gb_write(&gb, hContinueMusicAfterWarp, 0x01);
    mock_func_1f_calls = 0;
    func_27F2(&gb, mock_func_1f);
    assert(mock_func_1f_calls == 0);
    assert(gb.rom_bank == 0x03);
}

void run_audio_tests(void) {
    test_play_wrong_answer_jingle();
    test_alert_sword_moblins();
    test_play_bomb_explosion_sfx();
    test_play_audio_step();
    test_music_fade_and_track_routines();
    printf("  [PASS] All audio.asm functions verified successfully!\n\n");
}
