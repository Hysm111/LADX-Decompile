#ifndef LADX_HOME_GFX_H
#define LADX_HOME_GFX_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LoadCreditsKoholintDisappearingTiles (00:2A37)
 * Loads Koholint disappearing animation tiles from EndingTiles into vTiles2 and vTiles1.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsKoholintDisappearingTiles(GBState *gb);

/**
 * LoadCreditsStairsTiles (00:2A57)
 * Loads tileset 15, then loads credits stairs tiles into vTiles0 + $400.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsStairsTiles(GBState *gb);

/**
 * LoadTileset15 (00:2A66)
 * Loads tileset 15 for credits sequence:
 * - EndingTiles to vTiles0 ($180 tiles)
 * - Overworld1Tiles to vTiles2 + $7F0 (1 tile filler)
 * - Npc4Tiles to vTiles0 (4 tiles)
 * - Npc4Tiles to vTiles1 + $500 ($20 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadTileset15(GBState *gb);

/**
 * LoadCreditsKoholintViewsTiles (00:2AAE)
 * Copies tiles for the various Koholint views while the instruments are playing:
 * - Overworld1Tiles + $100 to vTiles2 ($80 tiles)
 * - Npc3Tiles + $2000 to vTiles0 ($80 tiles)
 * - Overworld2Tiles + $600 to vTiles1 ($80 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsKoholintViewsTiles(GBState *gb);

/**
 * LoadCreditsLinkOnSeaCloseTiles (00:2AEA)
 * Loads tiles for Link on sea (close-up) during credits:
 * - On DMG: EndingTiles to vTiles0 ($80 tiles)
 * - On GBC: PhotoAlbumTiles to vTiles0 ($80 tiles)
 * - Followed by EndingTiles + $1800 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkOnSeaCloseTiles(GBState *gb);

/**
 * LoadCreditsSunAboveTiles (00:2AF9)
 * Loads tiles for sun above sea view during credits:
 * - EndingTiles + $800 to vTiles0 ($80 tiles)
 * - EndingTiles + $1800 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsSunAboveTiles(GBState *gb);

/**
 * LoadCreditsLinkOnSeaLargeTiles (00:2AFE)
 * Loads tiles for large Link on sea view during credits:
 * - EndingTiles + $2000 to vTiles0 ($80 tiles)
 * - EndingTiles + $1800 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkOnSeaLargeTiles(GBState *gb);

/**
 * LoadCreditsRollTiles (00:2B26)
 * Loads tiles for the rolling credits sequence. Steps audio in between tile copy blocks.
 * Copies large font tiles to vTiles0 via func_2B92, Npc3 tiles to vTiles0,
 * and font or credits roll tiles to vTiles0 depending on GBC mode.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsRollTiles(GBState *gb);

/**
 * LoadCreditsLinkFaceCloseUpTiles (00:2B72)
 * Loads Link face close-up tiles during ending.
 * DMG: EndingTiles + $3800 (bank $13, adjusted).
 * GBC: EndingCGBAltTiles (bank $35).
 * Followed by common ending tiles to vTiles1 and vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkFaceCloseUpTiles(GBState *gb);

/**
 * LoadCreditsLinkSeatedOnLogTiles (00:2B81)
 * Loads Link seated on log tiles during ending.
 * DMG: EndingTiles + $800 (bank $13, adjusted).
 * GBC: PhotoAlbumTiles + $800 (bank $35).
 * Followed by common ending tiles to vTiles1 and vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkSeatedOnLogTiles(GBState *gb);

/**
 * func_2B92 (00:2B92)
 * Adjusts bank number for GBC, sets rSelectROMBank,
 * copies 0x80 tiles from src_hl to vTiles0,
 * then copies 0x80 tiles from EndingTiles + $3000 to vTiles1,
 * and 0x80 tiles from EndingTiles + $2800 to vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param src_hl Source ROM address for vTiles0
 * @param bank ROM bank to adjust and select
 */
void func_2B92(GBState *gb, uint16_t src_hl, uint8_t bank);

/**
 * LoadBaseTiles (00:2BCF)
 * Loads basic tiles (Link sprite tiles and inventory equipment icons) into VRAM:
 * - LinkCharacterTiles (bank $0C) to vTiles0 ($40 tiles)
 * - InventoryEquipmentItemsTiles (bank $0C) to vTiles1 ($100 tiles)
 * - Items1Tiles + $3A0 to vTiles1 + $600 (2 tiles)
 * Switches back to bank $01 before returning.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadBaseTiles(GBState *gb);

/**
 * LoadMenuTiles (00:2C03)
 * Calls LoadBaseTiles, then loads menu UI tiles and font tiles:
 * - MenuTiles (bank $0F) to vTiles1 ($400 bytes / $40 tiles)
 * - FontTiles (bank $0F) to vTiles2 ($800 bytes / $80 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadMenuTiles(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_GFX_H */
