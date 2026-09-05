#include "home/audio.h"
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
