#ifndef LADX_HOME_UI_H
#define LADX_HOME_UI_H

#include "common.h"
#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MinimapPaletteFunc)(GBState *gb);

/**
 * Loads 3 tiles of the Piece-of-Heart meter into VRAM at vTiles1 + $1A0 (part 1).
 * Corresponds to LoadPieceOfHeartMeterTiles1 (00:0062) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadPieceOfHeartMeterTiles1(GBState *gb);

/**
 * Loads 3 tiles of the Piece-of-Heart meter into VRAM at vTiles1 + $1D0 (part 2).
 * Corresponds to LoadPieceOfHeartMeterTiles2 (00:006A) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadPieceOfHeartMeterTiles2(GBState *gb);

/**
 * Restores 3 tiles of inventory equipment items to vTiles1 + $1D0 (part 1).
 * Corresponds to ClearPieceOfHeartMeterTiles1 (00:0072) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ClearPieceOfHeartMeterTiles1(GBState *gb);

/**
 * Restores 3 tiles of inventory equipment items to vTiles1 + $1A0 (part 2).
 * Corresponds to ClearPieceOfHeartMeterTiles2 (00:007A) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ClearPieceOfHeartMeterTiles2(GBState *gb);

/**
 * Copies 3 tiles ($30 bytes) to VRAM, clears BG tiles loading flags, and restores bank $0C.
 * Corresponds to CopyTilesToPieceOfHeartMeter (00:0080) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de VRAM destination address
 * @param hl ROM source address
 */
void CopyTilesToPieceOfHeartMeter(GBState *gb, uint16_t de, uint16_t hl);

/**
 * Load the dungeon minimap tiles and palettes in stages based on hBGTilesLoadingStage.
 * Stages 0-7: copies $40 bytes from DungeonMinimapTiles to vTiles1 + $500, increments stage.
 * Stage 8: switches to bank 2, calls CopyDungeonMinimapPalette, increments stage.
 * Stage 9: switches to bank 2, calls label_002_6827, increments stage.
 * Stage 10: switches to bank 2, calls label_002_680B, increments stage.
 * Stage 11+: switches to bank 2, calls label_002_67E5, clears hNeedsUpdatingBGTiles and hBGTilesLoadingStage.
 * Corresponds to LoadDungeonMinimapTiles (00:0826) in disassembly.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadDungeonMinimapTiles(GBState *gb);

/**
 * Stage-hooked version of LoadDungeonMinimapTiles allowing custom palette stage callbacks.
 */
void LoadDungeonMinimapTilesWithHooks(GBState *gb,
                                      MinimapPaletteFunc stage8,
                                      MinimapPaletteFunc stage9,
                                      MinimapPaletteFunc stage10,
                                      MinimapPaletteFunc stage11);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_UI_H */
