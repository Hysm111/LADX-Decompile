#ifndef LADX_BANK1_WORLD_MAP_H
#define LADX_BANK1_WORLD_MAP_H

#include <stdint.h>
#include "gb.h"

extern const uint8_t MapLocationNamesTable[64];
extern const uint8_t MapSpecialLocationNamesLookupTable[80];
extern const uint8_t MapSpecialLocationNamesTable[256];

/**
 * Play validation jingle (01:49BE).
 * Sets hJingle to JINGLE_VALIDATE.
 */
void PlayValidationJingle(GBState *gb);
void PlayValidationJingleAndReturn(GBState *gb);

/**
 * Open dialog for map room special location name (01:5A59).
 * Looks up room in MapSpecialLocationNamesTable, then MapSpecialLocationNamesLookupTable,
 * and calls OpenDialogInTable0.
 */
void func_001_5A59(GBState *gb);

/**
 * World map state 0 handler (01:5648).
 * Increments gameplay subtype, on GBC copies 0x80 palette bytes from WRAM bank 1 to 3,
 * and continues into WorldMapState1Handler.
 */
void WorldMapState0Handler(GBState *gb);

/**
 * World map state 1 handler (01:5678).
 * Draws Link sprite, animates entities, fades out via func_1A22, resets scroll registers,
 * checks owl statue status for current room, disables window in LCDC, and sets wBGMapToLoad.
 */
void WorldMapState1Handler(GBState *gb);

/**
 * World map state 2 handler (01:56F4).
 * Sets wTilesetToLoad to TILESET_WORLD_MAP (0x0B) and increments gameplay subtype.
 */
void WorldMapState2Handler(GBState *gb);

/**
 * World map state 3 handler (01:56FD).
 * Sets wTilesetToLoad to TILESET_WORLD_MAP_TILEMAP (0x0E), sets wPaletteUnknownE = 1,
 * and increments gameplay subtype.
 */
void WorldMapState3Handler(GBState *gb);

/**
 * World map state 4 handler (01:570B).
 * Fades in screen via func_1A39. When transition sequence counter reaches 4,
 * increments gameplay subtype and plays validation jingle.
 */
void WorldMapState4Handler(GBState *gb);


/**
 * Play move selection jingle (01:6BAE).
 * Sets hJingle to JINGLE_MOVE_SELECTION (0x0A).
 */
void playMoveSelectionJingle(GBState *gb);

/**
 * Check up/down D-pad and play move selection jingle (01:6BA8).
 */
void MoveSelect(GBState *gb);

/**
 * Handle world map cursor movement and fog-of-war constraints (01:5A71).
 */
void func_001_5A71(GBState *gb);

/**
 * Render world map cursor brackets and flashing directional arrows to OAM (01:5B3F).
 */
void label_001_5B3F(GBState *gb);


/**
 * Animate and render corner magnifying box/icon for special map locations (01:5C55).
 */
void func_001_5C55(GBState *gb);

/**
 * Preserve wEntitiesPhysicsFlagsTable across func_001_5C55 (01:5C49).
 */
void func_001_5C49(GBState *gb);

/**
 * World map interactive handler for cursor movement, location dialogs, and exit (01:571C).
 */
void WorldMapInteractiveHandler(GBState *gb);

/**
 * World map master entry point dispatching states 0-6 (01:5626).
 */
void WorldMapEntryPoint(GBState *gb);

#endif /* LADX_BANK1_WORLD_MAP_H */


