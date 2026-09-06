#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gb.h"
#include "home/link.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/sfx.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  [FAIL] %s:%d: %s\n", __FILE__, __LINE__, msg); \
            failures++; \
        } \
    } while (0)

static void test_disableMovement_and_playNoiseStairs(void) {
    GBState gb;

    /* test disableMovementInTransition directly */
    gb_init(&gb);
    gb_write(&gb, wLinkMotionState, LINK_MOTION_DEFAULT);
    gb_write(&gb, wTransitionSequenceCounter, 0x04);
    gb_write(&gb, wC16C, 0x12);
    gb_write(&gb, wD478, 0x34);

    disableMovementInTransition(&gb);

    TEST_ASSERT(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT, "Motion state not LINK_MOTION_MAP_FADE_OUT");
    TEST_ASSERT(gb_read(&gb, wTransitionSequenceCounter) == 0, "Transition counter not cleared");
    TEST_ASSERT(gb_read(&gb, wC16C) == 0, "wC16C not cleared");
    TEST_ASSERT(gb_read(&gb, wD478) == 0, "wD478 not cleared");

    /* test playNoiseStairs directly */
    gb_init(&gb);
    gb_write(&gb, hNoiseSfx, NOISE_SFX_NONE);
    gb_write(&gb, wLinkMotionState, LINK_MOTION_DEFAULT);
    gb_write(&gb, wTransitionSequenceCounter, 0x08);

    playNoiseStairs(&gb);

    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_STAIRS, "Noise SFX not set to NOISE_SFX_STAIRS");
    TEST_ASSERT(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT, "Motion state not set");
    TEST_ASSERT(gb_read(&gb, wTransitionSequenceCounter) == 0, "Transition counter not cleared");
}

static void test_fade_out_transitions(void) {
    GBState gb;

    /* Test ApplyMapFadeOutTransitionWithNoise */
    gb_init(&gb);
    gb_write(&gb, hMusicFadeOutTimer, 0);
    gb_write(&gb, hNoiseSfx, NOISE_SFX_NONE);
    gb_write(&gb, wLinkMotionState, LINK_MOTION_DEFAULT);
    gb_write(&gb, wTransitionSequenceCounter, 5);
    gb_write(&gb, wC16C, 5);
    gb_write(&gb, wD478, 5);

    ApplyMapFadeOutTransitionWithNoise(&gb);

    TEST_ASSERT(gb_read(&gb, hMusicFadeOutTimer) == 0x30, "Fade out timer not set to 0x30");
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_STAIRS, "Noise SFX not set to NOISE_SFX_STAIRS");
    TEST_ASSERT(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT, "Motion state not LINK_MOTION_MAP_FADE_OUT");
    TEST_ASSERT(gb_read(&gb, wTransitionSequenceCounter) == 0, "Transition counter not reset");
    TEST_ASSERT(gb_read(&gb, wC16C) == 0, "wC16C not reset");
    TEST_ASSERT(gb_read(&gb, wD478) == 0, "wD478 not reset");

    /* Test ApplyMapFadeOutTransition */
    gb_init(&gb);
    gb_write(&gb, hMusicFadeOutTimer, 0);
    gb_write(&gb, hNoiseSfx, NOISE_SFX_NONE);
    gb_write(&gb, wLinkMotionState, LINK_MOTION_DEFAULT);

    ApplyMapFadeOutTransition(&gb);

    TEST_ASSERT(gb_read(&gb, hMusicFadeOutTimer) == 0x30, "Fade out timer not set to 0x30");
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_NONE, "Noise SFX should not be modified");
    TEST_ASSERT(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT, "Motion state not LINK_MOTION_MAP_FADE_OUT");

    /* Test ApplyMapFadeOutTransitionWithSound (Indoors & warp category 1) */
    gb_init(&gb);
    gb_write(&gb, wWarp0MapCategory, 1);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hContinueMusicAfterWarp, 0);
    gb_write(&gb, hMusicFadeOutTimer, 0);
    gb_write(&gb, hNoiseSfx, NOISE_SFX_NONE);

    ApplyMapFadeOutTransitionWithSound(&gb);

    TEST_ASSERT(gb_read(&gb, hContinueMusicAfterWarp) == 1, "hContinueMusicAfterWarp not set to 1");
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_STAIRS, "Noise SFX not set to NOISE_SFX_STAIRS");
    TEST_ASSERT(gb_read(&gb, hMusicFadeOutTimer) == 0, "hMusicFadeOutTimer modified when music continues");
    TEST_ASSERT(gb_read(&gb, wLinkMotionState) == LINK_MOTION_MAP_FADE_OUT, "Motion state not set");

    /* Test ApplyMapFadeOutTransitionWithSound (Outdoors -> should use noise with fade out) */
    gb_init(&gb);
    gb_write(&gb, wWarp0MapCategory, 1);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hContinueMusicAfterWarp, 0);
    gb_write(&gb, hMusicFadeOutTimer, 0);
    gb_write(&gb, hNoiseSfx, NOISE_SFX_NONE);

    ApplyMapFadeOutTransitionWithSound(&gb);

    TEST_ASSERT(gb_read(&gb, hContinueMusicAfterWarp) == 0, "hContinueMusicAfterWarp should not be set outdoors");
    TEST_ASSERT(gb_read(&gb, hMusicFadeOutTimer) == 0x30, "Fade out timer not set to 0x30 outdoors");
    TEST_ASSERT(gb_read(&gb, hNoiseSfx) == NOISE_SFX_STAIRS, "Noise SFX not set outdoors");

    /* Test ApplyMapFadeOutTransitionWithSound (Indoors but different warp category) */
    gb_init(&gb);
    gb_write(&gb, wWarp0MapCategory, 2);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hContinueMusicAfterWarp, 0);
    gb_write(&gb, hMusicFadeOutTimer, 0);

    ApplyMapFadeOutTransitionWithSound(&gb);

    TEST_ASSERT(gb_read(&gb, hContinueMusicAfterWarp) == 0, "hContinueMusicAfterWarp should not be set for category 2");
    TEST_ASSERT(gb_read(&gb, hMusicFadeOutTimer) == 0x30, "Fade out timer not set to 0x30 for category 2");
}

static void test_resets_and_position(void) {
    GBState gb;

    /* ResetPegasusBoots */
    gb_init(&gb);
    gb_write(&gb, wPegasusBootsChargeMeter, 0x15);
    gb_write(&gb, wIsRunningWithPegasusBoots, 0x01);
    gb_write(&gb, wIsUsingSpinAttack, 0x20);
    gb_write(&gb, wSwordCharge, 0x28);

    ResetPegasusBoots(&gb);

    TEST_ASSERT(gb_read(&gb, wPegasusBootsChargeMeter) == 0, "Pegasus boots meter not cleared");
    TEST_ASSERT(gb_read(&gb, wIsRunningWithPegasusBoots) == 0, "Running with boots flag not cleared");
    TEST_ASSERT(gb_read(&gb, wIsUsingSpinAttack) == 0x20, "Spin attack modified by ResetPegasusBoots");
    TEST_ASSERT(gb_read(&gb, wSwordCharge) == 0x28, "Sword charge modified by ResetPegasusBoots");

    /* ResetSpinAttack */
    ResetSpinAttack(&gb);

    TEST_ASSERT(gb_read(&gb, wIsUsingSpinAttack) == 0, "Spin attack flag not cleared");
    TEST_ASSERT(gb_read(&gb, wSwordCharge) == 0, "Sword charge not cleared");
    TEST_ASSERT(gb_read(&gb, wPegasusBootsChargeMeter) == 0, "Pegasus boots meter not cleared by spin reset");
    TEST_ASSERT(gb_read(&gb, wIsRunningWithPegasusBoots) == 0, "Running flag not cleared by spin reset");

    /* CopyLinkFinalPositionToPosition */
    gb_init(&gb);
    gb_write(&gb, hLinkFinalPositionX, 0x45);
    gb_write(&gb, hLinkFinalPositionY, 0x82);
    gb_write(&gb, hLinkPositionX, 0x00);
    gb_write(&gb, hLinkPositionY, 0x00);

    CopyLinkFinalPositionToPosition(&gb);

    TEST_ASSERT(gb_read(&gb, hLinkPositionX) == 0x45, "hLinkPositionX mismatch");
    TEST_ASSERT(gb_read(&gb, hLinkPositionY) == 0x82, "hLinkPositionY mismatch");
}

static int anim_called = 0;
static void hook_walk_anim(GBState *gb) {
    anim_called++;
    TEST_ASSERT(gb->rom_bank == 0x02, "ROM bank not switched to 0x02 during anim update");
}

static void test_update_link_walking_animation_trampoline(void) {
    GBState gb;
    gb_init(&gb);

    gb_write(&gb, wCurrentBank, 0x07);
    gb.rom_bank = 0x07;

    anim_called = 0;
    UpdateLinkWalkingAnimation_trampoline(&gb, hook_walk_anim);

    TEST_ASSERT(anim_called == 1, "Animation callback not called");
    TEST_ASSERT(gb.rom_bank == 0x07, "ROM bank not restored to wCurrentBank (0x07)");
}

void run_link_tests(void) {
    printf("[*] Running disableMovementInTransition and playNoiseStairs tests...\n");
    test_disableMovement_and_playNoiseStairs();

    printf("[*] Running Map Fade-out Transition tests...\n");
    test_fade_out_transitions();

    printf("[*] Running Link Reset and Position tests...\n");
    test_resets_and_position();

    printf("[*] Running UpdateLinkWalkingAnimation_trampoline tests...\n");
    test_update_link_walking_animation_trampoline();

    if (failures == 0) {
        printf("  [PASS] All link tests passed.\n");
    } else {
        printf("  [FAIL] %d link test(s) failed.\n", failures);
    }
}
