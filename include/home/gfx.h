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

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_GFX_H */
