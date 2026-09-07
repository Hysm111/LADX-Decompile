#ifndef LADX_BANK1_SAVE_H
#define LADX_BANK1_SAVE_H

#include "gb.h"
#include <stdint.h>

#define SAVE_PREFIX_SIZE  5
#define SAVE_MAIN_SIZE    0x0380
#define SAVE_DX1_SIZE     0x0005
#define SAVE_DX2_SIZE     0x0020
#define SAVE_DX3_SIZE     0x0003

extern const uint16_t SaveGameTable[3];
extern const uint8_t MaxHeartsToStartingHealthTable[15];

/**
 * Builds a wDrawCommand to draw hearts next to the save files on the file selection screen (01:5D53).
 * Arguments:
 * - hMultiPurpose2 = Health
 * - hMultiPurpose3 = Max health
 * - hMultiPurpose4 = Save slot (0, 1, 2)
 */
void BuildSaveSlotHeartsDrawCommand(GBState *gb);

/**
 * Counts existing save files by scanning name slots in wSaveSlotNames and updating wSaveFilesCount (01:5DC0).
 */
void func_5DC0(GBState *gb);

/**
 * Saves game data for the slot in wSaveSlot into SRAM (01:5DE6).
 */
void SaveGameToFile(GBState *gb);

#endif /* LADX_BANK1_SAVE_H */
