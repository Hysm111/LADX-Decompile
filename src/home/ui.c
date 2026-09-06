#include "home/ui.h"
#include "home/copy_data.h"
#include "home/bank.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/gfx.h"

void CopyTilesToPieceOfHeartMeter(GBState *gb, uint16_t de, uint16_t hl) {
    if (!gb) return;

    CopyData(gb, de, hl, 0x30);
    gb_write(gb, hNeedsUpdatingBGTiles, 0);
    gb_write(gb, hBGTilesLoadingStage, 0);
    gb_write(gb, rSelectROMBank, 0x0C);
}

void LoadPieceOfHeartMeterTiles1(GBState *gb) {
    if (!gb) return;

    CopyTilesToPieceOfHeartMeter(gb, vTiles1 + 0x1A0, PieceOfHeartMeterTiles);
}

void LoadPieceOfHeartMeterTiles2(GBState *gb) {
    if (!gb) return;

    CopyTilesToPieceOfHeartMeter(gb, vTiles1 + 0x1D0, PieceOfHeartMeterTiles + 0x30);
}

void ClearPieceOfHeartMeterTiles1(GBState *gb) {
    if (!gb) return;

    CopyTilesToPieceOfHeartMeter(gb, vTiles1 + 0x1D0, InventoryEquipmentItemsTiles + 0x1D0);
}

void ClearPieceOfHeartMeterTiles2(GBState *gb) {
    if (!gb) return;

    CopyTilesToPieceOfHeartMeter(gb, vTiles1 + 0x1A0, InventoryEquipmentItemsTiles + 0x1A0);
}

void LoadDungeonMinimapTilesWithHooks(GBState *gb,
                                      MinimapPaletteFunc stage8,
                                      MinimapPaletteFunc stage9,
                                      MinimapPaletteFunc stage10,
                                      MinimapPaletteFunc stage11) {
    if (!gb) return;

    uint8_t bank = AdjustBankNumberForGBC(gb, BANK_DungeonMinimapTiles);
    gb_write(gb, rSelectROMBank, bank);

    uint8_t stage = gb_read(gb, hBGTilesLoadingStage);
    if (stage < 8) {
        uint16_t offset = (uint16_t)stage * 0x40;
        uint16_t dest = (vTiles1 + 0x500) + offset;
        uint16_t src = DungeonMinimapTiles + offset;
        CopyData(gb, dest, src, 0x40);
        gb_write(gb, hBGTilesLoadingStage, stage + 1);
        return;
    }

    if (stage == 8) {
        gb_write(gb, rSelectROMBank, 0x02);
        if (stage8) {
            stage8(gb);
        }
        gb_write(gb, hBGTilesLoadingStage, stage + 1);
        return;
    }

    if (stage == 9) {
        gb_write(gb, rSelectROMBank, 0x02);
        if (stage9) {
            stage9(gb);
        }
        gb_write(gb, hBGTilesLoadingStage, stage + 1);
        return;
    }

    if (stage == 10) {
        gb_write(gb, rSelectROMBank, 0x02);
        if (stage10) {
            stage10(gb);
        }
        gb_write(gb, hBGTilesLoadingStage, stage + 1);
        return;
    }

    /* stage >= 11 */
    gb_write(gb, rSelectROMBank, 0x02);
    if (stage11) {
        stage11(gb);
    }
    gb_write(gb, hNeedsUpdatingBGTiles, 0);
    gb_write(gb, hBGTilesLoadingStage, 0);
}

void LoadDungeonMinimapTiles(GBState *gb) {
    LoadDungeonMinimapTilesWithHooks(gb, NULL, NULL, NULL, NULL);
}
