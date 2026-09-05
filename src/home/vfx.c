#include "home/vfx.h"
#include "constants/memory.h"

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
