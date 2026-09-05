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

void run_audio_tests(void) {
    test_play_wrong_answer_jingle();
    test_alert_sword_moblins();
    test_play_bomb_explosion_sfx();
    printf("  [PASS] All audio.asm functions verified successfully!\n\n");
}
