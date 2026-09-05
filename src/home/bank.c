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

void RestoreBankAndReturn(GBState *gb) {
    if (!gb) return;

    uint8_t bank = gb_read(gb, wCurrentBank);
    gb_write(gb, rSelectROMBank, bank);
}

void LoadBank1AndReturn(GBState *gb) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 1);
}

void RestoreStackedBankAndReturn(GBState *gb, uint8_t stacked_bank) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, stacked_bank);
}

void RestoreStackedBank(GBState *gb, uint8_t stacked_bank) {
    if (!gb) return;

    SwitchBank(gb, stacked_bank);
}

void Farcall(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    uint8_t target_bank = gb_read(gb, wFarcallBank);
    gb_write(gb, rSelectROMBank, target_bank);

    if (target_func) {
        target_func(gb);
    }

    uint8_t return_bank = gb_read(gb, wFarcallReturnBank);
    gb_write(gb, rSelectROMBank, return_bank);
}

static const uint8_t sOverworldObjectIgnoreList[14] = {
    0x03, 0x04, 0x09, 0x5E, 0x91, 0xA1, 0xAA, 0xC4, 0xC6, 0xCC, 0xDB, 0xE1, 0xE3, 0xE8
};

bool CheckOverworldObjectIgnoreList(uint8_t object_id) {
    for (size_t i = 0; i < sizeof(sOverworldObjectIgnoreList); i++) {
        if (sOverworldObjectIgnoreList[i] == object_id) {
            return true;
        }
    }
    return false;
}

void BackupObjectInRAM2(GBState *gb, uint16_t hl, uint8_t flags_and_return_bank) {
    if (!gb) return;

    gb_write(gb, hMultiPurpose2, flags_and_return_bank);

    if (gb_read(gb, hIsGBC) == 0) {
        return;
    }
    if (gb_read(gb, wIsIndoor) != 0) {
        return;
    }

    if ((flags_and_return_bank & 0x80) == 0) {
        uint8_t obj = gb_read(gb, hl);
        if (!CheckOverworldObjectIgnoreList(obj)) {
            gb_write(gb, rSelectROMBank, flags_and_return_bank & 0x7F);
            return;
        }
    }

    uint8_t b = gb_read(gb, hl);
    gb_write(gb, rSVBK, 0x02);
    gb_write(gb, hl, b);
    gb_write(gb, rSVBK, 0x00);

    gb_write(gb, rSelectROMBank, flags_and_return_bank & 0x7F);
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
