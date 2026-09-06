#include "home/audio.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include <stdio.h>
#include <assert.h>

void test_play_wrong_answer_jingle(void) {
    printf("[*] Running PlayWrongAnswerJingle tests...\n");

    GBState gb;
    gb_init(&gb);

    gb_write(&gb, hJingle, 0x00);
    PlayWrongAnswerJingle(&gb);
    assert(gb_read(&gb, hJingle) == JINGLE_WRONG_ANSWER);
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

    /* Verify noise sfx registered */
    assert(gb_read(&gb, hNoiseSfx) == NOISE_SFX_EXPLOSION);
    /* Verify moblins alerted by the explosion */
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

void run_audio_tests(void) {
    test_play_wrong_answer_jingle();
    test_alert_sword_moblins();
    test_play_bomb_explosion_sfx();
    test_play_audio_step();
    printf("  [PASS] All audio.asm functions verified successfully!\n\n");
}
