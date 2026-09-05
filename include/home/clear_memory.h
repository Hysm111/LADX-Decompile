#ifndef LADX_HOME_CLEAR_MEMORY_H
#define LADX_HOME_CLEAR_MEMORY_H

#include "gb.h"
#include "constants/memory.h"
#include "constants/hardware.h"
#include "constants/dialog.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Clear BC bytes starting from address HL with zero.
 * Preserves hIsGBC across the operation.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param hl Starting address
 * @param bc Number of bytes to clear
 */
void ClearBytes(GBState *gb, uint16_t hl, uint16_t bc);

/**
 * Clear BC bytes of WRAM starting from wram0Section ($C000).
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bc Number of bytes to clear
 */
void ClearWRAMBytes(GBState *gb, uint16_t bc);

/**
 * Clear BC bytes in HRAM starting from hGameValuesSection ($FF90)
 * and clear all WRAM from $C000 to $DF00 ($1F00 bytes), preserving the stack.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param bc Number of bytes to clear in HRAM
 */
void ClearHRAMBytesAndWRAM(GBState *gb, uint16_t bc);

/**
 * Clear all game variables in HRAM ($FF90 to $FFFC) and all WRAM ($C000 to $DF00).
 * Preserves hIsGBC ($FFFE).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ClearHRAMAndWRAM(GBState *gb);

/**
 * Clear lower HRAM ($FF90 to $FFBE) and all WRAM ($C000 to $DF00).
 * Preserves hNextDefaultMusicTrack ($FFBF) and hIsGBC ($FFFE).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ClearWRAMAndLowerHRAM(GBState *gb);

/**
 * Clear lower WRAM ($C000 to $D2FF, $1300 bytes) up to wAudioSection.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ClearLowerWRAM(GBState *gb);

/**
 * Clear lower and middle WRAM ($C000 to $D5FF, $1600 bytes) up to wDrawCommandsSection.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void ClearLowerAndMiddleWRAM(GBState *gb);

/**
 * Fill Background Map 0 (vBGMap0 = $9800) with a specified tile value for count bytes.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param val Tile value to fill
 * @param count Number of bytes to fill
 */
void FillBGMap(GBState *gb, uint8_t val, uint16_t count);

/**
 * Fill Background Map 0 with black tiles (DIALOG_BG_TILE_DARK = $7E) for $400 bytes.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void FillBGMapBlack(GBState *gb);

/**
 * Fill Background Map 0 and 1 with white tiles ($7F) for $800 bytes ($9800 - $9FFF).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void FillBGMapWhite(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_CLEAR_MEMORY_H */
