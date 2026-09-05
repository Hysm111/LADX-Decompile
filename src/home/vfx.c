#include "home/vfx.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "constants/vfx.h"

uint8_t AddTranscientVfx(GBState *gb, uint8_t vfx_type) {
    if (!gb) return 0;

    int8_t slot = -1;

    /* Search backwards from slot $0F down to 0 for a free slot (type == 0) */
    for (int8_t e = 0x0F; e >= 0; e--) {
        uint8_t type = gb_read(gb, wTranscientVfxTypeTable + (uint8_t)e);
        if (type == 0) {
            slot = e;
            break;
        }
    }

    /* If no free slot is found, decrement and wrap wC5C0 */
    if (slot < 0) {
        uint8_t c5c0 = gb_read(gb, wC5C0);
        c5c0--;
        if (c5c0 == 0xFF) {
            c5c0 = 0x0F;
        }
        gb_write(gb, wC5C0, c5c0);
        slot = (int8_t)c5c0;
    }

    uint8_t e = (uint8_t)slot;

    gb_write(gb, wTranscientVfxTypeTable + e, vfx_type);
    uint8_t pos_y = gb_read(gb, hMultiPurpose1);
    gb_write(gb, wTranscientVfxPosYTable + e, pos_y);
    uint8_t pos_x = gb_read(gb, hMultiPurpose0);
    gb_write(gb, wTranscientVfxPosXTable + e, pos_x);
    gb_write(gb, wTranscientVfxCountdownTable + e, 0x0F);

    return e;
}

uint8_t label_D15(GBState *gb) {
    if (!gb) return 0;

    gb_write(gb, hJingle, JINGLE_SWORD_POKING);
    return AddTranscientVfx(gb, TRANSCIENT_VFX_SWORD_POKE);
}

uint8_t label_D07(GBState *gb) {
    if (!gb) return 0;

    uint8_t x = (uint8_t)(gb_read(gb, wC140) - 0x08);
    gb_write(gb, hMultiPurpose0, x);

    uint8_t y = (uint8_t)(gb_read(gb, wC142) - 0x08);
    gb_write(gb, hMultiPurpose1, y);

    return label_D15(gb);
}
