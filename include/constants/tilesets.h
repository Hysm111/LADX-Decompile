#ifndef LADX_CONSTANTS_TILESETS_H
#define LADX_CONSTANTS_TILESETS_H

/* World tileset control constants */
#define W_TILESET_KEEP               0x0F
#define W_TILESET_CAMERA_SHOP        0x1A
#define W_TILESET_NO_UPDATE          0xFF

/* BG tile update triggers */
#define TILESET_LOAD_WORLD           0x01
#define TILESET_LOAD_DUNGEON_MINIMAP 0x02
#define TILESET_LOAD_INVENTORY       0x07
#define TILESET_LOAD_INVENTORY_SONG_1 0x08
#define TILESET_LOAD_INVENTORY_SONG_2 0x09
#define TILESET_LOAD_INVENTORY_SONG_3 0x0A

/* Tables in Bank $20 */
#define IndoorsTilesetsTable_Addr            0x6EB3
#define ColorDungeonTilesetsTable_Addr       0x70B3
#define OverworldTilesetsTable_Addr          0x6E73
#define RoomSpritesheetGroupsTable_Addr      0x70D3
#define OverworldEntitySpritesheetsTable_Addr 0x73F3
#define IndoorEntitySpritesheetsTable_Addr   0x763B

#endif /* LADX_CONSTANTS_TILESETS_H */
