#ifndef LADX_BANK1_WORLD_HANDLER_H
#define LADX_BANK1_WORLD_HANDLER_H

#include "common.h"
#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Tables */
extern const uint8_t MinimapLayoutTable[16];
extern const uint8_t Data_001_53D8[16];
extern const uint8_t Data_001_53E8[16];
extern const uint8_t Data_001_53F8[16];
extern const uint8_t Data_001_5408[16];
extern const uint8_t Data_001_5418[67];
extern const uint8_t Data_001_545C[67];
extern const uint8_t Data_001_54A0[67];
extern const uint8_t Data_001_54E4[2];
extern const uint8_t Data_001_54E6[2];
extern const uint8_t Data_001_54E8[41];

/**
 * WorldHandlerEntryPoint (01:4371)
 * State machine dispatcher for preparing and presenting the World display.
 */
void WorldHandlerEntryPoint(GBState *gb);

/**
 * GameplayWorldLoad0Handler (01:4395)
 * Unloads audio track, copies dungeon item flags to wHasDungeonMap,
 * prepares minimap or inventory layout, scrambles D4 glint sequence.
 */
void GameplayWorldLoad0Handler(GBState *gb);

/**
 * GameplayWorldLoadRoomHandler (01:442B)
 * Clears lower WRAM, resets Link motion, sets entrance coords, loads room
 * and room entities, creates NPC companion, checks compass chime condition.
 */
void GameplayWorldLoadRoomHandler(GBState *gb);
void GameplayWorldLoadRoomHandlerWithHooks(GBState *gb, uint8_t (*get_chests_status)(GBState *gb, uint8_t d, uint8_t e));

/**
 * GameplayWorldSelectTilesetHandler (01:44B4)
 * Selects room tilesets, clears BG and entity tile update flags.
 */
void GameplayWorldSelectTilesetHandler(GBState *gb);

/**
 * GameplayWorldLoadRoomTilemapHandler (01:44DB)
 * Sets wTilesetToLoad to TILESET_ROOM_TILEMAP, handles room switchable objects.
 */
void GameplayWorldLoadRoomTilemapHandler(GBState *gb);

/**
 * GameplayWorldLoadHeartsAndRupeesHandler (01:44F9)
 * Calls LoadHeartsAndRupeesCount and increments gameplay subtype.
 */
void GameplayWorldLoadHeartsAndRupeesHandler(GBState *gb);

/**
 * GameplayWorldLoadABButtonsHandler (01:4500)
 * Calls DrawABButtonSlots and increments gameplay subtype.
 */
void GameplayWorldLoadABButtonsHandler(GBState *gb);

/**
 * GameplayWorldLoad6Handler (01:4507)
 * Initializes inventory bar, turns on window display, initiates fade in.
 */
void GameplayWorldLoad6Handler(GBState *gb);

/**
 * CreateMinimapTilemap (01:5511)
 * Creates in-memory tilemap of dungeon minimap and label at wMinimapTilemap.
 */
void CreateMinimapTilemap(GBState *gb);

/**
 * func_001_5619 (01:5619)
 * Helper writing hMultiPurpose0-3 to [hl].
 */
void func_001_5619(GBState *gb, uint16_t *hl);

#ifdef __cplusplus
}
#endif

#endif /* LADX_BANK1_WORLD_HANDLER_H */
