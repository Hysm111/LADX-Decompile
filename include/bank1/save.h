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


/**
 * Main entry point for loading a saved game (01:52A4).
 * Handles health reset, save data loading from SRAM, wrecking ball initialization,
 * and spawn location configuration (or new game initialization in Marin's house).
 */
void LoadSavedFile(GBState *gb);

extern const uint8_t DebugSaveFileData[67];
extern const uint8_t Data_001_4128[2];

/**
 * Validates save slot prefix and initializes SRAM slot if corrupt (01:4794).
 */
void func_001_4794(GBState *gb, uint16_t de);

/**
 * Initializes all save slots in SRAM, and loads debug save file if ROM_DebugTool1 is set (01:46AA).
 */
void InitSaveFiles(GBState *gb);

/**
 * Save screen initial handler (01:4012).
 */
void FileSaveInitial(GBState *gb);

/**
 * Save screen map fade out handler (01:4042).
 */
void FileSaveMapFadeOut(GBState *gb);

/**
 * Save screen delay stage 1 (01:4073).
 */
void FileSaveDelay1(GBState *gb);

/**
 * Save screen delay stage 2 (01:407F).
 */
void FileSaveDelay2(GBState *gb);

/**
 * Save screen visible stage (01:409C).
 */
void FileSaveVisible(GBState *gb);

/**
 * Helper for save screen option navigation and cursor arrow rendering (01:412A).
 */
void func_001_412A(GBState *gb);

/**
 * Save screen interactive selection handler ("Return to Game" vs "Save and Quit") (01:40AA).
 */
void FileSaveInteractive(GBState *gb);

/**
 * Enables LCD and configures screen settings upon save & quit or return (01:410D).
 */
void LCDOn(GBState *gb);

/**
 * Master entry point dispatcher for GAMEPLAY_FILE_SAVE (01:4000).
 */
void FileSaveEntryPoint(GBState *gb);

#endif /* LADX_BANK1_SAVE_H */


