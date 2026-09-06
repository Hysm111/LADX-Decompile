#ifndef LADX_HOME_GFX_H
#define LADX_HOME_GFX_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LoadCreditsKoholintDisappearingTiles (00:2A37)
 * Loads Koholint disappearing animation tiles from EndingTiles into vTiles2 and vTiles1.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsKoholintDisappearingTiles(GBState *gb);

/**
 * LoadCreditsStairsTiles (00:2A57)
 * Loads tileset 15, then loads credits stairs tiles into vTiles0 + $400.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsStairsTiles(GBState *gb);

/**
 * LoadTileset15 (00:2A66)
 * Loads tileset 15 for credits sequence:
 * - EndingTiles to vTiles0 ($180 tiles)
 * - Overworld1Tiles to vTiles2 + $7F0 (1 tile filler)
 * - Npc4Tiles to vTiles0 (4 tiles)
 * - Npc4Tiles to vTiles1 + $500 ($20 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadTileset15(GBState *gb);

/**
 * LoadCreditsKoholintViewsTiles (00:2AAE)
 * Copies tiles for the various Koholint views while the instruments are playing:
 * - Overworld1Tiles + $100 to vTiles2 ($80 tiles)
 * - Npc3Tiles + $2000 to vTiles0 ($80 tiles)
 * - Overworld2Tiles + $600 to vTiles1 ($80 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsKoholintViewsTiles(GBState *gb);

/**
 * LoadCreditsLinkOnSeaCloseTiles (00:2AEA)
 * Loads tiles for Link on sea (close-up) during credits:
 * - On DMG: EndingTiles to vTiles0 ($80 tiles)
 * - On GBC: PhotoAlbumTiles to vTiles0 ($80 tiles)
 * - Followed by EndingTiles + $1800 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkOnSeaCloseTiles(GBState *gb);

/**
 * LoadCreditsSunAboveTiles (00:2AF9)
 * Loads tiles for sun above sea view during credits:
 * - EndingTiles + $800 to vTiles0 ($80 tiles)
 * - EndingTiles + $1800 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsSunAboveTiles(GBState *gb);

/**
 * LoadCreditsLinkOnSeaLargeTiles (00:2AFE)
 * Loads tiles for large Link on sea view during credits:
 * - EndingTiles + $2000 to vTiles0 ($80 tiles)
 * - EndingTiles + $1800 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkOnSeaLargeTiles(GBState *gb);

/**
 * LoadCreditsRollTiles (00:2B26)
 * Loads tiles for the rolling credits sequence. Steps audio in between tile copy blocks.
 * Copies large font tiles to vTiles0 via func_2B92, Npc3 tiles to vTiles0,
 * and font or credits roll tiles to vTiles0 depending on GBC mode.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsRollTiles(GBState *gb);

/**
 * LoadCreditsLinkFaceCloseUpTiles (00:2B72)
 * Loads Link face close-up tiles during ending.
 * DMG: EndingTiles + $3800 (bank $13, adjusted).
 * GBC: EndingCGBAltTiles (bank $35).
 * Followed by common ending tiles to vTiles1 and vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkFaceCloseUpTiles(GBState *gb);

/**
 * LoadCreditsLinkSeatedOnLogTiles (00:2B81)
 * Loads Link seated on log tiles during ending.
 * DMG: EndingTiles + $800 (bank $13, adjusted).
 * GBC: PhotoAlbumTiles + $800 (bank $35).
 * Followed by common ending tiles to vTiles1 and vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadCreditsLinkSeatedOnLogTiles(GBState *gb);

/**
 * func_2B92 (00:2B92)
 * Adjusts bank number for GBC, sets rSelectROMBank,
 * copies 0x80 tiles from src_hl to vTiles0,
 * then copies 0x80 tiles from EndingTiles + $3000 to vTiles1,
 * and 0x80 tiles from EndingTiles + $2800 to vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param src_hl Source ROM address for vTiles0
 * @param bank ROM bank to adjust and select
 */
void func_2B92(GBState *gb, uint16_t src_hl, uint8_t bank);

/**
 * LoadBaseTiles (00:2BCF)
 * Loads basic tiles (Link sprite tiles and inventory equipment icons) into VRAM:
 * - LinkCharacterTiles (bank $0C) to vTiles0 ($40 tiles)
 * - InventoryEquipmentItemsTiles (bank $0C) to vTiles1 ($100 tiles)
 * - Items1Tiles + $3A0 to vTiles1 + $600 (2 tiles)
 * Switches back to bank $01 before returning.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadBaseTiles(GBState *gb);

/**
 * LoadMenuTiles (00:2C03)
 * Calls LoadBaseTiles, then loads menu UI tiles and font tiles:
 * - MenuTiles (bank $0F) to vTiles1 ($400 bytes / $40 tiles)
 * - FontTiles (bank $0F) to vTiles2 ($800 bytes / $80 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadMenuTiles(GBState *gb);

/**
 * LoadIntroSequenceTiles (00:2D79)
 * Loads opening sequence tiles:
 * - Rain tiles from bank $01 to vTiles0 + $700 (8 tiles)
 * - Intro3 tiles from bank $10 to vTiles0 ($60 tiles)
 * - Intro1 tiles from bank $10 to vTiles1 ($100 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadIntroSequenceTiles(GBState *gb);

/**
 * LoadTitleScreenTiles (00:2DA7)
 * Loads title screen tiles:
 * - Title logo from bank $0F to vTiles1 ($70 tiles)
 * - "DX" text tiles from bank $38 (DMG: TitleDXTilesDMG, CGB: TitleDXTilesCGB) to vTiles0 + $400 ($40 tiles)
 * - "DX" text OAM tiles from bank $38 to vTiles0 + $200 (16 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadTitleScreenTiles(GBState *gb);

/**
 * LoadWorldMapTiles (00:2DE9)
 * Loads world map overview screen tiles:
 * - WorldMapTiles from bank $0C to vTiles1 + $700 ($80 tiles)
 * - Overworld1Tiles + $100 to vTiles0 + $200 (16 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadWorldMapTiles(GBState *gb);

/**
 * LoadStaticPictureTiles (00:2E13)
 * Copies 0x80 tiles from static picture address in bank $10 to vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param src_hl Source ROM address in bank $10
 */
void LoadStaticPictureTiles(GBState *gb, uint16_t src_hl);

/**
 * LoadFaceShrineReliefTiles (00:2E06)
 * Loads Face Shrine relief picture to vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadFaceShrineReliefTiles(GBState *gb);

/**
 * LoadSchulePaintingTiles (00:2E0B)
 * Loads Schule painting picture to vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadSchulePaintingTiles(GBState *gb);

/**
 * LoadChristinePortraitTiles (00:2E10)
 * Loads Christine portrait picture to vTiles2.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadChristinePortraitTiles(GBState *gb);

/**
 * LoadEaglesTowerTopTiles (00:2E21)
 * Loads Eagle's Tower collapse/boss platform top tiles:
 * - EaglesTowerTop2Tiles (bank $13) to vTiles1 + $400 ($40 tiles)
 * - EaglesTowerTop1Tiles (bank $13) to vTiles2 ($40 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadEaglesTowerTopTiles(GBState *gb);

/**
 * LoadMarinBeachTiles (00:2E41)
 * Loads Marin at the beach sequence tiles:
 * - Large font tiles from bank $10 to vTiles0 + $400 ($40 tiles)
 * - Marin beach artwork from bank $10 to vTiles2 ($60 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadMarinBeachTiles(GBState *gb);

/**
 * LoadSaveMenuTiles (00:2E5E)
 * Loads save and game over menu tiles:
 * - SaveMenuTiles from bank $0F to vTiles1 ($50 tiles)
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadSaveMenuTiles(GBState *gb);

/**
 * LoadRoomSpecificTiles (00:2E73)
 * For overworld or indoor rooms, loads room-specific tiles:
 * - 4 rows of entity spritesheets to vTiles0 + $400 (NPCs tiles) with follower override
 * - 8 rows of BG tiles to vTiles2 based on dungeon/overworld/side-scrolling context
 *
 * @param gb Pointer to Game Boy hardware state
 * @param load_color_dungeon_tiles Optional callback for Color Dungeon tile loading in bank $20
 */
void LoadRoomSpecificTiles(GBState *gb, void (*load_color_dungeon_tiles)(GBState *));

/**
 * CopyWord (00:2FC7)
 * Copies two consecutive bytes from hl to de.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Destination memory address
 * @param hl Source memory address
 */
void CopyWord(GBState *gb, uint16_t de, uint16_t hl);

/**
 * WriteObjectToBG_DMG (00:2FCD)
 * Given an object pointer in wRoomObjects, retrieves its 2x2 tile indices from the
 * appropriate objects tilemap and writes them to the BG map.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Target address in BG map
 * @param hl Pointer to object byte in wRoomObjects
 */
void WriteObjectToBG_DMG(GBState *gb, uint16_t de, uint16_t hl);

/**
 * SwitchToObjectsTilemapBank (00:3905)
 * Switches rSelectROMBank to BANK(IndoorObjectsTilemapDMG) ($08) if indoors,
 * or BANK(OverworldObjectsTilemapDMG) ($1A) if outdoors.
 *
 * @param gb Pointer to Game Boy hardware state
 */
uint8_t SwitchToObjectsTilemapBank(GBState *gb);

/**
 * WriteOverworldObjectToBG (00:300E)
 * Given an overworld object pointer in wRoomObjects, reads its attribute value
 * from WRAM bank 2 and writes the 2x2 tile indices and palettes to the BG map.
 * (CGB only)
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Target address in BG map
 * @param hl Pointer to object byte in wRoomObjects
 * @param get_bg_attributes Callback to Bank $1A GetBGAttributesAddressForObject
 */
void WriteOverworldObjectToBG(GBState *gb, uint16_t de, uint16_t hl,
                              void (*get_bg_attributes)(GBState *, uint16_t, uint16_t));

/**
 * WriteIndoorObjectToBG (00:3018)
 * Given an indoor object pointer in wRoomObjects, reads its attribute value
 * from current WRAM bank and writes the 2x2 tile indices and palettes to the BG map.
 * (CGB only)
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Target address in BG map
 * @param hl Pointer to object byte in wRoomObjects
 * @param get_bg_attributes Callback to Bank $1A GetBGAttributesAddressForObject
 */
void WriteIndoorObjectToBG(GBState *gb, uint16_t de, uint16_t hl,
                            void (*get_bg_attributes)(GBState *, uint16_t, uint16_t));

/**
 * doCopyObjectToBG (00:3019)
 * Internal helper: copies 2x2 tiles and attributes for an object to BG map.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param de Target address in BG map
 * @param hl Pointer to object byte in wRoomObjects
 * @param obj_attr_val Object attribute value read from object map
 * @param get_bg_attributes Callback to Bank $1A GetBGAttributesAddressForObject
 */
void doCopyObjectToBG(GBState *gb, uint16_t de, uint16_t hl, uint8_t obj_attr_val,
                      void (*get_bg_attributes)(GBState *, uint16_t, uint16_t));

/**
 * LoadRoomTilemap (00:309B)
 * Copies the entire room tilemap (128 objects) to BG video memory (vBGMap0).
 * Handles both DMG (WriteObjectToBG_DMG) and CGB (WriteIndoorObjectToBG / WriteOverworldObjectToBG).
 * At completion, jumps to UpdateMinimapEntranceArrowAndReturn in bank 1.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param get_bg_attributes Callback to Bank $1A GetBGAttributesAddressForObject
 * @param update_minimap_arrow Callback to Bank $01 UpdateMinimapEntranceArrowAndReturn
 */
void LoadRoomTilemap(GBState *gb,
                     void (*get_bg_attributes)(GBState *, uint16_t, uint16_t),
                     void (*update_minimap_arrow)(GBState *));

/**
 * LoadCreditsMarinPortraitTiles_trampoline (00:3915)
 * Switches to BANK(LoadCreditsMarinPortraitTiles) ($27) and jumps to LoadCreditsMarinPortraitTiles.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param load_marin_portrait Callback to LoadCreditsMarinPortraitTiles in Bank $27
 */
void LoadCreditsMarinPortraitTiles_trampoline(GBState *gb, void (*load_marin_portrait)(GBState *));

/**
 * LoadThanksForPlayingTiles_trampoline (00:391D)
 * Switches to BANK(LoadThanksForPlayingTiles) ($20) and jumps to LoadThanksForPlayingTiles.
 *
 * @param gb Pointer to Game Boy hardware state
 * @param load_thanks Callback to LoadThanksForPlayingTiles in Bank $20
 */
void LoadThanksForPlayingTiles_trampoline(GBState *gb, void (*load_thanks)(GBState *));


/**
 * func_2D50 (00:2D50)
 * Clears hAnimatedTilesFrameCount and hAnimatedTilesDataOffset,
 * invokes AnimateTiles, copies InventoryEquipmentItemsTiles
 * to vTiles1 ($80 tiles), and LinkCharacterTiles + $200 to vTiles0 + $200 (16 tiles).
 *
 * @param gb Pointer to Game Boy hardware state
 */
void func_2D50(GBState *gb);

/**
 * PatchInventoryTiles (00:2CFE)
 * Shared subroutine for patching inventory tiles in VRAM based on progression flags:
 * - Toadstool replaces Magic Powder if wHasToadstool != 0
 * - Golden Leaf replaces Slime Key if indoor conditions match and wGoldenLeavesCount >= SLIME_KEY
 * - Sets hReplaceTiles = REPLACE_TILES_TRADING_ITEM if wTradeSequenceItem >= TRADING_ITEM_RIBBON
 *
 * @param gb Pointer to Game Boy hardware state
 */
void PatchInventoryTiles(GBState *gb);

/**
 * LoadBaseOverworldTiles (00:2D2D)
 * Copies default Overworld tiles to VRAM:
 * - OverworldLandscapeTiles (bank $0C) to vTiles2 + $200 ($60 tiles)
 * - InventoryOverworldItemsTiles (bank $0C) to vTiles1 + $400 ($40 tiles)
 * Calls func_2D50 and PatchInventoryTiles.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadBaseOverworldTiles(GBState *gb);

/**
 * LoadIndoorTiles (00:2C28)
 * Copies tiles for an indoor room (floor, objects, walls, items, inventory) to VRAM.
 * Reads floor, wall, and item pointers from tables in Bank $20,
 * handles Color Dungeon special cases, calls func_2D50, and calls PatchInventoryTiles.
 *
 * @param gb Pointer to Game Boy hardware state
 */
void LoadIndoorTiles(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_GFX_H */
