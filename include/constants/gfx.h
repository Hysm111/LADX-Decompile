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
