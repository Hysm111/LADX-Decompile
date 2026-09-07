#ifndef LADX_BANK1_FILE_MENU_H
#define LADX_BANK1_FILE_MENU_H

#include "gb.h"
#include <stdint.h>

extern const uint8_t Data_001_48E4[4];

/**
 * Updates OAM buffer with fairy pointer sprites indicating current save slot selection (01:4954).
 */
void func_001_4954(GBState *gb);

/**
 * Handles interactive navigation of the file selection menu (01:48E8).
 * Up/down selects save slots, left/right toggles Copy/Erase, A/Start validates choice.
 */
void FileSelectionInteractiveHandler(GBState *gb);

/**
 * Transitions from file selection to either File Deletion or File Copy screen (01:49DE).
 */
void HandleFileSelectionCommand(GBState *gb);

/**
 * Initiates loading of an existing saved file (01:49C3).
 * Plays validation jingle, clears palettes, clears menu BG, sets overworld tileset.
 */
void LoadSelectedFile(GBState *gb);

/**
 * Executes the choice confirmed on the file selection screen (01:4995).
 * Either opens Copy/Erase, starts creating a new file, or loads an existing save.
 */
void FileSelectionExecuteChoice(GBState *gb);

/**
 * Jumps to LoadSavedFile (01:4A04).
 */
void FileSelectionLoadSavedFile(GBState *gb);

#endif /* LADX_BANK1_FILE_MENU_H */
