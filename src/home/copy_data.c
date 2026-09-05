#include "home/copy_data.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gfx.h"
#include "constants/gameplay.h"

void CopyData(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc) {
    if (!gb) return;

    while (bc > 0) {
        uint8_t val = gb_read(gb, hl++);
        gb_write(gb, de++, val);
        bc--;
    }
}

void CopyDataFromBank(GBState *gb, uint8_t bank, uint16_t de, uint16_t hl, uint16_t bc) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, de, hl, bc);
    gb_write(gb, rSelectROMBank, 1);
}

void CopyData_trampoline(GBState *gb, uint8_t bank, uint16_t de, uint16_t hl, uint16_t bc) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, de, hl, bc);
    gb_write(gb, rSelectROMBank, 0x28);
}

void DrawCommandToVRAM(GBState *gb, uint16_t *de, uint16_t hl, uint8_t a) {
    if (!gb || !de) return;

    uint8_t mode = a & 0xC0;
    uint8_t count = (a & 0x3F) + 1;

    switch (mode) {
        case DC_COPY_ROW: {
            for (uint8_t i = 0; i < count; i++) {
                uint8_t tile = gb_read(gb, (*de)++);
                gb_write(gb, hl, tile);
                hl++;
                if ((hl & 0x1F) == 0) {
                    hl -= 32;
                }
            }
            break;
        }
        case DC_FILL_ROW: {
            uint8_t tile = gb_read(gb, (*de)++);
            for (uint8_t i = 0; i < count; i++) {
                gb_write(gb, hl, tile);
                hl++;
                if ((hl & 0x1F) == 0) {
                    hl -= 32;
                }
            }
            break;
        }
        case DC_COPY_COLUMN: {
            for (uint8_t i = 0; i < count; i++) {
                uint8_t tile = gb_read(gb, (*de)++);
                gb_write(gb, hl, tile);
                hl += 0x20;
            }
            break;
        }
        case DC_FILL_COLUMN: {
            uint8_t tile = gb_read(gb, (*de)++);
            for (uint8_t i = 0; i < count; i++) {
                gb_write(gb, hl, tile);
                hl += 0x20;
            }
            break;
        }
    }
}

void DrawCommandToVRAMDuringRoomTransition(GBState *gb, uint16_t *de, uint16_t hl, uint8_t a) {
    if (!gb || !de) return;

    uint8_t is_column = a & 0x80;
    uint8_t count = (a & 0x3F) + 1;

    if (!is_column) {
        /* Row copy with transparency skip */
        for (uint8_t i = 0; i < count; i++) {
            uint8_t tile = gb_read(gb, (*de)++);
            if (tile != 0xEE) {
                gb_write(gb, hl, tile);
            }
            hl++;
            if ((hl & 0x1F) == 0) {
                hl -= 32;
            }
        }
    } else {
        /* Column copy with transparency skip */
        for (uint8_t i = 0; i < count; i++) {
            uint8_t tile = gb_read(gb, (*de)++);
            if (tile != 0xEE) {
                gb_write(gb, hl, tile);
            }
            hl += 0x20;
        }
    }
}

void NoRoomTransitionDrawLoop(GBState *gb, uint16_t de, uint8_t a_dest_high) {
    if (!gb) return;

    while (1) {
        uint8_t dest_low = gb_read(gb, de++);
        uint16_t dst = ((uint16_t)a_dest_high << 8) | dest_low;
        uint8_t cmd = gb_read(gb, de++);
        DrawCommandToVRAM(gb, &de, dst, cmd);

        a_dest_high = gb_read(gb, de);
        if (a_dest_high == 0) {
            break;
        }
        de++;
    }
}

void ExecuteDrawCommands(GBState *gb, uint16_t de) {
    if (!gb) return;

    uint8_t transition_state = gb_read(gb, wRoomTransitionState);
    if (transition_state != 0) {
        /* Room transition mode */
        while (1) {
            uint8_t dest_high = gb_read(gb, de);
            if (dest_high == 0) {
                break;
            }
            de++;
            uint8_t dest_low = gb_read(gb, de++);
            uint16_t dst = ((uint16_t)dest_high << 8) | dest_low;
            uint8_t cmd = gb_read(gb, de++);
            DrawCommandToVRAMDuringRoomTransition(gb, &de, dst, cmd);
        }
    } else {
        /* Standard non-transition mode */
        while (1) {
            uint8_t dest_high = gb_read(gb, de);
            if (dest_high == 0) {
                break;
            }
            de++;
            uint8_t dest_low = gb_read(gb, de++);
            uint16_t dst = ((uint16_t)dest_high << 8) | dest_low;
            uint8_t cmd = gb_read(gb, de++);
            DrawCommandToVRAM(gb, &de, dst, cmd);
        }
    }
}

void CopyToBGMap0(GBState *gb, uint16_t *hl) {
    if (!gb || !hl) return;

    uint16_t de = vBGMap0;
    while (1) {
        uint8_t val = gb_read(gb, (*hl)++);
        gb_write(gb, de, val);
        de++;
        if ((de & 0x1F) == 0x14) {
            de += 0x0C;
            if (de == 0x9A40) {
                break;
            }
        }
    }
}

void CopyDataToVRAM_noDMA(GBState *gb, uint8_t b, uint8_t c, uint8_t return_bank) {
    if (!gb) return;

    uint16_t src = (uint16_t)b << 8;
    uint16_t dst = 0x8000 + ((uint16_t)c << 8);
    CopyData(gb, dst, src, 0x100);
    gb_write(gb, rSelectROMBank, return_bank);
}

void CopyDataToVRAM(GBState *gb, uint8_t src_bank, uint8_t b, uint8_t c, uint8_t return_bank) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, src_bank);

    if (gb_read(gb, hIsGBC) == 0) {
        CopyDataToVRAM_noDMA(gb, b, c, return_bank);
        return;
    }

    gb_write(gb, rHDMA1, b);
    gb_write(gb, rHDMA2, 0x00);
    gb_write(gb, rHDMA3, c);
    gb_write(gb, rHDMA4, 0x00);
    gb_write(gb, rHDMA5, 0x0F);

    gb_write(gb, rSelectROMBank, return_bank);
}

void func_BB5(GBState *gb, uint16_t hl) {
    if (!gb) return;

    CopyData(gb, wIsFileSelectionArrowShifted, hl, 0x168);
}

void CopyBGMapFromBank(GBState *gb, uint8_t src_bank, uint16_t hl) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, src_bank);

    if (gb_read(gb, hIsGBC) != 0) {
        uint16_t hl_attr = hl + 0x168;
        gb_write(gb, rVBK, 1);
        CopyToBGMap0(gb, &hl_attr);
        gb_write(gb, rVBK, 0);
    }

    uint16_t hl_tiles = hl;
    CopyToBGMap0(gb, &hl_tiles);

    if (gb_read(gb, wGameplayType) == GAMEPLAY_PHOTO_ALBUM) {
        func_BB5(gb, hl);
    }

    uint8_t return_bank = gb_read(gb, hMultiPurposeF);
    gb_write(gb, rSelectROMBank, return_bank);
}
