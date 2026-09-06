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

/* Graphic asset ROM addresses in Bank $0C */
#define BANK_LinkCharacterTiles      0x0C
#define LinkCharacterTiles           0x4000

#define BANK_Items1Tiles             0x0C
#define Items1Tiles                  0x4400

#define BANK_InventoryEquipmentItemsTiles 0x0C
#define InventoryEquipmentItemsTiles 0x4800
#define PieceOfHeartMeterTiles       0x6900

/* Menu Graphic assets in Bank $0F */
#define BANK_MenuTiles               0x0F
#define MenuTiles                    0x4000

/* Dungeon Minimap Graphic assets */
#define BANK_DungeonMinimapTiles     0x12
#define DungeonMinimapTiles          0x7E00

/* Font Graphic assets */
#define BANK_FontTiles               0x0F
#define FontTiles                    0x5000

#define BANK_FontLargeTiles          0x10
#define FontLargeTiles               0x6700

/* Ending and Credits Graphic assets */
#define BANK_EndingTiles             0x13
#define EndingTiles                  0x4000

#define BANK_EndingCGBAltTiles       0x35
#define EndingCGBAltTiles            0x7800

#define BANK_CreditsRollTiles        0x38
#define CreditsRollTiles             0x5000

#define BANK_Overworld1Tiles         0x0C
#define Overworld1Tiles              0x4F00

#define BANK_Npc4Tiles               0x12
#define Npc4Tiles                    0x7400

#define BANK_Npc3Tiles               0x12
#define Npc3Tiles                    0x4000

#define BANK_Overworld2Tiles         0x0F
#define Overworld2Tiles              0x5A00

#define BANK_PhotoAlbumTiles         0x35
#define PhotoAlbumTiles              0x6800

#endif /* LADX_CONSTANTS_GFX_H */
