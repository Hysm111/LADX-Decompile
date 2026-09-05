#include "home/clear_memory.h"

void ClearBytes(GBState *gb, uint16_t hl, uint16_t bc) {
    if (!gb) return;

    /* Save hIsGBC (ldh a, [hIsGBC] / push af) */
    uint8_t saved_is_gbc = gb_read(gb, hIsGBC);

    /* Loop clearing memory (xor a; ldi [hl], a; dec bc; ld a, b; or c; jr nz, .loop) */
    do {
        gb_write(gb, hl++, 0);
        bc--;
    } while (bc != 0);

    /* Restore hIsGBC (pop af / ldh [hIsGBC], a) */
    gb_write(gb, hIsGBC, saved_is_gbc);
}

void ClearWRAMBytes(GBState *gb, uint16_t bc) {
    ClearBytes(gb, wram0Section, bc);
}

void ClearHRAMBytesAndWRAM(GBState *gb, uint16_t bc) {
    ClearBytes(gb, hGameValuesSection, bc);
    ClearWRAMBytes(gb, 0xDF00 - wram0Section);
}

void ClearHRAMAndWRAM(GBState *gb) {
    ClearHRAMBytesAndWRAM(gb, hIsComputingFrame - hGameValuesSection);
}

void ClearWRAMAndLowerHRAM(GBState *gb) {
    ClearHRAMBytesAndWRAM(gb, hNextDefaultMusicTrack - hGameValuesSection);
}

void ClearLowerWRAM(GBState *gb) {
    ClearWRAMBytes(gb, wAudioSection - wram0Section);
}

void ClearLowerAndMiddleWRAM(GBState *gb) {
    ClearWRAMBytes(gb, wDrawCommandsSection - wram0Section);
}

void FillBGMap(GBState *gb, uint8_t val, uint16_t count) {
    if (!gb) return;

    uint16_t hl = vBGMap0;
    do {
        gb_write(gb, hl++, val);
        count--;
    } while (count != 0);
}

void FillBGMapBlack(GBState *gb) {
    FillBGMap(gb, DIALOG_BG_TILE_DARK, 0x400);
}

void FillBGMapWhite(GBState *gb) {
    FillBGMap(gb, 0x7F, 0x800);
}
