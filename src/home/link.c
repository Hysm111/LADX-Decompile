#include "home/link.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gameplay.h"
#include "constants/sfx.h"

void disableMovementInTransition(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wLinkMotionState, LINK_MOTION_MAP_FADE_OUT);
    gb_write(gb, wTransitionSequenceCounter, 0);
    gb_write(gb, wC16C, 0);
    gb_write(gb, wD478, 0);
}

void playNoiseStairs(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hNoiseSfx, NOISE_SFX_STAIRS);
    disableMovementInTransition(gb);
}

void ApplyMapFadeOutTransitionWithNoise(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hMusicFadeOutTimer, 0x30);
    playNoiseStairs(gb);
}

void ApplyMapFadeOutTransition(GBState *gb) {
    if (!gb) return;

    gb_write(gb, hMusicFadeOutTimer, 0x30);
    disableMovementInTransition(gb);
}

void ApplyMapFadeOutTransitionWithSound(GBState *gb) {
    if (!gb) return;

    uint8_t category = gb_read(gb, wWarp0MapCategory);
    uint8_t indoor = gb_read(gb, wIsIndoor);

    if (category == 1 && indoor != 0) {
        gb_write(gb, hContinueMusicAfterWarp, 1);
        playNoiseStairs(gb);
    } else {
        ApplyMapFadeOutTransitionWithNoise(gb);
    }
}

void ResetPegasusBoots(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wPegasusBootsChargeMeter, 0);
    gb_write(gb, wIsRunningWithPegasusBoots, 0);
}

void ResetSpinAttack(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wIsUsingSpinAttack, 0);
    gb_write(gb, wSwordCharge, 0);
    ResetPegasusBoots(gb);
}

void CopyLinkFinalPositionToPosition(GBState *gb) {
    if (!gb) return;

    uint8_t x = gb_read(gb, hLinkFinalPositionX);
    uint8_t y = gb_read(gb, hLinkFinalPositionY);
    gb_write(gb, hLinkPositionX, x);
    gb_write(gb, hLinkPositionY, y);
}

void UpdateLinkWalkingAnimation_trampoline(GBState *gb, void (*update_func)(GBState *)) {
    if (!gb) return;

    /* ld a, BANK(LinkAnimationsLists) -> bank 2 */
    gb_write(gb, rSelectROMBank, 0x02);
    if (update_func) {
        update_func(gb);
    }
    ReloadSavedBank(gb);
}
