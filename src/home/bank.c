#include "home/bank.h"
#include "home/copy_data.h"
#include "constants/hardware.h"
#include "constants/memory.h"

uint8_t AdjustBankNumberForGBC(GBState *gb, uint8_t bank) {
    if (!gb) return bank;

    if (gb_read(gb, hIsGBC) != 0) {
        return bank | 0x20;
    }
    return bank;
}

void SwitchBank(GBState *gb, uint8_t bank) {
    if (!gb) return;

    gb_write(gb, wCurrentBank, bank);
    gb_write(gb, rSelectROMBank, bank);
}

void SwitchAdjustedBank(GBState *gb, uint8_t bank) {
    if (!gb) return;

    uint8_t adjusted = AdjustBankNumberForGBC(gb, bank);
    SwitchBank(gb, adjusted);
}

void ReloadSavedBank(GBState *gb) {
    if (!gb) return;

    uint8_t bank = gb_read(gb, wCurrentBank);
    gb_write(gb, rSelectROMBank, bank);
}

void CopyObjectsAttributesToWRAM2(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc) {
    if (!gb) return;

    uint8_t src_bank = gb_read(gb, hMultiPurpose0);
    gb_write(gb, rSelectROMBank, src_bank);
    gb_write(gb, rSVBK, 0x02);
    CopyData(gb, de, hl, bc);
    gb_write(gb, rSVBK, 0x00);
    gb_write(gb, rSelectROMBank, 0x20);
}
