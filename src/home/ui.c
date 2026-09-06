#include "home/ui.h"
#include "home/copy_data.h"
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
