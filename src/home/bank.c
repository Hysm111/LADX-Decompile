#include "home/bank.h"
#include "home/copy_data.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gfx.h"

uint8_t AdjustBankNumberForGBC(GBState *gb, uint8_t bank) {
    if (!gb) return bank;

    uint8_t is_gbc = gb_read(gb, hIsGBC);
    if (is_gbc != 0) {
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

    uint8_t saved = gb_read(gb, wCurrentBank);
    gb_write(gb, rSelectROMBank, saved);
}

void RestoreBankAndReturn(GBState *gb) {
    if (!gb) return;

    uint8_t saved = gb_read(gb, wCurrentBank);
    gb_write(gb, rSelectROMBank, saved);
}

void LoadBank1AndReturn(GBState *gb) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x01);
}

void RestoreStackedBankAndReturn(GBState *gb, uint8_t stacked_bank) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, stacked_bank);
}

void RestoreStackedBank(GBState *gb, uint8_t stacked_bank) {
    if (!gb) return;

    SwitchBank(gb, stacked_bank);
}

uint16_t Farcall_trampoline(GBState *gb) {
    if (!gb) return 0;

    uint8_t high = gb_read(gb, wFarcallAdressHigh);
    uint8_t low = gb_read(gb, wFarcallAdressLow);
    return ((uint16_t)high << 8) | low;
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

bool CheckOverworldObjectIgnoreList(uint8_t object_id) {
    static const uint8_t ignore_list[] = {
        0x03, 0x04, 0x09, 0x5E, 0x91, 0xA1, 0xAA,
        0xC4, 0xC6, 0xCC, 0xDB, 0xE1, 0xE3, 0xE8
    };
    for (size_t i = 0; i < sizeof(ignore_list) / sizeof(ignore_list[0]); i++) {
        if (ignore_list[i] == object_id) {
            return true;
        }
    }
    return false;
}

void CopyObjectsAttributesToWRAM2(GBState *gb, uint16_t de, uint16_t hl, uint16_t bc) {
    if (!gb) return;

    uint8_t src_bank = gb_read(gb, hMultiPurpose0);
    gb_write(gb, rSelectROMBank, src_bank);

    gb_write(gb, rSVBK, 2);
    CopyData(gb, de, hl, bc);
    gb_write(gb, rSVBK, 0);

    gb_write(gb, rSelectROMBank, 0x20);
}

void BackupObjectInRAM2(GBState *gb, uint16_t hl, uint8_t flags_and_return_bank) {
    if (!gb) return;

    gb_write(gb, hMultiPurpose2, flags_and_return_bank);

    uint8_t is_gbc = gb_read(gb, hIsGBC);
    if (!is_gbc) {
        return;
    }

    /* Check if object is in ignore list unless bit 7 of flags is set */
    if ((flags_and_return_bank & 0x80) == 0) {
        uint8_t obj = gb_read(gb, hl);
        if (!CheckOverworldObjectIgnoreList(obj)) {
            /* Object is not in ignore list (carry set in asm), skip copy */
            uint8_t return_bank = flags_and_return_bank & 0x7F;
            gb_write(gb, rSelectROMBank, return_bank);
            return;
        }
    }

    /* Copy object from RAM bank 0 to RAM bank 2 */
    uint8_t val = gb_read(gb, hl);
    gb_write(gb, rSVBK, 2);
    gb_write(gb, hl, val);
    gb_write(gb, rSVBK, 0);

    /* Restore ROM bank (bits 0-6) */
    uint8_t return_bank = flags_and_return_bank & 0x7F;
    gb_write(gb, rSelectROMBank, return_bank);
}

void func_020_6A30_trampoline(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreBankAndReturn(gb);
}

void func_020_6AC1_trampoline(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreBankAndReturn(gb);
}

void UpdateIntroSeaBGPalettes_trampoline(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreBankAndReturn(gb);
}

void ClearFileMenuBG_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void LoadFileMenuBG_trampoline(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    LoadBank1AndReturn(gb);
}

void CopyLinkTunicPalette_trampoline(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    LoadBank1AndReturn(gb);
}

void LoadBaseTiles_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void ChangeBGColumnPaletteAndExecuteDrawCommands(GBState *gb, uint8_t stacked_bank, void (*change_palette)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x24);
    if (change_palette) {
        change_palette(gb);
    }
    ExecuteDrawCommands(gb, wDrawCommand);
    RestoreStackedBank(gb, stacked_bank);
}

void func_A9B(GBState *gb, uint8_t stacked_bank, void (*execute_dialog)(GBState *)) {
    if (!gb) return;

    SwitchBank(gb, BANK_FontTiles);
    if (execute_dialog) {
        execute_dialog(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void Spawn2x2RubbleEntities_trampoline(GBState *gb, uint8_t stacked_bank, void (*spawn_func)(GBState *)) {
    if (!gb) return;

    SwitchBank(gb, 0x36);
    if (spawn_func) {
        spawn_func(gb);
    }
    RestoreStackedBank(gb, stacked_bank);
}

void func_A5F(GBState *gb, uint8_t stacked_bank, void (*render_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (render_func) {
        render_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_036_703E_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void cycleInstrumentItemColor_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_036_4A77_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    SwitchBank(gb, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void GetOwlStatueDialogId_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void SpawnPhotographer_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void LoadPhotoBgMap_trampoline(GBState *gb, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x3D);
    if (target_func) {
        target_func(gb);
    }
}

void func_020_6D0E_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void CheckPushedTombStone_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void GetEntityInitHandler_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_020_4874_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_020_4954_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void ReplaceObjects56and57_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_036_505F_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_036_4F9B_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_003_5A2E_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x03);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_036_4F68_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_020_6D52_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x20);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}

void func_036_4BE8_trampoline(GBState *gb, uint8_t stacked_bank, void (*target_func)(GBState *)) {
    if (!gb) return;

    gb_write(gb, rSelectROMBank, 0x36);
    if (target_func) {
        target_func(gb);
    }
    RestoreStackedBankAndReturn(gb, stacked_bank);
}
