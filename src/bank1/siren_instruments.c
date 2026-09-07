#include "bank1/siren_instruments.h"
#include "home/copy_data.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include <string.h>

const uint8_t Data_001_6BCF[8] = {
    0x0F, 0x51, 0xB1, 0xEF, 0xEC, 0xAA, 0x4A, 0x0C
};

const uint8_t Data_001_6BD7[8] = {
    0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8
};

const uint8_t Data_001_6BDF[8] = {
    0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE
};

const uint8_t Data_001_6BE7[3] = {
    1, 0x1F, 1
};

uint16_t GetInstrumentNextBGAddress(uint16_t hl, uint8_t bc_idx) {
    return (uint16_t)(hl + Data_001_6BE7[bc_idx]);
}

void LoadInstrumentsBG(GBState *gb, uint8_t side) {
    if (!gb) return;
    uint8_t c = (side != 0) ? 4 : 8;
    uint8_t e = (side != 0) ? 0 : 4;
    gb_write_hram(gb, hMultiPurpose9, c);

    while (e < c) {
        gb_write_hram(gb, hMultiPurpose0, 0);
        gb_write_hram(gb, hMultiPurpose1, 0);
        gb_write_hram(gb, hMultiPurpose2, 0);
        gb_write_hram(gb, hMultiPurpose3, 0);

        uint8_t has_instrument = gb_read(gb, (uint16_t)(wHasInstrument1 + e));
        uint16_t hl = (uint16_t)(0x9D00 + Data_001_6BCF[e]);

        if (has_instrument & 0x02) {
            uint8_t tile = Data_001_6BDF[e];
            gb_write_hram(gb, hMultiPurpose0, tile);
            gb_write_hram(gb, hMultiPurpose1, (uint8_t)(tile + 1));
            gb_write_hram(gb, hMultiPurpose2, (uint8_t)(tile + 0x10));
            gb_write_hram(gb, hMultiPurpose3, (uint8_t)(tile + 0x11));
        } else {
            gb_write_hram(gb, hMultiPurpose0, 0x7C);
            gb_write_hram(gb, hMultiPurpose1, 0x7C);
            gb_write_hram(gb, hMultiPurpose2, 0x7C);
            gb_write_hram(gb, hMultiPurpose3, Data_001_6BD7[e]);
        }

        /* copyToBG */
        gb_write(gb, hl, gb_read_hram(gb, hMultiPurpose0));
        hl = GetInstrumentNextBGAddress(hl, 0);
        gb_write(gb, hl, gb_read_hram(gb, hMultiPurpose1));
        hl = GetInstrumentNextBGAddress(hl, 1);
        gb_write(gb, hl, gb_read_hram(gb, hMultiPurpose2));
        hl = GetInstrumentNextBGAddress(hl, 2);
        gb_write(gb, hl, gb_read_hram(gb, hMultiPurpose3));

        e++;
    }
}

void LoadSirenInstrumentTiles(GBState *gb, uint8_t instrument_idx) {
    if (!gb) return;
    uint16_t offset = (uint16_t)(instrument_idx << 6);
    uint16_t de = (uint16_t)(0x8D00 + offset);
    uint16_t hl = (uint16_t)(SirenInstrumentsTiles + offset);
    CopySirenInstrumentTiles(gb, de, hl);

    uint8_t stage = (uint8_t)(gb_read_hram(gb, hBGTilesLoadingStage) + 1);
    gb_write_hram(gb, hBGTilesLoadingStage, stage);
}

void LoadSirenInstruments(GBState *gb) {
    if (!gb) return;
    uint8_t stage = gb_read_hram(gb, hBGTilesLoadingStage);
    if (stage < 8) {
        LoadSirenInstrumentTiles(gb, stage);
    } else if (stage == 8) {
        LoadInstrumentsBG(gb, 1); /* rightSide */
        uint8_t next_stage = (uint8_t)(gb_read_hram(gb, hBGTilesLoadingStage) + 1);
        gb_write_hram(gb, hBGTilesLoadingStage, next_stage);
    } else {
        LoadInstrumentsBG(gb, 0); /* leftSide */
        gb_write_hram(gb, hNeedsUpdatingBGTiles, 0);
        gb_write_hram(gb, hBGTilesLoadingStage, 0);
    }
}
