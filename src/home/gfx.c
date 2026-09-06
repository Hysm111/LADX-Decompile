#include "home/gfx.h"
#include "home/bank.h"
#include "home/copy_data.h"
#include "home/audio.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/memory.h"

void LoadCreditsKoholintDisappearingTiles(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, bank);

    CopyData(gb, vTiles2, EndingTiles + 0x2800, TILE_SIZE * 0x80);
    CopyData(gb, vTiles1, EndingTiles + 0x3000, TILE_SIZE * 0x80);
}

void LoadTileset15(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, EndingTiles, TILE_SIZE * 0x180);

    bank = AdjustBankNumberForGBC(gb, BANK_Overworld1Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles2 + 0x7F0, Overworld1Tiles + 0x8E0, TILE_SIZE);

    bank = AdjustBankNumberForGBC(gb, BANK_Npc4Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, Npc4Tiles + 0x100, TILE_SIZE * 4);

    CopyData(gb, vTiles1 + 0x500, Npc4Tiles + 0x100, TILE_SIZE * 0x20);
}

void LoadCreditsStairsTiles(GBState *gb) {
    if (!gb) return;

    LoadTileset15(gb);
    CopyData(gb, vTiles0 + 0x400, EndingTiles + 0x3600, TILE_SIZE * 0x10);
}

void LoadCreditsKoholintViewsTiles(GBState *gb) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_Overworld1Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles2, Overworld1Tiles + 0x100, TILE_SIZE * 0x80);

    bank = AdjustBankNumberForGBC(gb, BANK_Npc3Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, Npc3Tiles + 0x2000, TILE_SIZE * 0x80);

    bank = AdjustBankNumberForGBC(gb, BANK_Overworld2Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles1, Overworld2Tiles + 0x600, TILE_SIZE * 0x80);
}

static void copyLinkOnSeaTiles(GBState *gb, uint16_t src_hl, uint8_t bank, bool adjust_bank) {
    if (adjust_bank) {
        bank = AdjustBankNumberForGBC(gb, bank);
    }
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, src_hl, TILE_SIZE * 0x80);

    uint8_t ending_bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, ending_bank);
    CopyData(gb, vTiles0 + TILE_SIZE * 0x80, EndingTiles + 0x1800, TILE_SIZE * 0x100);
}

void LoadCreditsLinkOnSeaCloseTiles(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) == 0) {
        copyLinkOnSeaTiles(gb, EndingTiles, BANK_EndingTiles, true);
    } else {
        copyLinkOnSeaTiles(gb, PhotoAlbumTiles, BANK_PhotoAlbumTiles, false);
    }
}

void LoadCreditsSunAboveTiles(GBState *gb) {
    if (!gb) return;

    copyLinkOnSeaTiles(gb, EndingTiles + 0x800, BANK_EndingTiles, true);
}

void LoadCreditsLinkOnSeaLargeTiles(GBState *gb) {
    if (!gb) return;

    copyLinkOnSeaTiles(gb, EndingTiles + 0x2000, BANK_EndingTiles, true);
}

static void loadCreditsEndingSceneTiles(GBState *gb, uint16_t src_hl, uint8_t bank, bool adjust_bank) {
    if (adjust_bank) {
        bank = AdjustBankNumberForGBC(gb, bank);
    }
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, src_hl, TILE_SIZE * 0x80);

    uint8_t ending_bank = AdjustBankNumberForGBC(gb, BANK_EndingTiles);
    gb_write(gb, rSelectROMBank, ending_bank);
    CopyData(gb, vTiles1, EndingTiles + 0x3000, TILE_SIZE * 0x80);
    CopyData(gb, vTiles2, EndingTiles + 0x2800, TILE_SIZE * 0x80);
}

void func_2B92(GBState *gb, uint16_t src_hl, uint8_t bank) {
    if (!gb) return;
    loadCreditsEndingSceneTiles(gb, src_hl, bank, true);
}

void LoadCreditsLinkFaceCloseUpTiles(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) == 0) {
        loadCreditsEndingSceneTiles(gb, EndingTiles + 0x3800, BANK_EndingTiles, true);
    } else {
        loadCreditsEndingSceneTiles(gb, EndingCGBAltTiles, BANK_EndingCGBAltTiles, false);
    }
}

void LoadCreditsLinkSeatedOnLogTiles(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, hIsGBC) == 0) {
        loadCreditsEndingSceneTiles(gb, EndingTiles + 0x800, BANK_EndingTiles, true);
    } else {
        loadCreditsEndingSceneTiles(gb, PhotoAlbumTiles + 0x800, BANK_PhotoAlbumTiles, false);
    }
}

void LoadCreditsRollTiles(GBState *gb) {
    if (!gb) return;

    PlayAudioStep(gb);

    func_2B92(gb, FontLargeTiles + 0x100, BANK_FontLargeTiles);

    PlayAudioStep(gb);

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_Npc3Tiles);
    gb_write(gb, rSelectROMBank, bank);
    CopyData(gb, vTiles0, Npc3Tiles + 0x2600, TILE_SIZE * 0x08);

    PlayAudioStep(gb);

    if (gb_read(gb, hIsGBC) == 0) {
        gb_write(gb, rSelectROMBank, BANK_FontLargeTiles);
        CopyData(gb, vTiles0 + 0x100, FontLargeTiles + 0x200, TILE_SIZE * 0x70);
    } else {
        gb_write(gb, rSelectROMBank, BANK_CreditsRollTiles);
        CopyData(gb, vTiles0, CreditsRollTiles, TILE_SIZE * 0x80);
    }
}

void LoadBaseTiles(GBState *gb) {
    if (!gb) return;

    SwitchAdjustedBank(gb, BANK_LinkCharacterTiles);
    CopyData(gb, vTiles0, LinkCharacterTiles, TILE_SIZE * 0x40);

    SwitchAdjustedBank(gb, BANK_InventoryEquipmentItemsTiles);
    CopyData(gb, vTiles1, InventoryEquipmentItemsTiles, TILE_SIZE * 0x100);

    CopyData(gb, vTiles1 + 0x600, Items1Tiles + 0x3A0, TILE_SIZE * 2);

    SwitchBank(gb, 0x01);
}

void LoadMenuTiles(GBState *gb) {
    if (!gb) return;

    LoadBaseTiles(gb);

    SwitchAdjustedBank(gb, BANK_MenuTiles);
    CopyData(gb, vTiles1, MenuTiles, 0x400);

    SwitchAdjustedBank(gb, BANK_FontTiles);
    CopyData(gb, vTiles2, FontTiles, 0x800);
}
