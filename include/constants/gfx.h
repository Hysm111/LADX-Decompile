#ifndef LADX_CONSTANTS_GFX_H
#define LADX_CONSTANTS_GFX_H

#include "common.h"

/* Size of a single 8x8 2bpp tile in bytes */
#define TILE_SIZE                   0x10

/* Draw command copy modes (bits 6-7 of length/mode byte) */
#define DC_COPY_ROW                 0x00
#define DC_FILL_ROW                 0x40
#define DC_COPY_COLUMN              0x80
#define DC_FILL_COLUMN              0xC0

/* Special tile during room transitions treated as transparent */
#define TRANSITION_TRANSPARENT_TILE 0xEE

/* Map / Object Dimensions */
#define TILES_PER_MAP                0x80

/* Object Types */
#define OBJECT_SHORT_GRASS           0x04
#define OBJECT_ROCKY_GROUND          0x09
#define OBJECT_ROCKY_CAVE_DOOR       0xE1
#define OBJECT_CAVE_DOOR             0xE3

/* Indoor Door Object Types */
#define OBJECT_DOOR_TYPE_KEY_TOP        0xEC
#define OBJECT_DOOR_TYPE_KEY_BOTTOM     0xED
#define OBJECT_DOOR_TYPE_KEY_LEFT       0xEE
#define OBJECT_DOOR_TYPE_KEY_RIGHT      0xEF
#define OBJECT_DOOR_TYPE_SHUTTER_TOP    0xF0
#define OBJECT_DOOR_TYPE_SHUTTER_BOTTOM 0xF1
#define OBJECT_DOOR_TYPE_SHUTTER_LEFT   0xF2
#define OBJECT_DOOR_TYPE_SHUTTER_RIGHT  0xF3
#define OBJECT_OPEN_DOOR_TOP            0xF4
#define OBJECT_OPEN_DOOR_BOTTOM         0xF5
#define OBJECT_OPEN_DOOR_LEFT           0xF6
#define OBJECT_OPEN_DOOR_RIGHT          0xF7
#define OBJECT_DOOR_TYPE_BOSS_TOP       0xF8
#define OBJECT_STAIRS_DOOR              0xF9
#define OBJECT_FLIP_WALL                0xFA
#define OBJECT_ONE_WAY_ARROW            0xFB
#define OBJECT_DUNGEON_ENTRANCE         0xFC
#define OBJECT_INDOOR_ENTRANCE          0xFD

/* Room Object Types */
#define OBJECT_FLOOR_0D              0x0D
#define OBJECT_WALL_TOP              0x21
#define OBJECT_WALL_BOTTOM           0x22
#define OBJECT_BOMBED_PASSAGE_VERTICAL 0x3D
#define OBJECT_BOMBED_PASSAGE_HORIZONTAL 0x3E
#define OBJECT_BOMBABLE_WALL_TOP     0x3F
#define OBJECT_BOMBABLE_WALL_BOTTOM  0x40
#define OBJECT_BOMBABLE_WALL_LEFT    0x41
#define OBJECT_BOMBABLE_WALL_RIGHT   0x42
#define OBJECT_HIDDEN_BOMBABLE_WALL_TOP 0x47
#define OBJECT_HIDDEN_BOMBABLE_WALL_BOTTOM 0x48
#define OBJECT_HIDDEN_BOMBABLE_WALL_LEFT 0x49
#define OBJECT_HIDDEN_BOMBABLE_WALL_RIGHT 0x4A
#define OBJECT_WEATHER_VANE_BASE     0x5E
#define OBJECT_POT_WITH_SWITCH       0x8E
#define OBJECT_WEATHER_VANE_TOP      0x91
#define OBJECT_CHEST_CLOSED          0xA0
#define OBJECT_CHEST_OPEN            0xA1
#define OBJECT_BOMBABLE_BLOCK        0xA9
#define OBJECT_SWITCH_BUTTON         0xAA
#define OBJECT_TORCH_UNLIT           0xAB
#define OBJECT_BOMBABLE_CAVE_DOOR    0xBA
#define OBJECT_STAIRS_DOWN           0xBE
#define OBJECT_HIDDEN_STAIRS_DOWN    0xBF
#define OBJECT_CLOSED_GATE           0xC2
#define OBJECT_GROUND_STAIRS         0xC6
#define OBJECT_STAIRS_UP             0xCB
#define OBJECT_CONVEYOR_BOTTOM       0xCF
#define OBJECT_TRENDY_GAME_BORDER    0xD3
#define OBJECT_BUSH_GROUND_STAIRS    0xD3
#define OBJECT_RAISED_FENCE_TOP      0xD5
#define OBJECT_RAISED_FENCE_BOTTOM   0xD6
#define OBJECT_RAISED_FENCE_LEFT     0xD7
#define OBJECT_RAISED_FENCE_RIGHT    0xD8
#define OBJECT_MONKEY_BRIDGE_TOP     0xD8
#define OBJECT_MONKEY_BRIDGE_MIDDLE  0xD9
#define OBJECT_MONKEY_BRIDGE_BOTTOM  0xDA
#define OBJECT_MONKEY_BRIDGE_BUILT   0xDB
#define OBJECT_LOWERED_BLOCK         0xDB
#define OBJECT_RAISED_BLOCK          0xDC
#define OBJECT_WEATHER_VANE_ABOVE    0xDC
#define OBJECT_KEYHOLE_BLOCK         0xDE
#define OBJECT_WATERFALL             0xE9
#define OBJECT_MACROS_SECTION        0xF5

/* Staircase state */
#define STAIRCASE_INACTIVE           0x01

/* Switchable object types */
#define ROOM_SWITCHABLE_OBJECT_NONE          0x00
#define ROOM_SWITCHABLE_OBJECT_SWITCH_BUTTON 0x01
#define ROOM_SWITCHABLE_OBJECT_MOBILE_BLOCK  0x02

/* Graphic asset ROM addresses in Bank $08 */
#define BANK_IndoorObjectsTilemapDMG 0x08
#define IndoorObjectsTilemapDMG      0x4000
#define BANK_IndoorObjectsTilemapCGB 0x08
#define IndoorObjectsTilemapCGB      0x43B0

/* Graphic asset ROM addresses in Bank $0C */
#define BANK_LinkCharacterTiles      0x0C
#define LinkCharacterTiles           0x4000

#define BANK_Items1Tiles             0x0C
#define Items1Tiles                  0x4400

#define BANK_InventoryEquipmentItemsTiles 0x0C
#define InventoryEquipmentItemsTiles 0x4800
#define PieceOfHeartMeterTiles       0x6900

#define BANK_WorldMapTiles           0x0C
#define WorldMapTiles                0x7800

#define BANK_Overworld1Tiles         0x0C
#define Overworld1Tiles              0x4F00

/* Dungeons Graphic assets in Bank $0D */
#define BANK_DungeonsTiles           0x0D
#define DungeonSideview1Tiles        0x7000
#define DungeonSideview2Tiles        0x7800

/* NPC 1 Graphic assets in Bank $0E */
#define BANK_Npc1Tiles               0x0E
#define NpcTilesDataStart            0x4000
#define Npc1Tiles                    0x4000

/* Menu Graphic assets in Bank $0F */
#define BANK_MenuTiles               0x0F
#define MenuTiles                    0x4000

#define BANK_SaveMenuTiles           0x0F
#define SaveMenuTiles                0x4400

#define BANK_TitleLogoTiles          0x0F
#define TitleLogoTiles               0x4900

#define BANK_FontTiles               0x0F
#define FontTiles                    0x5000

/* Intro sequence graphic assets */
#define BANK_IntroRainTiles          0x01
#define IntroRainTiles               0x6D4A

#define BANK_IntroTiles              0x10
#define IntroTiles                   0x4000
#define Intro1Tiles                  0x4000
#define Intro3Tiles                  0x5400

/* Static picture, indoor, and scene graphic assets in Bank $10 */
#define BANK_IndoorTiles             0x10
#define IndoorTiles                  0x5000
#define BANK_StaticPicturesTiles     0x10
#define ChristineTiles               0x5800
#define MarinBeachTiles              0x6000
#define BANK_FontLargeTiles          0x10
#define FontLargeTiles               0x6700
#define ReliefTiles                  0x7000
#define PaintingTiles                0x7800

/* NPC 2 Graphic assets in Bank $11 */
#define BANK_Npc2Tiles               0x11
#define Npc2Tiles                    0x4000

/* Dungeon Minimap and NPC Graphic assets in Bank $12 */
#define BANK_Npc3Tiles               0x12
#define Npc3Tiles                    0x4000

#define BANK_Npc4Tiles               0x12
#define Npc4Tiles                    0x7400

#define BANK_DungeonMinimapTiles     0x12
#define DungeonMinimapTiles          0x7E00

/* Ending and Credits Graphic assets in Bank $13 */
#define BANK_EndingTiles             0x13
#define EndingTiles                  0x4000

#define BANK_EaglesTowerTop1Tiles    0x13
#define EaglesTowerTop1Tiles         0x6800
#define EaglesTowerTop2Tiles         0x7C00

/* Overworld 2 Graphic assets in Bank $0F */
#define BANK_Overworld2Tiles         0x0F
#define Overworld2Tiles              0x5A00

/* Overworld objects tilemaps in Bank $1A */
#define BANK_OverworldObjectsTilemapDMG 0x1A
#define OverworldObjectsTilemapDMG   0x6749
#define BANK_OverworldObjectsTilemapCGB 0x1A
#define OverworldObjectsTilemapCGB   0x6B1D

/* Color Dungeon objects tilemap */
#define ColorDungeonObjectsTilemap   0x4760

/* GBC specific Graphic assets in Bank $35 */
#define BANK_CameraShopIndoorTiles   0x35
#define CameraShopIndoorTiles        0x6600

#define BANK_PhotoAlbumTiles         0x35
#define PhotoAlbumTiles              0x6800

#define BANK_EndingCGBAltTiles       0x35
#define EndingCGBAltTiles            0x7800

/* Credits and Title DX Graphic assets in Bank $38 */
#define BANK_CreditsRollTiles        0x38
#define CreditsRollTiles             0x5000

#define BANK_TitleDXTiles            0x38
#define TitleDXTilesCGB              0x5800
#define TitleDXTilesDMG              0x5C00
#define TitleDXOAMTiles              0x6500

#endif /* LADX_CONSTANTS_GFX_H */
