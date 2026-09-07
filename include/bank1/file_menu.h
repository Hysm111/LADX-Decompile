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

#define FILE_NEW_SAVE_SLOT_INDEX_BG   0x9849
#define FILE_NEW_SAVE_SLOT_1_TILE     0xAB

extern const uint8_t SaveSlotNameAddresses[3];
extern const uint16_t Data_001_49F2[3];

/**
 * Stage 0 init for file creation screen (01:4A11).
 * Sets tileset to TILESET_FILL_TILEMAP, clears input index and name entry char, increments subtype.
 */
void FileCreationInit1Handler(GBState *gb);

/**
 * Stage 1 init for file creation screen (01:4A24).
 * Sets BG map to TILEMAP_MENU_FILE_CREATION, writes draw command for slot index tile, increments subtype.
 */
void FileCreationInit2Handler(GBState *gb);

/**
 * Writes a single byte into SRAM at [hl + bc] (01:4A3F).
 */
void WriteByteToSRAM(GBState *gb, uint16_t hl, uint16_t bc, uint8_t val);

/**
 * Reloads all save slot names, health, hearts, and deaths from SRAM and switches to file select (01:4555).
 */
void label_001_4555(GBState *gb);

/**
 * Sets wForceFileSelectionScreenMusic to force_music and reloads file selection menu (01:4552).
 */
void TransitionToFileMenu(GBState *gb, uint8_t force_music);

extern const uint8_t Data_001_4B30[64];
extern const uint8_t Data_001_4B70[64];
extern const uint8_t Data_001_4BB0[5];
extern const uint8_t NameEntryCharacterTable[64];

/**
 * Appends draw commands to display 5-character save slot name and empty spacing row on BG map (01:4852).
 */
void DrawSaveSlotName(GBState *gb, uint16_t dest_bg, uint16_t name_addr);

/**
 * Navigates 16x4 character grid for name entry, handles repeating directional inputs, and draws cursor (01:4BF5).
 */
void func_001_4BF5(GBState *gb);

/**
 * Handles A/B buttons for character entry/backspace and draws blinking underline cursor (01:4C8A).
 */
void func_001_4C8A(GBState *gb);

/**
 * Inserts current character from NameEntryCharacterTable into current slot name (01:4CDA).
 */
void func_001_4CDA(GBState *gb);

/**
 * Interactive handler for file creation screen (01:4A9B).
 */
void FileCreationInteractiveHandler(GBState *gb);

/**
 * Entry point and dispatcher for file creation (01:4A07).
 */
void FileCreationEntryPoint(GBState *gb);

extern const uint8_t Data_001_4F3B[10];

/**
 * Appends a 3-digit number draw command (BCD) to wDrawCommand queue (01:4F45).
 */
void CopyDigitsToFileScreenBG(GBState *gb, uint16_t dest_bg, uint8_t high_b, uint8_t low_c);

/**
 * Copies death counters for active save files into BG map (01:480C).
 */
void CopyDeathCountsToBG(GBState *gb);

/**
 * Draw save slot names for slots 1, 2, 3 (01:4D8B, 01:4D94, 01:4D9D).
 */
void DrawSaveSlot1Name(GBState *gb);
void DrawSaveSlot2Name(GBState *gb);
void DrawSaveSlot3Name(GBState *gb);

/**
 * Draw max hearts for save slots 1, 2, 3 (01:4DA6, 01:4DBE, 01:4DD6).
 */
void DrawSaveSlot1MaxHearts(GBState *gb);
void DrawSaveSlot2MaxHearts(GBState *gb);
void DrawSaveSlot3MaxHearts(GBState *gb);

/**
 * File selection menu preparation stages 2 and 3 (01:47FD, 01:4806).
 */
void FileSelectionPrepare2(GBState *gb);
void FileSelectionPrepare3(GBState *gb);

/**
 * File deletion state handlers 0 through 9 (01:4D1A-01:4D88).
 */
void FileDeletionState0Handler(GBState *gb);
void FileDeletionState1Handler(GBState *gb);
void FileDeletionState2Handler(GBState *gb);
void FileDeletionState3Handler(GBState *gb);
void FileDeletionState4Handler(GBState *gb);
void FileDeletionState5Handler(GBState *gb);
void FileDeletionState6Handler(GBState *gb);
void FileDeletionState7Handler(GBState *gb);
void FileDeletionState8Handler(GBState *gb);
void FileDeletionState9Handler(GBState *gb);

#endif /* LADX_BANK1_FILE_MENU_H */



